# Wave System Design Plan

## Current State
- Single invader at a time
- Linear speed increase with each kill
- No progression phases

## Desired Behavior
1. Start with 1 invader, moderate pace
2. Every 3-5 kills, increase pace
3. After N kills, add a second simultaneous invader
4. Continue ramping: more invaders + faster pace
5. Endgame: multiple fast invaders

---

## Proposed Architecture

### Invader Storage (Memory: ~25 bytes)

```cpp
// Max simultaneous invaders (memory constrained)
#define MAX_INVADERS 4

struct Invader {
  uint8_t col;    // 0-7
  uint8_t row;    // 0-7 (255 = inactive)
  bool active;
};

Invader invaders[MAX_INVADERS];  // 12 bytes
```

**Alternative (smaller):** Use `row = 255` as inactive marker, skip `active` bool.

### Wave Configuration

```cpp
struct WaveConfig {
  uint8_t maxActiveInvaders;  // 1-4 invaders allowed on screen
  uint16_t dropInterval;      // ms between drops (gravity)
  uint16_t spawnInterval;     // ms between new spawns
  uint8_t killsToAdvance;     // kills needed to reach next wave
};
```

### Tuning Table (stored in PROGMEM to save RAM)

```cpp
const WaveConfig waves[] PROGMEM = {
  // { invaders, dropInterval, spawnInterval, killsToAdvance }
  { 1, 400, 0,   5 },   // Wave 1: 1 invader, slow, 5 kills
  { 1, 350, 0,   5 },   // Wave 2: 1 invader, faster
  { 1, 300, 0,   5 },   // Wave 3: 1 invader, even faster
  { 2, 300, 500, 5 },   // Wave 4: 2 invaders, spawn every 500ms
  { 2, 250, 400, 5 },   // Wave 5: 2 invaders, faster
  { 3, 250, 350, 5 },   // Wave 6: 3 invaders
  { 3, 200, 300, 5 },   // Wave 7: 3 invaders, fast
  { 4, 200, 250, 0 },   // Wave 8+: 4 invaders, endgame (loop here)
};
#define NUM_WAVES 8
```

**Memory:** ~32 bytes in flash (PROGMEM), 0 bytes RAM

---

## Game Loop Changes

### Spawn Logic
```cpp
void trySpawnInvader() {
  // Count active invaders
  uint8_t activeCount = 0;
  for (uint8_t i = 0; i < MAX_INVADERS; i++) {
    if (invaders[i].active) activeCount++;
  }

  // Check if we can spawn (under limit and spawn timer ready)
  if (activeCount < currentWave.maxActiveInvaders && spawnTimerReady) {
    // Find inactive slot and spawn
    for (uint8_t i = 0; i < MAX_INVADERS; i++) {
      if (!invaders[i].active) {
        invaders[i].col = random(0, 8);
        invaders[i].row = 0;
        invaders[i].active = true;
        break;
      }
    }
  }
}
```

### Collision Detection
```cpp
void checkCollisions() {
  if (!bulletActive) return;

  for (uint8_t i = 0; i < MAX_INVADERS; i++) {
    if (invaders[i].active &&
        bulletRow == invaders[i].row &&
        bulletCol == invaders[i].col) {
      // Hit!
      invaders[i].active = false;
      bulletActive = false;
      score++;
      waveKills++;

      // Check wave advancement
      if (waveKills >= currentWave.killsToAdvance) {
        advanceWave();
      }
      break;
    }
  }
}
```

### Game Over Check
```cpp
void checkGameOver() {
  for (uint8_t i = 0; i < MAX_INVADERS; i++) {
    if (invaders[i].active && invaders[i].row >= 7) {
      gameOver();
      return;
    }
  }
}
```

---

## Tuning Parameters Summary

| Parameter | Description | Typical Range |
|-----------|-------------|---------------|
| `maxActiveInvaders` | Simultaneous invaders | 1-4 |
| `dropInterval` | Gravity (lower = faster) | 150-400 ms |
| `spawnInterval` | Time between spawns | 200-500 ms |
| `killsToAdvance` | Kills per wave | 3-10 |

---

## Memory Budget (ATtiny85: 512 bytes RAM)

| Item | Bytes |
|------|-------|
| Invaders array (4 × 3) | 12 |
| Wave state variables | 8 |
| Existing game state | ~30 |
| Matrix buffer | 8 |
| Stack/locals | ~50 |
| **Total estimated** | **~108** |

Plenty of headroom.

---

## Implementation Phases

### Phase 1: Multi-Invader Support
- [ ] Add invaders array
- [ ] Update draw loop for multiple invaders
- [ ] Update collision detection
- [ ] Update game over check
- [ ] Test with hardcoded 2 invaders

### Phase 2: Wave System
- [ ] Add wave configuration table
- [ ] Add spawn timer and logic
- [ ] Add wave advancement
- [ ] Test progression

### Phase 3: Tuning
- [ ] Playtest and adjust wave table values
- [ ] Balance difficulty curve
- [ ] Document final tuning values

---

## Questions for Review

1. **Max invaders:** Is 4 enough for endgame chaos, or should we support more?

2. **Spawn behavior:** When a new wave adds invaders, should they:
   - A) Spawn immediately (sudden difficulty spike)
   - B) Spawn gradually via spawnInterval (smoother)

3. **Wave looping:** At final wave, should it:
   - A) Loop the last wave forever
   - B) Keep incrementing speed infinitely
   - C) "Victory" state after X waves

4. **Visual feedback:** Flash or animation when advancing waves?

---

## Recommendation

Start with **Phase 1** - get multiple invaders working with hardcoded values. This validates the core mechanics before adding the wave system complexity.

Estimated code size increase: ~200-300 bytes (well within ATtiny85's 8KB flash).
