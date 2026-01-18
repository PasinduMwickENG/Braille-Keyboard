# Braille Keyboard – Custom HID Device for Typing & Media Control

 <p float="left">
  <img src="https://github.com/user-attachments/assets/2fb4d2a7-52c6-466e-a6e5-7643cf6a1dc2" width="1800" style="margin-right:10px"/>
  
</p>
  


<p float="left">
  <img src="https://github.com/user-attachments/assets/b31995be-0356-46af-a5cb-950d45e6b0c8" width="300" style="margin-right:10px"/>
  <img src="https://github.com/user-attachments/assets/fb2e57c3-e174-4cca-93e6-f217b1025be4" width="300"/>
</p>






A **custom Braille keyboard** built on ESP32 with USB HID support. This project allows users to type using Braille input, control media, and program custom macros directly via USB. It’s designed for accessibility, efficiency, and modular hardware customization.  

---

## 🛠 Features

- **Braille Input**  
  - Type letters, numbers, and punctuation using a 6-dot Braille system.  
  - Supports capitalization and numeric input modifiers.  

- **Secondary Numpad**  
  - Direct number input using a 6-button pad.  

- **Custom Keys**  
  - 3 programmable keys for macros.  
  - 3 media/control keys (volume, play/pause, next/previous).  
  - Multiple custom modes (Media, Modifiers, Programming).  

- **Special Keys**  
  - Dedicated Backspace, Enter, Space, Delete, and Caps Lock keys.  

- **Programming Mode**  
  - Easy macro programming stored in EEPROM.  

- **Robust USB HID Implementation**  
  - Works with both keyboards and consumer control (media) devices.  

---

## 🔧 Hardware Overview

### Pin Configuration

| Function | Pins |
|----------|------|
| Braille Input | 6, 5, 4, 7, 15, 16 |
| Numpad Input | 8, 18, 17, 9, 10, 11 |
| Custom Keys | 12, 13, 14, 20, 45, 3 |
| Special Keys | SPACE: 38, ENTER: 42, BACKSPACE: 37, DELETE: 39, CAPSLOCK: 21 |
| Mode & Programming | MODE: 41, PROGRAM: 2, CUSTOM_MAIN: 46 |

---

### Hardware Components

- ESP32 microcontroller (any variant supporting USB HID)  
- Tactile push buttons for Braille input  
- Numpad buttons for secondary input  
- Custom/macro keys  
- Enclosure with key alignment and ergonomic layout  

---

## 🖥 PCB & Enclosure

### PCB Design

- Designed with KiCAD  
- 2-layer PCB supporting all input buttons and USB HID interface  
- Optimized for compact Braille keyboard layout

 <p float="left">
  <img src="https://github.com/user-attachments/assets/cf254a68-d0d6-460c-af31-a80cd016acc5" width="500" style="margin-right:10px"/>
  <img src="https://github.com/user-attachments/assets/15fa9277-f736-48cb-93b4-275cadc31fb1" width="500"/>
</p> 





### Enclosure

- 3D-printed  enclosure  
- Ergonomic key placement for Braille typing  
- Openings for USB and buttons  

<img width="1200"  alt="Screenshot 2026-01-18 144346" src="https://github.com/user-attachments/assets/9ba52503-d87b-4baa-8a89-06ab53886f2f " />


---

## 💻 Software

### Libraries Used

- `USB.h` – USB core for ESP32 HID support  
- `USBHIDKeyboard.h` – Keyboard HID functions  
- `USBHIDConsumerControl.h` – Media/Consumer control HID  
- `EEPROM.h` – Store programmed macros  
- `esp_system.h` – System-level operations  

### Features

- Robust Braille mapping with letters and punctuation  
- Customizable macros via programming mode  
- Support for both media keys and modifier shortcuts  
- EEPROM storage to retain macros across power cycles  

### Usage

1. Connect the ESP32 to your PC via USB.  
2. Input letters using the Braille keys.  
3. Switch modes using the **Mode** button.  
4. Program macros using the **Program** button in Custom Mode 2.  

---

## 📝 Code Structure

- `setup()` – Initializes USB HID, EEPROM, and pins  
- `loop()` – Main logic for Braille input, numpad, custom keys, and special keys  
- `getBraille()` – Maps Braille patterns to letters/punctuation  
- `sendShortcut()` – Sends Ctrl-based keyboard shortcuts  
- `sendMedia()` – Sends media control codes  

---


