# Dot Invaders Hardware Specification

## Overview

Ultra-low-power retro handheld game platform featuring:
- 8x8 LED matrix display
- Two-button input (with optional third button)
- Battery or USB powered
- In-circuit USB programming
- 3D printable enclosure
- **Hackable/modular design for geeks**

---

## Design Philosophy: Hackability First

This is a **geek machine** - designed for tinkering, modding, and learning.

| Principle | Implementation |
|-----------|----------------|
| Socketed MCU | DIP-8 socket for easy chip swap/upgrade |
| Modular display | Header pins for MAX7219 + matrix removal |
| Swappable battery | 2xAAA clip OR LiPo holder (same footprint) |
| Customizable buttons | Replaceable button caps (3D printed) |
| Exposed pads | Breakout pads for unused pins |
| Open source | All design files public (KiCad, STL) |

---

## Platform Goals

| Goal | Target |
|------|--------|
| Power consumption | <20mA active, <1µA sleep |
| Battery life | 20+ hours on 2xAAA or coin cell |
| Cost | <$10 BOM |
| Size | Pocket-sized (~60x40x15mm) |
| Programming | USB in-place, no disassembly |

---

## Core Components

### Microcontroller: ATtiny85

| Spec | Value |
|------|-------|
| Package | 8-pin DIP or SOIC |
| Flash | 8KB |
| SRAM | 512 bytes |
| EEPROM | 512 bytes |
| Clock | 8MHz internal (16MHz PLL available) |
| Operating voltage | 2.7-5.5V |
| Active current | ~1mA @ 1MHz, ~5mA @ 8MHz |
| Sleep current | <0.1µA power-down mode |

**Pin Assignment:**
```
ATtiny85 Pinout:
                 ┌──────┐
    (RESET) PB5 ─┤1    8├─ VCC
            PB3 ─┤2    7├─ PB2 (SCK/USB D+)
            PB4 ─┤3    6├─ PB1 (MISO/USB D-)
            GND ─┤4    5├─ PB0 (MOSI)
                 └──────┘

Current Assignment:
  PB0 - MAX7219 DIN (data)
  PB1 - MAX7219 CLK (clock) / USB D-
  PB2 - MAX7219 CS (chip select) / USB D+
  PB3 - RIGHT button (active low, internal pullup)
  PB4 - LEFT button (active low, internal pullup)
  PB5 - RESET (or third button if fuse disabled)
```

### Display: MAX7219 + 8x8 LED Matrix

| Spec | Value |
|------|-------|
| Driver IC | MAX7219 |
| Display | 8x8 common cathode LED matrix |
| Interface | 3-wire SPI (DIN, CLK, CS) |
| Current per LED | 40mA max (adjustable via ISET resistor) |
| Scan rate | 800Hz (no flicker) |
| Operating voltage | 4.0-5.5V |

**Power Considerations:**
- MAX7219 intensity register (0x0A) controls brightness 0-15
- At intensity 8: ~100mA with all LEDs on
- At intensity 2: ~30mA with all LEDs on
- Typical gameplay: ~20-50mA

### Buttons (2 Pins → 3 Inputs)

Only **2 physical buttons** using **2 GPIO pins**, but providing **3 logical inputs** via chord detection:

| Physical | Pin | Logical Inputs |
|----------|-----|----------------|
| LEFT button | PB4 | Left action (tap/hold) |
| RIGHT button | PB3 | Right action (tap/hold) |
| *Both pressed* | — | Third action (chord) |

**Input Matrix:**
```
             LEFT released    LEFT pressed
            ┌────────────────┬────────────────┐
RIGHT       │                │                │
released    │    (idle)      │  LEFT action   │
            │                │                │
            ├────────────────┼────────────────┤
RIGHT       │                │                │
pressed     │  RIGHT action  │  CHORD action  │
            │                │                │
            └────────────────┴────────────────┘
```

