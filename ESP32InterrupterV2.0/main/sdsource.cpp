#include <SPI.h>
#include <SD.h>
#include <vector>
#include <stdlib.h>
#include <string.h>
#include "constants.h"
#include "sdsource.h"
#include "midisource.h"
#include "data.h"
#include "datatypes.h"
#include "player.h"
#include "util.h"
#include "lcd.h"

struct Entry {
  String fsPath;
  String display;
  bool isDir;
};

static std::vector<Entry> entries;
static int entryIndex = 0;
static String currentPath = "/";
static File currentFile;

static void populateEntries();
static void focusEntry();
static String parentPath(const String& path);
void sdsource_showfilename(int pos = 0);
unsigned int read_msg(unsigned char *track, unsigned int index, timedMidiMsg* msg);
void sdsource_loadfile();

void sdsource_init() {
  sd = (sdsource*) malloc(sizeof(sdsource));
  sd->buf = (unsigned char*) calloc(FILE_BUFFER_SIZE, sizeof(char));
  sd->next = (timedMidiMsg*) malloc(sizeof(timedMidiMsg));
  sd->p_msg = (midiMsg*) malloc(sizeof(midiMsg));
  sd->last_error[0] = '\0';
  sd->valid = 0;
  sd->file_count = 0;
  sd->dir_count = 0;
  sdsource_initcard();
}

void sdsource_initcard() {
  lcd_printhome("Checking SD Card");
  SPI.begin(SD_CLK_PIN, SD_MISO_PIN, SD_MOSI_PIN, SD_CS_PIN);
  if (!SD.begin(SD_CS_PIN)) {
    strcpy(sd->last_error, "SD init failed");
    sd->valid = 0;
    return;
  }
  currentPath = "/";
  populateEntries();
  if (sd->valid) focusEntry();
}

static void populateEntries() {
  entries.clear();
  sd->file_count = 0;
  sd->dir_count = 0;

  File dir = SD.open(currentPath);
  if (!dir || !dir.isDirectory()) {
    strcpy(sd->last_error, "Bad path");
    sd->valid = 0;
    return;
  }

  File f;
  while (true) {
    f = dir.openNextFile();
    if (!f) break;
    String name = String(f.path());
    bool isDir = f.isDirectory();
    String display = name.substring(name.lastIndexOf('/') + 1);
    if (!isDir) {
      int dot = display.lastIndexOf('.');
      if (dot < 0) { f.close(); continue; }
      String ext = display.substring(dot);
      if (!ext.equalsIgnoreCase(".omd") && !ext.equalsIgnoreCase(".mid")) { f.close(); continue; }
      display = display.substring(0, dot);
      entries.push_back({name, display, false});
      sd->file_count++;
    } else {
      entries.push_back({name, display, true});
      sd->dir_count++;
    }
    f.close();
  }
  dir.close();

  if (entries.empty()) {
    strcpy(sd->last_error, "No valid files");
    sd->valid = 0;
  } else {
    sd->valid = 1;
    entryIndex = 0;
  }
}


static void focusEntry() {
  if (currentFile) currentFile.close();
  if (entries.empty()) return;
  if (entries[entryIndex].isDir) { sd->len = 0; return; }
  currentFile = SD.open(entries[entryIndex].fsPath, FILE_READ);
  if (!currentFile) {
    strcpy(sd->last_error, "Open failed");
    sd->valid = 0;
    return;
  }
  sdsource_loadfile();
}

static String parentPath(const String& path) {
  if (path == "/") return "/";
  int slash = path.lastIndexOf('/');
  if (slash <= 0) return "/";
  return path.substring(0, slash);
}

void sdsource_showfilename(int pos) {
  if (entries.empty()) { lcd_printhome("No files"); return; }
  String name = entries[entryIndex].display;
  int fn_len = name.length() - 16;
  int start = 0;
  if (fn_len >= 0) {
    start = (pos % (fn_len + 6)) - 3;
    if (start < 0) start = 0;
    if (start > fn_len) start = fn_len;
  }

  lcd_printhome(name.substring(start).c_str());

  const char* path = entries[entryIndex].fsPath.c_str();
  const char* ext  = strrchr(path, '.');
  bool isMid = ext && strcasecmp(ext, ".mid") == 0;

  lcd_setcursor(0, 1);
  if (entries[entryIndex].isDir) {
    lcd_print("Folder");
  } else if (isMid || sd->len == 0) {
    lcd_print("MIDI");
  } else {
    unsigned int lsec = (int)(sd->len / 1000);
    lcd_print(lsec / 60);
    lcd_print(':');
    unsigned int tmp = lsec % 60;
    if (tmp < 10) lcd_print('0');
    lcd_print(tmp);
  }
}

void sdsource_loadfile() {
  if (!currentFile) return;
  currentFile.seek(0);
  uint8_t header[4];
  if (currentFile.read(header, 4) != 4) { sd->read_count = 0; sd->len = 0; return; }
  sd->len = ((uint32_t)header[0] << 24) | ((uint32_t)header[1] << 16) | ((uint32_t)header[2] << 8) | header[3];
  sd->time = 0;
  sd->buf_index = 0;
  sd->read_count = currentFile.read(sd->buf, FILE_BUFFER_SIZE);
  sd->buf_index = read_msg(sd->buf, 0, sd->next);
}

