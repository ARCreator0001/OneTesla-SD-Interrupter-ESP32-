# ESP32 Musical Tesla Coil interrupter - a truly versatile interrupter

<img width="1257" height="724" alt="image" src="https://github.com/user-attachments/assets/06b8d16a-2eb3-4a12-a401-15702a54bbad" />
<img width="1048" height="626" alt="image" src="https://github.com/user-attachments/assets/0a7b73ee-704c-430d-9e90-3e3829a2eea8" />



<img width="1102" height="761" alt="Screenshot 2026-03-31 202751" src="https://github.com/user-attachments/assets/1bad1ee0-b1f1-484d-b681-b96e319bf2ea" />
Link to onshape: [Onshape](https://cad.onshape.com/documents/e0858d62bc807065493100b9/w/9e573f9273f6c5cefa0e0909/e/5fb6c5c268f4d3541bf88c26?renderMode=0&uiState=69cbe64c76ff3f85d768a45f)

## Why? 

I always wanted a good, cheap and reliable Tesla Coil Interrupter but as of now, there is not a good, easily accessible proper product like project or prototype I could get. And I needed a good, feature rich, safe interrupter too for my Tesla Coil journey since all my prototype perfboard designs failed of the harsh environments. I wanted something close to the OneTesla SD interrupter but a modern touch, more modern features that make sense and a lot of new features that is essential nowadays. 
 
That is why I set out to build a good, ESP32S3 based Tesla Coil interrupter that is reliable as a product and robust for the environments of a Tesla Coil.

## About the project in a nutshell

It is a portable fiber optic(POF) output interrupter designed to interrupt DRSSTCs and SSTCs safely with limits, and even audio modulate the interruption for musical tesla coil arcs! 
It has an easy to use UI, simple settings on the frontend, and importantly, it has so much limits and safeties specifically tuned for DRSSTC to ensure the coil and driver is not stressed very much. 

It is powered by a 9V battery inside the battery compartment by default, but USB C power input(compatible with modern chargers) can also be used by changing a mode select jumper. 

## Hardware it uses and more features

- Compact 16x2 LCD for displaying information. 
- Micro SDcard slot for music. It supports playing  .mid file and even OneTesla's propietary binary music data file (.omd) used kn their interrupters as a fallback. 
- 4 Voices support for midi files(2 voice for  .omd) means more complex music can be played! 
- Optical fiber output (POF 1mm) for output to coil driver, ensuring isolation from EM radiation and noise. 
- Live mode MIDI over serial through DIN 5 jack and also USB MIDI support is present (usb midi needs firmware addition yet) 
- Tactile Buttons and Rotary encoder(setting volume and also for selection) for the UI and adjustments in operation. There is also a latching power switch. 
- Fixed-frequency mode with adjustable freq and volume

## How to use. 

- Press the latching switch. The device will turn on and display by default Fixed Mode. 
- Left&Right buttons can be used to scroll through menus. Select is done by pressing on the rotary encodee knob till thr encoder knob clicks. 
- The middle button(used to be select)will become the new exit button to exit out of modes(yet to be totally verified in firmware)
- To play fixed freq pulses select fixed mode. Rotating rotary knob when the Freq is selected will change the frequency, to select volume, left or right button is used till bottom row of LCD is highlighted,then volume(duty cycle) can be adjusted by rotary knob. Middle button will be used to exit the mode.Note optical output starts once in fixed mode(default 20Hz with minimum volume) and will stop once exited. 

- For live modes(Live&MIDI BLE), an input such as from a keyboard or any device can be given to the interrupter in live. The device will act as Midi receiver. 


# Firmware details

## Build & Upload (Arduino IDE)

 The interrupter after building needs to be programmed with firmware. The latest firmware files can be accessed in releases. 

1. Board: **ESP32S3 Devkit** (or ESP32S3 Dev Module)

2. Libraries: Please download the necessary libraries, it will after compiling give errors indicating which is missing. Most libraries are common and preinstalled in Arduino. Some libraries (not all) that need to be installed is LiquidCrystal_I2C, NimBLE. For now the firmware is still in work and libraries constantly change, so I apologize for less clarity, soon it will be clear on dependencies. 

3. Select correct COM port, Upload.

## Controls

- **Main menu**: LEFT/RIGHT to navigate; SELECT(rotary's switch) to enter, soon mid button for exiting. 
- **Fixed mode**: short SELECT toggles freq/volume adjust; hold SELECT ~1 s to exit(as of current firmware) 

- **Live mode&MIDI BLE**: SELECT activates and exits live modes. Status is shown in LCD. 

- **SD browser**:LEFT/RIGHT to navigate; SELECT opens folder or plays file; hold SELECT ~1 s to exit to menus(as of now) 

- **Playback**: SELECT stops; Rotary knob rotation djust volume bar

## File format

- It accepts .mid MIDI files. More than 4 channels won't play correctly, always keep melody in first two as they are preferred internally. 
-It also uses .omd file but it is a limited fallback only, it is a cut down midi file format OneTesla used so your music may not play properly. Soon  .omd will be disabled by default. 

# CAD images and images of PCB & schematic
<img width="1680" height="899" alt="image" src="https://github.com/user-attachments/assets/5525e93a-25ad-4251-b350-33e3f8a4bf7a" />
<img width="918" height="679" alt="image" src="https://github.com/user-attachments/assets/a4fbd079-8a44-4cc9-9eff-eb03a8ec3017" />
<img width="1086" height="649" alt="image" src="https://github.com/user-attachments/assets/86f8c524-60f1-4022-98ec-872d1a598f7b" />
<img width="1102" height="761" alt="image" src="https://github.com/user-attachments/assets/2c8b6ca5-a0b9-4732-b86c-bf5e4fee5b58" />
<img width="992" height="762" alt="image" src="https://github.com/user-attachments/assets/0e483338-82aa-4360-8ca8-ee8dfd671f4a" />
<img width="1103" height="634" alt="image" src="https://github.com/user-attachments/assets/da671fe7-a5e1-48c3-b2bf-8681943b2bd4" />
<img width="1054" height="700" alt="image" src="https://github.com/user-attachments/assets/72788fde-34f4-43f1-9fd9-15f5e951fe4e" />
<img width="1032" height="812" alt="image" src="https://github.com/user-attachments/assets/413a8464-317f-4809-94f0-a928c2feb570" />
<img width="1047" height="835" alt="image" src="https://github.com/user-attachments/assets/8d12908c-c802-4836-a0dd-734005201af4" />
<img width="831" height="737" alt="image" src="https://github.com/user-attachments/assets/72fe30d3-3e3c-4d01-b53e-f4ffe767c5de" />
<img width="1310" height="826" alt="image" src="https://github.com/user-attachments/assets/e55e008d-4078-47d0-bec6-ccba448f418a" />

Exploded view
<img width="1170" height="735" alt="image" src="https://github.com/user-attachments/assets/a160d0d6-d68f-4ae5-8933-08075a5ff77e" />
<img width="1107" height="647" alt="image" src="https://github.com/user-attachments/assets/bb9f048d-a60a-4a5c-b9d1-7baddd771605" />
<img width="1194" height="737" alt="image" src="https://github.com/user-attachments/assets/fc06c8cb-4a9f-4e1f-a545-51790129f071" />

Here is schematic
<img width="2358" height="1672" alt="SCH_ESP32 SD Interrupter_1-Sheet_1_2026-03-31" src="https://github.com/user-attachments/assets/584c0d0e-c3f7-41b7-809b-03401be68280" />
Spent a lot of time and many sleepless nights for these nice designs :)

# Bill of materials

| Part name | Qty required(not ordered due to MOQs) | Source | Price | Actual Price with taxes & shipping | Links |
| --- | --- | --- | --- | --- | --- |
| PCB board fabrication | 1 used 5 MOQ |Lion Circuits or ~~Robu.in~~ | ~~robu(₹3556/$37.7)~~, Lion ₹2983 $31.74 | ~~robu(same)~~,Lion ₹2983 $31.74 |Cannot link to quote|
| 2.54MM male Pin headers | 5 pins(2+3) | Local shop | Already own | Already own | |
|Power Switch Latching|1|LCSC|$0.84|$0.84|[link](https://www.lcsc.com/product-detail/C22386787.html?spm=wm.gwc.xh.7.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|USB C Port Connector|1|LCSC|$1.06|$1.06|[link](https://www.lcsc.com/product-detail/C20624793.html?spm=wm.gwc.xh.8.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|Tactile Button|3|LCSC|$0.06|$0.06|[link](https://www.lcsc.com/product-detail/C7528723.html?spm=wm.gwc.xh.6.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|OptoTransmitter HFBR-1521Z|1|LCSC|$7.65|$7.65|[link](https://www.lcsc.com/product-detail/C188712.html?spm=wm.gwc.xh.9.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|AMS1117-3.3 regulator|1|LCSC|$1.13|$1.13|[link](https://www.lcsc.com/product-detail/C6186.html?spm=wm.gwc.xh.10.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|AMS1117-5.0 regulator|1|LCSC|$1.19|$1.19|[link](https://www.lcsc.com/product-detail/C6187.html?spm=wm.gwc.xh.11.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|ESP32S3 Wroom 1 N8|1|LCSC|$5.42|$5.42|[link](https://www.lcsc.com/product-detail/C2913198.html?spm=wm.gwc.xh.12.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|10uF 25V Aluminum Electrolytic Cap|1|LCSC|$0.49|$0.49|[link](https://www.lcsc.com/product-detail/C43846.html?spm=wm.gwc.xh.13.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|100nF 25V 0805 Ceramic cap|8|LCSC|$0.18|$0.18|[link](https://www.lcsc.com/product-detail/C53084462.html?spm=wm.gwc.dh.14.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|470Ω 150V Thin Film Resistor|1|LCSC|$0.70|$0.70|[link](https://www.lcsc.com/product-detail/C479100.html?spm=wm.gwc.xh.4.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|220Ω 0805 Res|1|LCSC|$1.70|$1.70|[link](https://www.lcsc.com/product-detail/C186237.html?spm=wm.gwc.xh.3.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|180Ω 0805 Res|1|LCSC|$0.49|$0.49|[link](https://www.lcsc.com/product-detail/C313842.html?spm=wm.gwc.xh.2.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|33ohm 0805 Res|1|LCSC|$0.41|$0.41|[link](https://www.lcsc.com/product-detail/C557530.html?spm=wm.gwc.xh.5.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|Rotary encoder EC11E18244A5|1|LCSC|$2.05|$2.05|[link](https://www.lcsc.com/product-detail/C255515.html?spm=wm.gwc.xh.1.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|Micro SD TF card port|1|LCSC|$0.73|$0.73|[link](https://www.lcsc.com/product-detail/C111196.html?spm=wm.gwc.xh.0.cbm___wm.sxq.ssl.gwc&lcsc_vid=RAIIXwdQElVfUAVfEgNfVlFWEVQIXwcAR1RbVVFeTlIxVlNRQ1hYXlxWRVBfVTsOAxUeFF5JWBYZEEoKFBINSQcJGk4dAgUUFAk%3D)|
|10K Resistor array 1206|1|robu.in|₹ 2.28 $0.0243|₹ 2.28 $0.0243|[link](https://robu.in/product/cay16-1002f4lf-bourns-4-%c2%b11-10k%cf%89-62-5mw-%c2%b1200ppm-%e2%84%83-1206-resistor-networks-arrays-rohs/)|
|0805 LED Green|1|robu.in|₹ 2.62 $0.0279|₹ 2.62 $0.0279|[link](https://robu.in/product/0805-surface-mount-led-green-50-pcs/)|
|0805 LED Red|2|robu.in|₹2.36 $0.0252|₹2.36 $0.0252|[link](https://robu.in/product/0805-surface-mount-led-red-50-pcs/)|
|DSS34 diode|2|robu.in|₹ 9.58 $0.1021|₹ 9.58 $0.1021|[link](https://robu.in/product/dss34-slkor-sod-123fl-schottky-diodes-rohs/)|
|1N4148W 1206 Diode|2|robu.in|₹ 1.70 $0.0181| ₹ 1.70 $0.181|[link](https://robu.in/product/1n4148w-sod-123-1206-diodereel-of-3000/)|
|SS8050S-FOSAN-SOT-23 Bipolar (BJT)|1|robu.in|₹1.82 $0.0194| ₹1.82 $0.0194|[link](https://robu.in/product/ss8050s-fosan-sot-23-bipolar-bjt-rohs/)|
|510kΩ 0805 res|1|robu.in|₹0.21 $0.0022||[link](https://robu.in/product/0805w8j0514t5e-uniohm-royal-ohm-125mw-thick-film-resistors-150v-%c2%b1100ppm-%e2%84%83-%c2%b15-510k%cf%89-0805-chip-resistor-surface-mount-rohs/)|
|5.1kΩ 0805 res|2|robu.in|₹ 1.96 $0.0209|₹ 1.96 $0.0209|[link](https://robu.in/product/ac0805fr-075k1l-yageo-res-thick-film-0805-5-1k-ohm-1-0-125w1-8w-%c2%b1100ppm-c-pad-smd-t-r-automotive-aec-q200/)|
|2.2Kohm 0805 res|1|robu.in|₹ 0.42 $0.0045|₹ 0.42 $0.0045|[link](https://robu.in/product/ac0805jr-072k2l-yageo-res-thick-film-0805-2-2k-ohm-5-0-125w1-8w-%c2%b1100ppm-c-pad-smd-t-r-automotive-aec-q200/)|
|47Ω 0805 res|1|robu.in|₹ 0.20 $0.0021|₹ 0.20 $0.0021|[link](https://robu.in/product/0805w8f470jt5e-uniohm-royal-ohm-125mw-thick-film-resistors-150v-%c2%b11-%c2%b1200ppm-%e2%84%83-47%cf%89-0805-chip-resistor-surface-mount-rohs/)|
|TS-1088-AR02016 Switch|2|robu.in|₹8.43 $0.0899|₹8.43 $0.0899|[link](https://robu.in/product/1-month-warranty-470/)|
|TLJR106M010S2000-KYOCERA AVX-10uF 10V cap|1|robu.in|₹26 $0.2772|₹26 $0.2772|[link](https://robu.in/product/tljr106m010s2000-kyocera-avx-10uf-10v-2%cf%89-%c2%b120-0805-tantalum-capacitors-rohs/)|
|16V 10uF X5R 0805|2|robu.in|₹1.96 $0.0209|₹1.96 $0.0209|[link](https://robu.in/product/cl21a106kofnnne-samsang-16v-10uf-x5r-%c2%b110-0805-multilayer-ceramic-capacitors-mlcc-smd-smt-rohs/)|
|1.3Kohm 0805 res|1|robu.in|₹ 1.96 $0.021|₹ 1.96 $0.021|[link](https://robu.in/product/rt0805fre071k3l-yageo-125mw-thin-film-resistor-%c2%b150ppm-%e2%84%83-%c2%b11-1-3k%cf%89-0805-chip-resistor-surface-mount-rohs/)|
|M2 X 12mm Female to Female Brass with Nickel plating Hex Threaded Standoff Spacer|4|onlyscrews.in|₹24.8 $0.2644|₹24.8 $0.2644|[link](https://onlyscrews.in/products/m2-x-12mm-female-to-female-brass-with-nickel-plating-hex-threaded-standoff-spacer?variant=50830639006009)|
|M2 X 10mm Hex (Allen) Socket Head High Tensile(12.9) Black oxide screw (Dia. 2mm, Length 10mm)|8|onlyscrews.in|₹28.8 $0.307|₹28.8 $0.307|[link](https://onlyscrews.in/products/m2-x-10mm-hex-allen-socket-head-high-tensile12-9-black-oxide-screw-dia-2mm-length-10mm?variant=49979287339321)|
|M2 Plain Washer SS304 (ID. 2.1mm OD:4.5mm T:0.5mm)|4|onlyscrews.in|₹2.4 $0.0256|₹2.4 $0.0256|[link](https://onlyscrews.in/products/m2-x-6mm-hex-allen-socket-head-high-tensile12-9-black-oxide-screw-dia-2mm-length-6mm?variant=49979039416633)|
|M2 Hex Nut Mild Steel with White Zinc Plating (Dia. 2mm)|4|onlyscrews.in|₹4 $0.0426|₹0.0426|[link](https://onlyscrews.in/products/m2-hex-nut-mild-steel-dia-2mm?variant=50252401246521)|
|M2 X 4mm Brass Threaded Inserts (Dia. 2mm, Length 4mm)|8|onlyscrews.in|₹21.6 $0.23|₹21.6 $0.23|[link](https://onlyscrews.in/products/m2-x-4mm-brass-threaded-inserts?variant=49423172829497)|
|16*2 I2C LCD|1|Local shop|Alr own|Alr own| |
|9V PP3 Battery|1|Local shop|Alr own|Alr own| |
|9V PP3 Battery connector clip|1|Local shop|Will purchase myself|Will purchase myself| |
|Sum||||$58.8217 excluding shipping cost of LCSC of $29.25 and $0.5224 of Robu,LionCircuits is free shipping,OnlyScrews has shipping cost of $0.6397| |
|Grand Total|||||$89.2338| |
 # Pictures of carts with the products needed to order:
 <img width="1916" height="978" alt="image" src="https://github.com/user-attachments/assets/f36e0148-2897-4e6b-bdff-f7a7475dce63" />
<img width="1919" height="977" alt="image" src="https://github.com/user-attachments/assets/be509a54-4a1c-4d58-9a77-3b5e6cfbb854" />
<img width="1918" height="954" alt="image" src="https://github.com/user-attachments/assets/0a687b3c-43eb-4415-8762-64847ac0c2db" />



<img width="1919" height="998" alt="image" src="https://github.com/user-attachments/assets/415cf5db-4ea6-4080-85b6-b63f5b300836" />
<img width="1916" height="963" alt="image" src="https://github.com/user-attachments/assets/106d0382-2154-4d47-a9b9-3207e09d75ca" />
<img width="1916" height="1023" alt="image" src="https://github.com/user-attachments/assets/028c742f-b012-457e-95c5-625751852e00" />

<img width="1912" height="1023" alt="image" src="https://github.com/user-attachments/assets/a1cfc6eb-8f49-4cd8-925e-2148298976c0" />
<img width="1919" height="1038" alt="image" src="https://github.com/user-attachments/assets/671cb776-4622-4659-a3da-ed219c9392b1" />



Link to onshape: [Onshape](https://cad.onshape.com/documents/e0858d62bc807065493100b9/w/9e573f9273f6c5cefa0e0909/e/5fb6c5c268f4d3541bf88c26?renderMode=0&uiState=69cbe64c76ff3f85d768a45f)