**Advantages:**
- PB5/RESET pin preserved for USB bootloader programming
- No fuse changes required
- Full ISP capability retained
- Simple 2-button hardware, 3-action gameplay

**Button Specs:**
- Type: Tactile momentary switch
- Travel: 0.25mm typical
- Force: 160-260gf
- Debounce: Software (handled in code)
- Chord detection: Movement on release, fire on both-pressed

---

## Modular/Hackable Components

### MCU Socket (DIP-8)

```
┌─────────────────────────┐
│  DIP-8 Socket           │
│  ┌───────────────────┐  │
│  │ ○ ○ ○ ○   ○ ○ ○ ○ │  │  <- Machined pins
│  └───────────────────┘  │
│                         │
│  ATtiny85 drops in      │
└─────────────────────────┘
```

**Benefits:**
- Swap MCU without soldering (upgrade to ATtiny85V, ATtiny84, etc.)
- Easy chip replacement if damaged
- Program chips externally before insertion
- Educational: see the actual chip

**Socket type:** Machined-pin DIP-8 (gold contacts, low insertion force)

### Display Module (Header-Connected)

```
Main PCB                    Display Module
┌──────────┐               ┌──────────────┐
│          │               │   8x8 LED    │
│  ○ ○ ○ ○ ├───────────────┤   Matrix     │
│  VCC     │  5-pin header │              │
│  GND     │               │   MAX7219    │
│  DIN     │               │   (on back)  │
│  CLK     │               │              │
│  CS      │               └──────────────┘
└──────────┘

Header: 2.54mm pitch, 5-pin female on main PCB
        5-pin male on display module
```

**Benefits:**
- Swap display colors (red, green, blue, white matrix)
- Upgrade to different matrix sizes (with code change)
- Replace if LEDs burn out
- Test different LED brightness/styles

### Swappable Battery System

**Design:** Same PCB footprint for both options

```
Option A: 2xAAA Holder          Option B: LiPo + Charger
┌─────────────────────┐         ┌─────────────────────┐
│  [AAA]    [AAA]     │         │  [LiPo 3.7V]        │
│   (+)      (-)      │         │  [TP4056 charger]   │
│    └────┬───┘       │         │       └──┬──┘       │
│         │           │         │          │          │
│    3V nominal       │         │    3.7-4.2V         │
│    (needs boost)    │         │    (needs boost)    │
└─────────────────────┘         └─────────────────────┘
          │                               │
          └───────────┬───────────────────┘
                      ▼
              [Boost to 5V]
              (shared circuit)
```

**Compatibility:**
- Same boost converter footprint
- Same power switch location
- AAA holder and LiPo holder have matching mounting holes
- User swaps by replacing battery holder + optionally adding TP4056

### Customizable Button Caps

```
3D Printed Button Caps (various styles):

  Standard    Concave     Raised      D-Pad
   ┌───┐      ╭───╮      ┌─▲─┐      ┌─────┐
   │   │      │ ╰ │      │   │      │  ▲  │
   └───┘      ╰───╯      └───┘      │◄ ● ►│
                                    │  ▼  │
                                    └─────┘
```

**Cap specs:**
- 6mm diameter base (fits standard tactile switch)
- Friction fit or snap-on
- STL files provided for customization
- Print in different colors

### Expansion Pads

```
PCB Edge Breakout:
┌──────────────────────────────────────┐
│  Main Circuit                        │
│                          ○ VCC       │
│                          ○ GND       │
│                          ○ PB5/RST   │
│                          ○ SPARE1    │
│                          ○ SPARE2    │
└──────────────────────────────────────┘
```

**For hackers:**
- Add external sensors (light, temp, tilt)
- Add piezo speaker (1 pin)
- Add vibration motor
- Chain additional LED matrices
- Connect to external programmer

---

## Power System

### Power Options

#### Option A: USB Only (5V)
```
USB 5V ──┬── VCC (ATtiny85)
         └── VCC (MAX7219)
```
- Simplest design
- Requires USB connection to play
- No battery management needed

