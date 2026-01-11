
#include "player.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"

void updatePlayerPosition(Player *p, float *dt, int *inputX, int *inputY, const int screenW, const int screenH){

  float stepFactor = p->speed * (*dt);
  p->entity.position.x += (*inputX) * stepFactor; 
  p->entity.position.y += (*inputY) * stepFactor;
  
  
  /*
  wrapping
  */
  p->entity.position.x = (float)((int)(p->entity.position.x + screenW)%screenW);
  p->entity.position.y = (float)((int)(p->entity.position.y + screenH)%screenH);
  

}