unsigned int read_msg(unsigned char *track, unsigned int index, timedMidiMsg* msg) {
  if (index + 3 >= sd->read_count) {
    sd->read_count = currentFile.read(sd->buf, FILE_BUFFER_SIZE);
    if (sd->read_count == 0) return 0;
    index = 0;
  }
  msg->time = ((((unsigned int) track[index]) << 7) + (track[index + 1] & 0x7f));
  msg->cmd = ((track[index + 1] & 0x80) >> 7) ? 0x91 : 0x81;
  msg->db1 = track[index + 2];
  msg->db2 = track[index + 3];
  return index + 4;
}

void sdsource_run() {
  if (!sd->valid) { lcd_printhome(sd->last_error); delay(500); return; }
  unsigned long kt = millis();
  int filepos = 0;
  sdsource_showfilename(0);
  for (;;) {
    unsigned long t = millis();
    unsigned char key = get_key();
    
    if (t - kt > 600) { sdsource_showfilename(filepos++); kt = t; }
    if (key == btnBACK) {
      delay(180);
      if (currentPath != "/") {
        currentPath = parentPath(currentPath);
        populateEntries();
        if (sd->valid) focusEntry();
        filepos = 0;
        kt = millis();
        sdsource_showfilename(filepos);
      } else {
        note_stop();
        return;
      }
    }
    if (key == btnSELECT) {
      delay(180);
      if (entries[entryIndex].isDir) {
        currentPath = entries[entryIndex].fsPath;
        populateEntries();
        if (sd->valid) focusEntry();
      } else {
        sdsource_playfile();
        if (midi_skip_dir != 0) {
          midi_skip_dir = 0;
          note_stop();
          return;
        }
      }
      filepos = 0;
      kt = millis();
      sdsource_showfilename(filepos);
    }
    if (key == btnDOWN && !entries.empty()) {
      delay(150);
      entryIndex = (entryIndex + 1) % entries.size();
      focusEntry();
      filepos = 0; kt = millis(); sdsource_showfilename(filepos);
    }
    if (key == btnUP && !entries.empty()) {
      delay(150);
      if (entryIndex == 0 && currentPath != "/") {
        currentPath = parentPath(currentPath);
        populateEntries();
      } else {
        entryIndex = (entryIndex + entries.size() - 1) % entries.size();
      }
      focusEntry();
      filepos = 0; kt = millis(); sdsource_showfilename(filepos);
    }
  }
}


void sdsource_playfile() {
  if (entries.empty() || entries[entryIndex].isDir) return;
  midi_skip_dir = 0;

  if (currentFile) currentFile.close();
  const char* path = entries[entryIndex].fsPath.c_str();
  const char* title = entries[entryIndex].display.c_str();
  const char* ext  = strrchr(path, '.');
  bool isMid = ext && strcasecmp(ext, ".mid") == 0;

  currentFile = SD.open(path, FILE_READ);
  if (!currentFile) { strcpy(sd->last_error, "Open failed"); return; }

  if (isMid) {
    currentFile.close();  // ensure Arduino SD handle is closed
    midisource_play(path, title);
    return;
  }

  // OMD files are already 2-voice preprocessed; keep legacy 2-voice behavior.
  set_polyphony_limit(2);
  allow_all_midi_channels = false;
  midi_relaxed_poly = false;

  // OMD path (unchanged)
  sdsource_loadfile();
  lcd_printhome("\2 ");
  for (int i = 0; i < volindex; i++) lcd_printat(i, 1, (char) (1));
  unsigned long lt = millis(), kt = 0, ct = 0, st = millis();
  // one action on key transition (edge) to prevent repeat while held
  unsigned char prevKey = btnNONE;

  for (;;) {
    unsigned long t = millis();
    unsigned char key = get_key();
    if (t - kt > 50) { // check edges every 50ms
      kt = t;
      if (key != prevKey) {
        prevKey = key;
        if (key == btnSELECT) {
          note_stop(); currentFile.close(); sd->buf_index = sd->read_count = 0;
          return;
        } else if (key == btnBACK) {
          midi_skip_dir = 1;
          note_stop(); currentFile.close(); sd->buf_index = sd->read_count = 0;
          return;
        } else if (key == btnUP) {
          lcd_setcursor(0, 1);
          incvol(&lcd);
          lcd_printat(0, 1, '[');
          lcd_printat(15, 1, ']');
        } else if (key == btnDOWN) {
          lcd_setcursor(0, 1);
          decvol(&lcd);
          lcd_printat(0, 1, '[');
          lcd_printat(15, 1, ']');
        }
      }
    }

    if (t - ct > 1000) {
      unsigned int tsec = (int)((t - st) / 1000);
      unsigned int lsec = (int)(sd->len / 1000);
      lcd_printat(2, 0, 13, tsec / 60);
      lcd_print(':');
      unsigned int tmp = tsec % 60; if (tmp < 10) lcd_print('0'); lcd_print(tmp);
      lcd_print('/');
      lcd_print(lsec / 60);
      lcd_print(':');
      tmp = lsec % 60; if (tmp < 10) lcd_print('0'); lcd_print(tmp);
      ct = t;
    }
    if (t - lt > sd->next->time) {
      sd->time += (t - lt);
      sd->p_msg->cmd = sd->next->cmd;
      sd->p_msg->db1 = sd->next->db1;
      sd->p_msg->db2 = sd->next->db2;
      parsemsg(sd->p_msg);
      sd->buf_index = read_msg(sd->buf, sd->buf_index, sd->next);
      if (sd->read_count == 0) { note_stop(); currentFile.close(); return; }
      lt = t;
    }
  }
}
