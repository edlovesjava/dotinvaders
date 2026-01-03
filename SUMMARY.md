# Project Summary: Dot Invaders

## Overview
Dot Invaders is a minimalist Space Invaders-style game implemented for the ATtiny85 microcontroller with an 8x8 LED matrix display driven by a MAX7219 chip.

## Implementation Details

### Hardware Requirements
- **ATtiny85** microcontroller (8MHz internal clock)
- **8x8 LED Matrix** with MAX7219 driver
- **2 Push Buttons** for left/right movement
- **5V Power Supply** (USB or battery)
- **Decoupling capacitor** (0.1µF)

### Game Features
1. **Descending Invader**: Single enemy dot descends from random column at top
2. **Player Gun**: Controllable ship at bottom row
3. **Bullet Firing**: Chord both buttons to shoot upward
4. **Collision Detection**: Bullet destroys invader on contact
5. **Progressive Difficulty**: Speed increases with each successful hit
6. **Game Over**: Triggered when invader reaches bottom row
7. **Score Display**: Visual feedback using filled LED rows
8. **Auto-Reset**: Game automatically restarts after game over

### Technical Implementation

#### Display Management
- 8x8 matrix buffer updated each frame
- Bit-banged SPI communication with MAX7219
- 60+ FPS refresh rate (1ms frame time)

#### Input Handling
- Debounced button reading (100ms interval)
- Support for chording (both buttons together)
- Separate actions for single vs. dual button press

#### Game Logic
- Frame-based timing with independent counters
- Invader speed: 300ms initially, decreasing to 100ms at max difficulty
- Bullet speed: 30ms per row (fast)
- Collision detection after entity movement, before game over check

#### Memory Efficiency
- Flash usage: ~2-3KB (of 8KB available)
- SRAM usage: ~50-100 bytes (of 512 bytes available)
- No dynamic memory allocation
- Optimized variable types (uint8_t, uint16_t)

### Code Structure

#### Main Components
1. **MAX7219 Interface**
   - `sendByte()`: Bit-bang SPI communication
   - `sendCmd()`: Send register commands
   - `updateDisplay()`: Refresh entire matrix

2. **Game State**
   - Position tracking (gun, invader, bullet)
   - Timing counters (movement, buttons)
   - Score and speed tracking

3. **Game Loop**
   - Clear and redraw display each frame
   - Process button input
   - Move game entities
   - Check collisions
   - Handle game over

4. **Game Management**
   - `setup()`: Initialize hardware and game state
   - `loop()`: Main game loop
   - `spawnInvader()`: Create new enemy
   - `gameOver()`: End sequence and reset

### Key Design Decisions

1. **Single Invader**: Simplified from classic Space Invaders to fit limited display
2. **Chording for Fire**: Maximizes use of only 2 buttons
3. **Progressive Difficulty**: Maintains engagement through speed increases
4. **Visual Score**: Uses row fills instead of numbers (no font needed)
5. **Auto-Reset**: Enables continuous play without restart button

### Files Included

- **ATTiny_DotInvaders.ino**: Main game code (243 lines)
- **README.md**: Project overview and quick start
- **HARDWARE.md**: Circuit diagrams and assembly instructions
- **GAME.md**: Detailed game mechanics and customization
- **TESTING.md**: Testing procedures and troubleshooting
- **.gitignore**: Excludes build artifacts

### Compatibility

Based on the [dotris](https://github.com/edlovesjava/dotris) hardware platform:
- Same pin configuration
- Same component requirements
- Same programming setup
- Fully compatible hardware swap

### Performance Characteristics

| Metric | Value |
|--------|-------|
| Frame Rate | ~1000 FPS (limited by 1ms delay) |
| Display Update | 8 register writes per frame |
| Button Polling | Every 100ms |
| Invader Speed | 300ms → 100ms (dynamic) |
| Bullet Speed | 30ms per row |
| Memory Footprint | <3KB flash, <100 bytes SRAM |

### Known Limitations

1. **Button Labels**: May be reversed due to hardware wiring (documented)
2. **Random Seed**: Uses millis() which may be similar across power cycles
3. **Single Bullet**: Only one bullet can be active at a time
4. **Score Cap**: Display limited to 8 rows (shows score up to 8)
5. **No Persistence**: Score resets on power cycle (no EEPROM save)

### Future Enhancement Possibilities

- Multiple simultaneous invaders
- Invader shooting back
- Shield structures
- Sound effects (piezo buzzer)
- High score persistence (EEPROM)
- Different difficulty levels
- Power-ups
- Boss battles

### Testing Status

✅ Code compiles successfully
✅ Logic reviewed and validated
✅ Timing calculations verified
✅ Boundary conditions checked
✅ Memory usage optimized
✅ Security review passed (no vulnerabilities)
✅ Documentation complete

### Notes for Users

1. Hardware setup identical to dotris - users can swap between games
2. Comprehensive testing guide included (TESTING.md)
3. All timing parameters easily adjustable in code
4. Visual documentation aids assembly
5. No external libraries required (uses Arduino core only)

## Conclusion

This implementation delivers a complete, playable game that:
- Meets all requirements from the problem statement
- Runs efficiently on minimal hardware
- Provides engaging, progressive gameplay
- Includes comprehensive documentation
- Follows embedded systems best practices
- Maintains code quality and safety standards

The game is ready for deployment on ATtiny85 hardware and should provide hours of retro gaming enjoyment!
