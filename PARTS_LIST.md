# DotInvaders - Parts List and Shopping Guide

## Required Components

### Essential Parts

| Component | Quantity | Notes | Estimated Price |
|-----------|----------|-------|-----------------|
| ATTiny85 IC | 1 | DIP-8 package recommended for breadboard | $1-2 |
| MAX7219 8x8 LED Matrix Module | 1 | Complete module with driver chip | $2-5 |
| Push Buttons (Tactile) | 3 | 6mm x 6mm recommended | $0.50-1 |
| Breadboard | 1 | 400 or 830 tie points | $2-5 |
| Jumper Wires | 10-15 | Male-to-male for breadboard | $2-5 |
| 5V Power Supply | 1 | USB cable or wall adapter, 500mA+ | $3-8 |

**Total Estimated Cost: $11-26 USD**

### Optional But Recommended

| Component | Quantity | Notes | Estimated Price |
|-----------|----------|-------|-----------------|
| 8-pin DIP Socket | 1 | For ATTiny85 (easier to replace) | $0.20 |
| Capacitors (100µF) | 2 | Power supply smoothing | $0.50 |
| Capacitors (0.1µF) | 3 | Button debouncing | $0.30 |
| 10kΩ Resistors | 3 | External pull-ups (if needed) | $0.30 |
| Enclosure/Case | 1 | Project box for finished product | $3-10 |
| PCB (optional) | 1 | For permanent build | $5-15 |

### For ATTiny85 Programming

You'll need one of these to program the ATTiny85:

**Option 1: Arduino as ISP (Cheapest)**
- Arduino Uno or Nano ($5-25)
- Used as programmer for ATTiny85
- Good if you already have an Arduino

**Option 2: USBasp Programmer**
- USBasp USB programmer ($3-8)
- Direct ATTiny85 programming
- Easier for multiple projects

**Option 3: Arduino ISP Shield**
- Shield with ZIF socket for ATTiny ($5-15)
- Most convenient for repeated programming

## Alternative: Arduino-Based Build

If you want easier development and debugging:

| Component | Quantity | Notes | Estimated Price |
|-----------|----------|-------|-----------------|
| Arduino Nano/Uno | 1 | More pins, easier programming | $5-25 |
| MAX7219 8x8 LED Matrix Module | 1 | Same as above | $2-5 |
| Push Buttons | 3 | Same as above | $0.50-1 |
| USB Cable | 1 | For power and programming | Included |

**Benefits of Arduino:**
- Built-in USB programming
- More memory for features
- Easier debugging
- More GPIO pins available
- Can add serial debug output

## Where to Buy

### Online Retailers

**International:**
- AliExpress (cheapest, slow shipping)
- eBay (various sellers)
- Banggood (good prices)

**USA:**
- Amazon (fast shipping, higher prices)
- Adafruit (high quality, good documentation)
- SparkFun (high quality, good support)
- Digi-Key (professional, wide selection)
- Mouser (professional, wide selection)

**Europe:**
- Pimoroni (UK)
- Conrad Electronics (Germany)
- RS Components (UK/Europe)

**Asia:**
- Taobao (China)
- Tokopedia (Indonesia)
- Lazada (Southeast Asia)

### Local Options

- Electronics hobby shops
- University bookstores
- Maker spaces (may have parts libraries)
- Hackerspaces

## Buying Tips

1. **Buy in Bulk**: Components are cheaper when bought in packs
2. **Combo Kits**: Look for Arduino starter kits that include components
3. **Quality vs. Price**: 
   - ATTiny85: Authentic Atmel/Microchip chips recommended
   - MAX7219 modules: Clones work fine for this project
   - Buttons: Quality varies; get tactile feedback buttons
4. **Shipping**: Factor in shipping time and cost
5. **Compatibility**: Ensure MAX7219 module is the 8x8 single matrix type

## Pre-Made Kits

Some sellers offer kits with all components. Search for:
- "ATTiny85 starter kit"
- "MAX7219 LED matrix kit"
- "Arduino LED matrix kit"

## Tools You'll Need

### Essential Tools
- None! Breadboard build requires no soldering

### Recommended Tools
- Multimeter (for troubleshooting)
- USB cable for power
- Computer with Arduino IDE

### Optional Tools (for permanent build)
- Soldering iron
- Solder
- Wire cutters/strippers
- Helping hands
- Heat shrink tubing

## Component Specifications

### ATTiny85 Specifications
- Flash: 8KB
- RAM: 512 bytes
- EEPROM: 512 bytes
- GPIO Pins: 6 (includes RESET)
- Clock: 8MHz internal
- Voltage: 2.7V - 5.5V

### MAX7219 Module Specifications
- Display: 8x8 LED matrix
- Color: Usually red (green/blue available)
- Driver: MAX7219 chip
- Interface: SPI-like (3-wire)
- Voltage: 5V
- Current: Up to 330mA

### Button Specifications
- Type: Tactile push button
- Size: 6mm x 6mm (most common)
- Actuation Force: 160gf typical
- Travel: 0.25mm
- Contact Resistance: <100mΩ

## Build Variations

### Budget Build (~$10)
- ATTiny85 (no socket)
- MAX7219 module (clone)
- Basic buttons
- Breadboard
- Recycled USB cable for power
- Use Arduino as programmer

### Standard Build (~$20)
- ATTiny85 with socket
- Quality MAX7219 module
- Good tactile buttons
- Breadboard + jumpers
- USB power adapter
- USBasp programmer

### Deluxe Build (~$40)
- Arduino Nano (for easier development)
- Quality MAX7219 module
- Premium buttons
- Custom PCB
- 3D printed case
- Rechargeable battery pack

## Substitutions

### Can substitute:
- ATTiny85 → ATTiny45/25 (less memory but works)
- ATTiny85 → Arduino Nano/Uno (easier, more features)
- MAX7219 → Any MAX7219-based 8x8 matrix
- Breadboard → Perfboard/PCB for permanent build

### Cannot substitute:
- MAX7219 → Other LED drivers (code won't work)
- 8x8 matrix → Different size (game designed for 8x8)

## Shopping List Checklist

Use this checklist when ordering:

- [ ] ATTiny85 IC (or Arduino)
- [ ] MAX7219 8x8 LED matrix module
- [ ] 3x push buttons
- [ ] Breadboard
- [ ] Jumper wires
- [ ] 5V power supply
- [ ] Programmer (Arduino or USBasp)
- [ ] Optional: DIP socket
- [ ] Optional: Capacitors
- [ ] Optional: Enclosure

---

**Note**: Prices are approximate and may vary by region and retailer. Shop around for best deals!
