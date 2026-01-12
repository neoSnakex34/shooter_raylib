#ifndef ENTITY_H
#define ENTITY_H

#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>

typedef struct Entity {
  Vector2 position;
  bool active;
  Texture2D texture; 
  int speed; 
} Entity;

bool atLeastOneIsActive(Entity *e, size_t size);
int getActiveEntity(Entity *e, size_t size);
Vector2 getRandomPositionAvoidingCircle(int maxW, int maxH, Entity* target, int radius);


#endif 