#### Option B: 2xAAA Battery
```
2xAAA (3V) ──[Boost Converter]── 5V ── VCC
```
- Boost converter (e.g., MCP1640, TPS61220)
- ~85% efficiency
- 20+ hours playtime

#### Option C: USB + Battery (Recommended)
```
USB 5V ──[Schottky Diode]──┬── VCC
                           │
LiPo 3.7V ─[Boost]─────────┘
          ─[Charger TP4056]── USB 5V
```
- Best of both worlds
- Play while charging
- LiPo or 2xAAA options

### Power Budget

| Component | Active (mA) | Sleep (µA) |
|-----------|-------------|------------|
| ATtiny85 @ 8MHz | 5 | 0.1 |
| MAX7219 (intensity 8) | 50 | 10 |
| Boost converter | 2 | 10 |
| **Total** | **~57** | **~20** |

With 2xAAA (2000mAh): ~35 hours active play

---

## USB Programming Interface

### Micronucleus Bootloader

The ATtiny85 can be programmed via USB using the Micronucleus bootloader:

| Spec | Value |
|------|-------|
| Bootloader size | ~1.5KB |
| Available flash | ~6.5KB |
| USB protocol | V-USB (software USB 1.1) |
| Programming | avrdude with micronucleus protocol |

**USB Connection:**
```
USB D- ── 68Ω ── PB1
USB D+ ── 68Ω ── PB2
         ├── 1.5kΩ pullup to VCC (via 3.6V zener)
USB GND ── GND
USB 5V ── VCC
```

**Note:** USB D+ needs 1.5kΩ pullup to 3.3-3.6V for device detection.

### Pin Conflict Resolution

PB1 and PB2 are shared between MAX7219 and USB:

| Mode | PB1 | PB2 |
|------|-----|-----|
| Game running | MAX7219 CLK | MAX7219 CS |
| USB programming | USB D- | USB D+ |

**Solutions:**
1. **Physical switch** - DPDT switch to select mode
2. **Jumper** - Solder jumper or header pins
3. **Auto-detect** - Bootloader checks USB connection at startup
4. **Dedicated USB port** - Use programming header only

**Recommended:** Option 3 (auto-detect) - Micronucleus checks for USB host at reset. If found, enters bootloader. Otherwise boots game.

---

## Schematic

```
                                    VCC (+5V)
                                      │
                     ┌────────────────┼────────────────┐
                     │                │                │
                    ┌┴┐              ┌┴┐            ┌──┴──┐
                    │ │10k           │ │10k         │     │
                    │ │              │ │            │ 8x8 │
                    └┬┘              └┬┘            │ LED │
    ┌────────┐       │                │             │     │
    │        │1      8                │             └──┬──┘
    │ PB5/RST├───────┼───VCC         │                │
    │        │       │                │             ┌──┴──┐
    │    PB3 ├───────┴──[BTN]──GND   │             │     │
    │        │                        │             │MAX  │
    │    PB4 ├─────────[BTN]──GND    │             │7219 │
    │        │                        │             │     │
    │    GND ├─────────GND           │             └──┬──┘
    │        │                        │                │
    │    PB0 ├────────────────────────┼──────DIN──────┘
    │        │                        │                │
    │    PB1 ├───────[68Ω]───USB_D-  ├──────CLK──────┘
    │        │                        │                │
    │    PB2 ├───────[68Ω]───USB_D+ ─┼──────CS───────┘
    │        │                 │      │
    └────────┘                ┌┴┐     │
     ATtiny85                 │ │1.5k │
                              └┬┘     │
                               │      │
                              [Z]3.6V │
                               │      │
                              GND    GND

USB Connector:
  Pin 1: VCC (+5V)
  Pin 2: D- (via 68Ω to PB1)
  Pin 3: D+ (via 68Ω to PB2, 1.5k pullup via 3.6V zener)
  Pin 4: GND
```

---

