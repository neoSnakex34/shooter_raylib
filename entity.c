
#include "entity.h"
#include "glob_macros.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <stddef.h>

bool atLeastOneIsActive(Entity *e, size_t size)
{
    // TODO maybe add a check for size > 0
    for (size_t i = 0; i < size; ++i)
    {
        if (e[i].active == true)
            return true;
    }
    return false;
}

Vector2 getRandomPositionAvoidingCircle(int maxW, int maxH, Entity *target,
                                        int radius)
{

    // double radius threshold
    int x = GetRandomValue(100, maxW - 100);
    int y = GetRandomValue(100, maxH - 100);

    // are x, y valid?
    while (!(((x - target->position.x) * (x - target->position.x) +
              (y - target->position.y) * (y - target->position.y)) >
             (radius * radius)))
    {
        x = GetRandomValue(100, maxW - 100);
        y = GetRandomValue(100, maxH - 100);
    }

    Vector2 newPosition;
    newPosition.x = x;
    newPosition.y = y;

    return newPosition;
}

int getActiveEntity(Entity *e, size_t size)
{
    int c = 0;
    for (size_t i = 0; i < size; ++i)
        if (e[i].active)
            c++;

    return c;
}
