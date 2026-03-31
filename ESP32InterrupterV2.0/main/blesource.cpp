#include <Arduino.h>
#include <BLEDevice.h>
#include "blesource.h"
#include "player.h"
#include "data.h"
#include "lcd.h"
#include "util.h"
#include "constants.h"

// BLE MIDI UUIDs (standard)
static const char* MIDI_SERVICE_UUID = "03B80E5A-EDE8-4B33-A751-6CE34EC4C700";
static const char* MIDI_CHAR_UUID    = "7772E5DB-3868-4112-A1A9-F2669D106BF3";

static BLECharacteristic* midiChar = nullptr;
static BLEServer* bleServer = nullptr;
static volatile bool bleClientConnected = false;
static volatile bool bleStop = false;
static const uint8_t PRIORITY_CH_A = 0;   // MIDI ch 1
static const uint8_t PRIORITY_CH_B = 1;   // MIDI ch 2
static const uint8_t NON_PRIORITY_GAIN_NUM = 1; // 1/2 velocity
static const uint8_t NON_PRIORITY_GAIN_DEN = 2;

// Raw MIDI byte queue (fed by BLE callback, consumed in blesource_run loop).
static uint8_t rxBuf[1024];
static volatile uint16_t rxHead = 0;
static volatile uint16_t rxTail = 0;
static portMUX_TYPE bleRxMux = portMUX_INITIALIZER_UNLOCKED;
static const uint16_t RX_MASK = 0x03FF;

static inline void enqueueRawByte(uint8_t b) {
  portENTER_CRITICAL(&bleRxMux);
  uint16_t next = (uint16_t)((rxHead + 1) & RX_MASK);
  if (next == rxTail) {
    // Overflow strategy for realtime MIDI: keep newest bytes, drop oldest.
    rxTail = (uint16_t)((rxTail + 1) & RX_MASK);
  }
  rxBuf[rxHead] = b;
  rxHead = next;
  portEXIT_CRITICAL(&bleRxMux);
}

static inline bool dequeueRawByte(uint8_t* out) {
  bool ok = false;
  portENTER_CRITICAL(&bleRxMux);
  if (rxTail != rxHead) {
    *out = rxBuf[rxTail];
    rxTail = (uint16_t)((rxTail + 1) & RX_MASK);
    ok = true;
  }
  portEXIT_CRITICAL(&bleRxMux);
  return ok;
}

static void enqueueBlePacket(const uint8_t* data, size_t n) {
  static bool runningSupported = false; // running-status source supported by our parser
  if (n < 2) return;

  size_t i = 1; // Skip header timestamp-high byte.
  while (i < n) {
    uint8_t b = data[i];

    // Each BLE-MIDI event should start with timestamp-low (MSB set).
    if ((b & 0x80) == 0) {
      i++;
      continue;
    }
    i++;
    if (i >= n) break;

    // Optional explicit status at start of event.
    bool enqueueData = true;
    b = data[i];
    if (b & 0x80) {
      uint8_t msn = b & 0xF0;
      if (msn == 0x80 || msn == 0x90 || msn == 0xB0) {
        enqueueRawByte(b);
        runningSupported = true;
      } else {
        // Do not forward data bytes of unsupported status frames.
        enqueueData = false;
        if (b < 0xF0) runningSupported = false;
      }
      i++;
    } else {
      // Running-status event: only forward if last explicit status was supported.
      enqueueData = runningSupported;
    }

    // Data bytes for this event (until next MSB byte).
    while (i < n) {
      b = data[i];
      if (b & 0x80) break;
      if (enqueueData) enqueueRawByte(b);
      i++;
    }
  }
}

class MidiCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic* c) override {
    enqueueBlePacket(c->getData(), c->getLength());
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer* /*s*/) override {
    bleClientConnected = true;
  }
  void onConnect(BLEServer* s, esp_ble_gatts_cb_param_t* param) override {
    bleClientConnected = true;
    if (s != nullptr && param != nullptr) {
      // Lower connection interval improves BLE MIDI throughput for fast passages.
      s->updateConnParams(param->connect.remote_bda, 6, 12, 0, 200);
    }
  }

  void onDisconnect(BLEServer* s) override {
    bleClientConnected = false;
    note_stop(); // safety: force output low on disconnect
    s->startAdvertising();
  }
};