## Power Supply Circuit (Boost Converter)

When running from batteries (2xAAA = 3V, or LiPo = 3.7V), a boost converter steps up to 5V:

### Boost Converter Schematic

```
                         TPS61220 / MCP1640
                        ┌──────────────────┐
                        │                  │
  BAT+ (2.5-4.2V) ──────┤VIN          VOUT├──────┬──── +5V VCC
                        │                  │      │
                   ┌────┤EN               │     ┌┴┐
                   │    │                  │     │ │ 10µF
             ON/OFF│    │            FB ───┤     └┬┘
              ↓    │    │                  │      │
           [SWITCH]│    │             GND──┤──────┴──── GND
                   │    │                  │
                   └────┤               SW├────┐
                        └──────────────────┘    │
                                                │
                                           ┌────┴────┐
                                           │   L1    │  4.7µH
                                           │  ════   │  inductor
                                           └────┬────┘
                                                │
                                                └──── VOUT

Component values (TPS61220):
  L1: 4.7µH inductor (shielded, low DCR)
  C_IN: 10µF ceramic
  C_OUT: 10µF ceramic
  R_FB1: 1MΩ (sets 5V output)
  R_FB2: 324kΩ
```

### Power Path with USB + Battery

```
                    USB 5V
                      │
                     ─┴─
                     \ /  D1 (Schottky)
                      V   BAT54
                      │
                      ├─────────────────────────────── VCC (+5V)
                      │                                  │
                      │    ┌────────────┐               │
  BAT+ ───[SWITCH]────┼────┤ TPS61220   ├───────────────┤
                      │    │  BOOST     │               │
                      │    └────────────┘               │
                      │                                  │
                      │         ┌─────────────────────┐ │
                      │         │                     │ │
                      ├─────────┤ ATtiny85   MAX7219  ├─┘
                      │         │                     │
                     GND        └─────────────────────┘

Power priority:
  1. USB connected → USB powers device (diode blocks battery)
  2. USB disconnected → Battery boost powers device
  3. ON/OFF switch controls battery path only
```

### PCB Placement

```
┌─────────────────────────────────────────────┐
│  ┌───────────────────────────────────────┐  │
│  │           8x8 LED MATRIX              │  │
│  └───────────────────────────────────────┘  │
│                                             │
│  [USB]  [ATtiny85]  [MAX7219]  ┌────────┐  │
│                                │ BOOST  │  │
│                                │ L1 ▣   │  │
│    [LEFT]        [RIGHT]       └────────┘  │
│                                             │
│  [PWR SW]        [BAT+ BAT-]               │
└─────────────────────────────────────────────┘

Boost section: Bottom-right corner
  - Keep inductor away from display (EMI)
  - Short traces: VIN → L1 → SW → VOUT
  - Ground plane under boost section
```

---

## Expansion Port (Smart Cartridge System)

For hackers who want to chain displays or add expansion modules:

### Expansion Header

```
Main PCB Edge Connector (active active active active active active active active active 8-pin):

    ┌─────────────────────────────────────────┐
    │  Main PCB                               │
    │                                         │
    │  ○ ○ ○ ○ ○ ○ ○ ○   <- 2.54mm header    │
    │  1 2 3 4 5 6 7 8                        │
    └──┬─┬─┬─┬─┬─┬─┬─┬────────────────────────┘
       │ │ │ │ │ │ │ │
       │ │ │ │ │ │ │ └── 8: GND
       │ │ │ │ │ │ └──── 7: VCC (+5V)
       │ │ │ │ │ └────── 6: DOUT (MAX7219 data out)
       │ │ │ │ └──────── 5: CLK (directly from PB1)
       │ │ │ └────────── 4: CS2 (directly from PB2)
       │ │ └──────────── 3: PB5/RST (optional I/O)
       │ └────────────── 2: PB3 (directly from button input)
       └──────────────── 1: PB4 (directly from button input)
```

### Chaining MAX7219 Displays

