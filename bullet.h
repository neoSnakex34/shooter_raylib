#ifndef BULLET_H
#define BULLET_H

#include "entity.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"

typedef struct Bullet {
  Entity entity;
  Vector2 velocity;
} Bullet;

void initShootFromEntity(Bullet *b, Entity *e, char *dir);
void updateBullet(Bullet *b, const int maxW, const int maxH); 
#endif
