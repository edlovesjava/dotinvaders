# Dot Invaders - Game Enhancements

Tracking document for iterative improvements and playtesting.

---

## Items Needing Attention

### 1. Random Seed Quality (Priority: Medium)
**File:** `ATTiny_DotInvaders.ino:87`

**Issue:** Current implementation uses `randomSeed(millis())` which returns near-zero at power-on, making invader spawn positions predictable.

**Previous approach:** `randomSeed(analogRead(3))` - reads electrical noise from floating pin.

**Suggested fix:** Combine both for better entropy:
```cpp
randomSeed(analogRead(3) ^ millis());
```

**Status:** [ ] Not started

---

### 2. Score Display Limitation (Priority: Low)
**File:** `ATTiny_DotInvaders.ino:283-287`

**Issue:** Score display caps at 8 lit rows. Players scoring higher than 8 see the same display.

**Options:**
- A) Binary display (8 bits = 0-255 range)
- B) Scrolling digits
- C) Flash count (flash N times for score N)
- D) Keep simple, document as intentional

**Status:** [ ] Not started

---

### 3. Speed Curve Balance (Priority: Medium)
**File:** `ATTiny_DotInvaders.ino:101, 185`

**Issue:** Linear speed increase may become too difficult too quickly.

**Current formula:**
```cpp
invaderDropInterval = 300 - (speed * 10);  // Min: 100ms at speed 20
```

**Options:**
- A) Logarithmic curve (slower ramp-up)
- B) Plateau at certain scores
- C) Adjustable difficulty setting
- D) Playtest and tune constants

**Status:** [ ] Needs playtesting

---

## Completed Enhancements

### Key Repeat for Gun Movement
**Commit:** `7346917`

Added typematic key repeat - hold a direction button to continuously move after 250ms initial delay, then every 50ms.

**Constants:**
- `KEY_REPEAT_DELAY = 250` (initial delay)
- `KEY_REPEAT_RATE = 50` (repeat interval)

---

### Continuous Movement After Fire (A/B Test)
**Commit:** `b16b2c6`

When sliding and tap-firing, movement now resumes seamlessly without resetting key repeat delay.

**A/B Test Flag:**
```cpp
const bool CONTINUE_MOVE_AFTER_FIRE = true;  // Option A (default)
```

---

## Future Ideas (Backlog)

### Gameplay
- [ ] Multiple invaders at higher scores
- [ ] Power-ups (faster bullets, wider gun, shield)
- [ ] Boss invaders (take multiple hits)
- [ ] Combo scoring for consecutive hits

### Controls
- [ ] Sensitivity adjustment
- [ ] Alternative fire method (double-tap?)

### Visuals
- [ ] Hit animations/patterns
- [ ] Level transitions
- [ ] High score persistence (EEPROM)

### Audio (if piezo available)
- [ ] Fire sound
- [ ] Hit sound
- [ ] Game over jingle

---

## Testing Notes

| Date | Tester | Version | Notes |
|------|--------|---------|-------|
| | | | |

---

## Hardware Constraints

- **MCU:** ATtiny85 (8KB flash, 512B RAM)
- **Display:** MAX7219 8x8 LED matrix
- **Input:** 2 buttons (LEFT/RIGHT, chord to fire)
- **No audio hardware currently**

Keep implementations minimal - every byte counts!