The MAX7219 has DOUT for daisy-chaining multiple matrices:

```
Main Unit                    Expansion Cartridge
┌──────────────┐            ┌──────────────────────┐
│   8x8 LED    │            │   8x8 LED   8x8 LED  │
│   ┌─────┐    │            │   ┌─────┐   ┌─────┐  │
│   │     │    │            │   │     │   │     │  │
│   └──┬──┘    │            │   └──┬──┘   └──┬──┘  │
│   ┌──┴──┐    │            │   ┌──┴──┐   ┌──┴──┐  │
│   │MAX  │    │  8-pin     │   │MAX  │───│MAX  │  │
│   │7219 ├────┼──cable────►│───│7219 │   │7219 │  │
│   │#1   │    │            │   │#2   │   │#3   │  │
│   │DOUT─┼────┼───────────►├───│DIN  │   │DIN  │  │
│   └─────┘    │            │   └─────┘   └─────┘  │
└──────────────┘            └──────────────────────┘

Software addressing:
  - Send 3 commands per frame (one per chip)
  - First command goes to chip #3 (farthest)
  - Last command goes to chip #1 (nearest)
  - CS pulses latch all chips simultaneously
```

### Smart Cartridge Concept

Expansion cartridge with its own microcontroller:

```
┌──────────────────────────────────────────────────────┐
│  SMART CARTRIDGE                                     │
│  ┌─────────────────────────────────────────────────┐ │
│  │                                                 │ │
│  │   ┌───────────┐    ┌───────────┐               │ │
│  │   │  Extra    │    │   Extra   │               │ │
│  │   │  8x8 LED  │    │   8x8 LED │               │ │
│  │   └─────┬─────┘    └─────┬─────┘               │ │
│  │         │                │                      │ │
│  │      ┌──┴──┐          ┌──┴──┐     ┌─────────┐  │ │
│  │      │MAX  │──────────│MAX  │     │ATtiny85 │  │ │
│  │      │7219 │          │7219 │     │ "CO-    │  │ │
│  │      │     │          │     │     │PROCESSOR│  │ │
│  │      └─────┘          └─────┘     └────┬────┘  │ │
│  │                                        │       │ │
│  └────────────────────────────────────────┼───────┘ │
│                                           │         │
│     ════════════════════════════════════════        │
│              8-pin edge connector                   │
└─────────────────────────────────────────────────────┘

Communication:
  - Coprocessor ATtiny85 listens on shared bus
  - Main unit sends commands via CLK/DIN
  - Cartridge MCU can add game logic, sound, sensors
```

### Expansion Ideas

| Cartridge Type | Components | Use Case |
|----------------|------------|----------|
| **Display Expander** | 2x MAX7219 + matrices | 24x8 or 16x8 display |
| **Sound Cartridge** | ATtiny85 + piezo/speaker | Music, sound effects |
| **Motion Cartridge** | MPU6050 accelerometer | Tilt controls |
| **Memory Cartridge** | 24LC256 EEPROM | Save games, high scores |
| **Link Cartridge** | nRF24L01 radio | Multiplayer wireless |
| **Debug Cartridge** | FTDI USB-serial | Serial debugging |

### Cartridge Detection

```cpp
// On startup, main unit checks for cartridge:
void detectCartridge() {
  // Send discovery command on expansion bus
  // Cartridge responds with ID byte

  // ID 0x00 = No cartridge / dumb display chain
  // ID 0x01 = Display expander (report chain length)
  // ID 0x02 = Sound cartridge
  // ID 0x03 = Motion cartridge
  // ...
}
```

### Physical Cartridge Design

