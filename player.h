#ifndef PLAYER_H
#define PLAYER_H

#include "entity.h"
#include <raylib.h>

typedef struct Player {
  Entity entity;
  float speed;  
} Player; 

 
void updatePlayerPosition(Player *p, float *dt, int *inputX, int *inputY, const int screenW, const int screenH);

#endif

