#ifndef GAME_H
#define GAME_H

#include "world.h"
#include "player.h"
#include "defines.h"

extern LocalPlayer localPlayers[MAX_LOCAL_PLAYERS];
extern int localPlayersCount;

extern ControlType mainControlType;
extern int mainGamepad;

extern bool shouldClose;

void Game_Init(void);
void Game_Update(float dt);
void Game_Draw(void);
void Game_OnTick(void);
int  Game_AddLocalPlayer(ControlType controlType, int gamepad);

#endif