```
Side View:
                    ┌─────────────────┐
                    │   CARTRIDGE     │
                    │   ┌─────────┐   │
                    │   │ 8x8 LED │   │
                    │   └─────────┘   │
                    │                 │
                    │   [CIRCUITRY]   │
                    │                 │
                    └────────┬────────┘
                             │  Edge connector
    ┌────────────────────────┼────────────────────────┐
    │                   ═════╧═════                   │
    │                  MAIN UNIT                      │
    │              (cartridge slot)                   │
    └─────────────────────────────────────────────────┘

Mechanical:
  - 8-pin 2.54mm pitch edge connector
  - Friction fit or latching
  - Cartridge extends above main unit
  - 3D printable cartridge shells
```

---

## PCB Design

### Board Specifications

| Spec | Value |
|------|-------|
| Dimensions | 50 x 35 mm (fits 8x8 matrix) |
| Layers | 2 (top + bottom) |
| Thickness | 1.6mm |
| Copper | 1oz |
| Finish | HASL or ENIG |
| Solder mask | Matte black |
| Silkscreen | White |

### Component Placement

```
Top View:
┌─────────────────────────────────┐
│  ┌───────────────────────────┐  │
│  │                           │  │
│  │       8x8 LED MATRIX      │  │
│  │                           │  │
│  │                           │  │
│  └───────────────────────────┘  │
│                                 │
│  [USB]     [ATtiny85]    [MAX]  │
│            (socket)      7219   │
│                                 │
│    [LEFT BTN]    [RIGHT BTN]    │
│                                 │
│  [BAT+]                  [BAT-] │
└─────────────────────────────────┘

Bottom View:
- SMD passives (resistors, caps)
- Battery contacts or JST connector
```

### Design Files Needed

- [ ] Schematic (KiCad or EasyEDA)
- [ ] PCB layout
- [ ] BOM (Bill of Materials)
- [ ] Gerber files for fabrication
- [ ] Pick-and-place file (if SMD assembly)

---

## 3D Printable Enclosure

### Enclosure Requirements

| Requirement | Specification |
|-------------|---------------|
| Material | PLA or PETG |
| Wall thickness | 1.5-2mm |
| Print orientation | Split horizontal |
| Button access | Cutouts for tactile switches |
| USB access | Side cutout for programming |
| Battery access | Removable back or door |
| Viewing angle | 0° (straight on) |

### Dimensions (Estimated)

```
External: 60 x 45 x 18mm
Internal: 56 x 41 x 14mm

Side View:
┌─────────────────────┐
│   ╔═══════════════╗ │  <- LED matrix visible
│   ║               ║ │
│   ║   DISPLAY     ║ │
│   ║               ║ │
│   ╚═══════════════╝ │
│                     │
│  [BTN]       [BTN]  │  <- Button caps protruding
│                     │
├─────────────────────┤
│      PCB HERE       │
├─────────────────────┤
│   BATTERY DOOR      │
└─────────────────────┘
       ↑
    USB port cutout
```

### Design Files Needed

- [ ] Top shell (STL)
- [ ] Bottom shell (STL)
- [ ] Button caps (STL)
- [ ] Battery door (STL) - optional
- [ ] Assembly instructions

---

## Bill of Materials (BOM)

| Qty | Component | Value | Package | Est. Cost |
|-----|-----------|-------|---------|-----------|
| 1 | ATtiny85 | - | DIP-8 | $1.50 |
| 1 | MAX7219 | - | DIP-24 | $1.00 |
| 1 | 8x8 LED Matrix | Common cathode | 20x20mm | $1.00 |
| 1 | DIP-8 socket | - | - | $0.10 |
| 1 | DIP-24 socket | - | - | $0.20 |
| 2 | Tactile switch | 6x6mm | THT | $0.10 |
| 1 | Micro USB connector | - | SMD | $0.30 |
| 2 | Resistor | 68Ω | 0603 | $0.02 |
| 1 | Resistor | 1.5kΩ | 0603 | $0.01 |
| 1 | Resistor | 10kΩ | 0603 | $0.01 |
| 1 | Resistor | 27kΩ | 0603 | $0.01 |
| 1 | Zener diode | 3.6V | SOD-123 | $0.05 |
| 2 | Capacitor | 100nF | 0603 | $0.02 |
| 1 | Capacitor | 10µF | 0805 | $0.05 |
| 1 | 2xAAA holder | - | - | $0.50 |
| | | | **Total** | **~$5.00** |

