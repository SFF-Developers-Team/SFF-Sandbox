#include "timer.h"
#include <stdint.h>
#include "utils.h"
#include "raylib.h"
#include "game.h"

static float passedTime = 0;

void Timer_Update() {
    static int64_t lastTime = 0;

    const float tps = TICKS_PER_SECOND;

    double time = GetTime();
    int64_t nanos = time * 1.0e9f;
    int64_t passedNS = nanos - lastTime;
    lastTime = nanos;

    if (passedNS < 0LL) {
        passedNS = 0LL;
    }

    if (passedNS > 1000000000LL) {
        passedNS = 1000000000LL;
    }

    passedTime += passedNS * tps / 1.0e9f;
    uint32_t ticks = MIN(100, passedTime);
    passedTime -= (float)ticks;

    for (int i = 0; i < ticks; i++) {
        Game_OnTick();
    }
}

float Timer_GetPartialTicks(void) {
    return passedTime;
}