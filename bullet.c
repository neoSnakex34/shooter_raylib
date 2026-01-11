#include <stdlib.h>
#include <string.h>
#include "bullet.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"
#include "glob_macros.h"

#define B_POSITION b->entity.position 
#define B_VELOCITY b->velocity
#define B_ACTIVE b->entity.active 
/*
  dir would either be up, down, right, left 
*/
void initShootFromEntity(Bullet *b, Entity *e, char *dir) {
  printf("%s\n", dir);

  // TODO refactor using something more fancy 
  if (strcmp(dir, "up") == 0){
    B_POSITION.x = e->position.x;
    B_POSITION.y = e->position.y - (PLAYER_RADIUS);
    B_VELOCITY.x = 0;
    B_VELOCITY.y = -1; 
  }

  else if (strcmp(dir, "down") == 0) {
    
    B_POSITION.x = e->position.x;
    B_POSITION.y = e-> position.y + (PLAYER_RADIUS);
    B_VELOCITY.x = 0;
    B_VELOCITY.y = 1; 
  }

  else if (strcmp(dir, "right") == 0) {
    
    B_POSITION.x = e->position.x + (PLAYER_RADIUS );
    B_POSITION.y = e-> position.y ;
    B_VELOCITY.x = 1;
    B_VELOCITY.y = 0; 
  }

  else if (strcmp(dir, "left") == 0) {
    
    B_POSITION.x = e->position.x - (PLAYER_RADIUS);
    B_POSITION.y = e-> position.y ;
    B_VELOCITY.x = -1;
    B_VELOCITY.y = 0; 
  }

  else {
    B_ACTIVE = false; 
    printf("wrong shooting direction\n");
    return;
  }

  B_ACTIVE = true; 

  
}

void updateBullet(Bullet *b, const int maxW, const int maxH) {

   if (!B_ACTIVE) return;

    B_POSITION.x += B_VELOCITY.x * b->entity.speed;
    B_POSITION.y += B_VELOCITY.y * b->entity.speed;

    if ((B_POSITION.y < 0) || (B_POSITION.y > maxH) || (B_POSITION.x < 0) || (B_POSITION.x > maxW)) {
      B_ACTIVE = false; 
    }
    
}