*Optional additions:*
- LiPo battery + TP4056 charger: +$3
- On/off switch: +$0.20
- 3D printed case: +$1 (material)

---

## Software Ecosystem

### For Players: Game Flasher App

A desktop/web app for non-technical users to swap games:

```
┌─────────────────────────────────────────────────────────┐
│  🎮 Dot Platform Game Flasher                      [─]  │
├─────────────────────────────────────────────────────────┤
│                                                         │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐     │
│  │ 👾          │  │ 🧱          │  │ 🐍          │     │
│  │ Dot Invaders│  │ Dot Tetris  │  │ Dot Snake   │     │
│  │             │  │             │  │             │     │
│  │ [Install]   │  │ [Install]   │  │ [Install]   │     │
│  └─────────────┘  └─────────────┘  └─────────────┘     │
│                                                         │
│  ⚙️ Game Settings (before flash):                       │
│  ┌─────────────────────────────────────────────────┐   │
│  │ Starting Lives:    [3 ▼]                        │   │
│  │ Starting Level:    [1 ▼]                        │   │
│  │ Button Swap:       [ ] Left/Right reversed      │   │
│  │ Brightness:        [████████░░] 8/15            │   │
│  │ Speed Preset:      (•) Normal ( ) Fast ( ) Slow │   │
│  └─────────────────────────────────────────────────┘   │
│                                                         │
│  [🔌 Connect Device]  Status: Ready                     │
│                                                         │
└─────────────────────────────────────────────────────────┘
```

**Features:**
- Game library browser (curated .hex files)
- Pre-flash configuration (compile-time constants)
- One-click install via Micronucleus
- Auto-detect device connection
- No Arduino/programming knowledge required

**Implementation:**
- Electron or Tauri desktop app
- Uses `micronucleus` CLI under the hood
- Config options modify game constants before compile
- Or: pre-compiled variants for common configurations

### For Hackers: Arduino Framework

Full Arduino IDE support for custom game development:

**Board Package Installation:**
```
Arduino IDE → Preferences → Additional Board URLs:
https://dotplatform.github.io/package_dotplatform_index.json

Tools → Board → Boards Manager → "Dot Platform"
```

**Board Settings:**
| Setting | Value |
|---------|-------|
| Board | "Dot Platform (ATtiny85)" |
| Clock | 8MHz (internal) or 16.5MHz (USB) |
| Bootloader | Micronucleus |
| Programmer | Micronucleus (USB) |

### Game Development Framework

A lightweight library for Dot Platform games:

```cpp
#include <DotPlatform.h>

DotDisplay display;
DotInput input;

void setup() {
  Dot.begin();           // Initialize hardware
  display.clear();
  display.show();
}

void loop() {
  // Input handling
  if (input.pressed(BTN_LEFT)) {
    // Left button just pressed
  }
  if (input.held(BTN_RIGHT)) {
    // Right button being held
  }
  if (input.chord()) {
    // Both buttons pressed (fire!)
  }

  // Display
  display.clear();
  display.setPixel(x, y, ON);
  display.drawSprite(x, y, spriteData);
  display.show();

  // Timing
  Dot.waitFrame(60);     // 60 FPS frame limiter
}
```

**Framework Features:**

| Module | Functions |
|--------|-----------|
| `DotDisplay` | `clear()`, `setPixel()`, `drawSprite()`, `scrollText()`, `show()` |
| `DotInput` | `pressed()`, `released()`, `held()`, `chord()`, `holdTime()` |
| `DotAudio`* | `beep()`, `playTone()`, `playMelody()` |
| `DotSave` | `save()`, `load()` (EEPROM high scores) |
| `DotPower` | `sleep()`, `batteryLow()` |