void blesource_init() {
  BLEDevice::init("ESP32 BLE MIDI");
  BLEDevice::setMTU(247);

  bleServer = BLEDevice::createServer();
  bleServer->setCallbacks(new ServerCallbacks());

  BLEService* svc = bleServer->createService(MIDI_SERVICE_UUID);
  midiChar = svc->createCharacteristic(
    MIDI_CHAR_UUID,
    BLECharacteristic::PROPERTY_READ |
    BLECharacteristic::PROPERTY_WRITE |
    BLECharacteristic::PROPERTY_WRITE_NR |
    BLECharacteristic::PROPERTY_NOTIFY
  );
  midiChar->setCallbacks(new MidiCallbacks());
  svc->start();

  BLEAdvertising* adv = BLEDevice::getAdvertising();
  adv->addServiceUUID(MIDI_SERVICE_UUID);
  adv->setScanResponse(true);
  adv->start();
}

// Same parser logic as serialsource_run(), just fed from BLE byte queue.
static void processMidiByte(uint8_t b, uint8_t* lastStatus, uint8_t* buf, uint8_t* remaining, uint8_t* bufIdx) {
  if (b & 0x80) {
    uint8_t msn = b & 0xF0;
    if (msn == 0x80 || msn == 0x90 || msn == 0xB0) {
      *lastStatus = b;
      *remaining = 2;
      *bufIdx = 0;
    } else {
      *lastStatus = 0;
    }
    return;
  }

  if (*lastStatus == 0) return;

  buf[*bufIdx] = b;
  (*bufIdx)++;
  (*remaining)--;

  if (*remaining == 0) {
    midiMsg m;
    m.cmd = *lastStatus;
    m.db1 = buf[0];
    m.db2 = buf[1];

    // Merge all channels in BLE mode, prioritizing ch 1/2 and softening others.
    if ((m.cmd & 0xF0) == 0x90 && m.db2 > 0) {
      uint8_t ch = m.cmd & 0x0F;
      if (ch != PRIORITY_CH_A && ch != PRIORITY_CH_B) {
        uint16_t v = (uint16_t)m.db2 * NON_PRIORITY_GAIN_NUM / NON_PRIORITY_GAIN_DEN;
        m.db2 = (v == 0) ? 1 : (uint8_t)v;
      }
    }

    *remaining = 2;
    *bufIdx = 0;
    parsemsg(&m);
  }
}

void blesource_run() {
  bool prevAllowAll = allow_all_midi_channels;
  uint8_t prevPoly = get_polyphony_limit();
  bool prevRelaxedPoly = midi_relaxed_poly;
  set_polyphony_limit(4);
  allow_all_midi_channels = true; // merge channels for BLE playback
  midi_relaxed_poly = true;       // better note selection under dense MIDI traffic
  bleStop = false;
  bleClientConnected = false;

  portENTER_CRITICAL(&bleRxMux);
  rxHead = rxTail = 0;
  portEXIT_CRITICAL(&bleRxMux);

  lcd_printhome("BLE MIDI");
  lcd_setcursor(0, 1);
  for (int i = 0; i < volindex; i++) lcd_print((char)1);

  uint8_t lastStatus = 0;
  uint8_t msgBuf[2] = {0, 0};
  uint8_t remaining = 0;
  uint8_t bufIdx = 0;

  unsigned long lt = millis();
  bool prevConnected = false;

  for (;;) {
    unsigned long t = millis();
    bool connected = (bleServer != nullptr) && (bleServer->getConnectedCount() > 0);
    bleClientConnected = connected;

    if (!connected && prevConnected) {
      note_stop();
    }
    prevConnected = connected;

    if (t - lt > 150) {
      lt = t;
      unsigned char k = get_key();
      if (k == btnBACK) {
        note_stop();
        break;
      } else if (k == btnUP) {
        incvol(&lcd);
      } else if (k == btnDOWN) {
        decvol(&lcd);
      }
      lcd_printat(0, 0, connected ? "BLE MIDI ON " : "Waiting BLE");
    }

    uint8_t b = 0;
    while (dequeueRawByte(&b)) {
      processMidiByte(b, &lastStatus, msgBuf, &remaining, &bufIdx);
    }
  }

  BLEDevice::getAdvertising()->stop();
  BLEDevice::deinit(true);
  allow_all_midi_channels = prevAllowAll;
  set_polyphony_limit(prevPoly);
  midi_relaxed_poly = prevRelaxedPoly;
}