*If piezo speaker added

### Configuration System

Games can expose compile-time configuration:

```cpp
// game_config.h - User-modifiable settings
#define STARTING_LIVES    3       // 1-9
#define STARTING_LEVEL    1       // 1-8
#define BUTTON_SWAP       false   // Swap left/right
#define BRIGHTNESS        8       // 0-15
#define INVADER_SPEED     400     // ms per drop (lower = faster)
```

**Flasher app integration:**
1. App reads `game_config.h` comments for UI hints
2. User adjusts sliders/dropdowns
3. App modifies `#define` values
4. Recompile with Arduino CLI
5. Flash via Micronucleus

### Fuse Configuration

For advanced users - fuse settings via the flasher app:

| Fuse | Default | Option | Effect |
|------|---------|--------|--------|
| Clock | 8MHz internal | 16MHz PLL | Faster but more power |
| BOD | 2.7V | Disabled | Lower power, less stable |
| EESAVE | Clear | Preserve | Keep EEPROM on reflash |

**Note on RSTDISBL:** Not recommended. Since chord detection provides a third input, there's no need to sacrifice PB5/RESET. Enabling RSTDISBL would disable USB programming permanently and require a high-voltage programmer to recover. Only for advanced hackers who need a true 4th physical input.

### Game Repository

Community game collection:

```
github.com/dotplatform/games/
├── official/
│   ├── dot-invaders/      # Space Invaders clone
│   ├── dot-tetris/        # Tetris clone
│   ├── dot-snake/         # Snake game
│   └── dot-pong/          # Pong for 2 players
├── community/
│   ├── dot-breakout/      # Breakout clone
│   ├── dot-asteroids/     # Asteroids
│   ├── dot-frogger/       # Frogger
│   └── ...
└── templates/
    ├── basic-game/        # Minimal game template
    ├── scrolling-game/    # Side-scroller template
    └── menu-system/       # Multi-game menu
```

**Submission guidelines:**
- Must compile for ATtiny85 (≤6.5KB with bootloader)
- Include `game_config.h` for user settings
- MIT or similar open license
- README with controls and gameplay

---

## Development Phases

### Phase 1: Breadboard Prototype
- [ ] Wire up ATtiny85 + MAX7219 + matrix on breadboard
- [ ] Test game code
- [ ] Verify USB programming with Micronucleus
- [ ] Measure power consumption

### Phase 2: PCB Design
- [ ] Create schematic in KiCad/EasyEDA
- [ ] Design PCB layout
- [ ] Generate Gerbers
- [ ] Order prototype PCBs (JLCPCB, PCBWay)

### Phase 3: Enclosure Design
- [ ] Design enclosure in Fusion 360/OpenSCAD
- [ ] 3D print prototype
- [ ] Iterate on fit and feel
- [ ] Finalize STL files

### Phase 4: Assembly & Testing
- [ ] Assemble first complete unit
- [ ] Test all functions
- [ ] Battery life test
- [ ] Document assembly process

---

## Open Questions

1. ~~**Third button?**~~ **RESOLVED:** Chord detection (both buttons pressed) provides 3rd input using only 2 pins. No need to sacrifice PB5/RESET.
2. **Battery type?** 2xAAA, CR2032 coin cell, or LiPo?
3. **USB connector?** Micro-USB, USB-C, or bare pads?
4. **Speaker?** Add piezo for audio feedback? (uses 1 pin)
5. **LED matrix style?** Round dots, square pixels, or diffused?

---

## References

- [Micronucleus Bootloader](https://github.com/micronucleus/micronucleus)
- [V-USB Library](https://www.obdev.at/products/vusb/)
- [ATtiny85 Datasheet](https://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2586-AVR-8-bit-Microcontroller-ATtiny25-ATtiny45-ATtiny85_Datasheet.pdf)
- [MAX7219 Datasheet](https://datasheets.maximintegrated.com/en/ds/MAX7219-MAX7221.pdf)
