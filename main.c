#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// #define OBSTACLE_RADIUS 30

typedef struct Bullet
{
    Vector2 position;
    bool active;
} Bullet;

void floatPosToString(float pos, char posSym, char *buf, size_t bufSize)
{
    snprintf(buf, bufSize, "%c: %.2f", posSym, pos);
}

void scoreToString(int score, char *buf, size_t bufSize)
{
    snprintf(buf, bufSize, "score: %d", score);
}

Vector2 getRandomPositionWithNoCollision(int maxW, int maxH,
                                         Vector2 avoidTarget)
// TODO implement right checks for positioning
{
    int x = rand() % maxW + 1;
    int y = rand() % maxH + 1;
    float player_radius = 50;

    Vector2 p;
    p.x = x;
    p.y = y;

    // check isValid

    return p;
}

int main(void)
{
    srand(time(NULL));
    int score = 0;
    char scoreBuf[32];

    float speed = 240.0f;
    float dt = 1;
    float bulletSpeed = 30.0f;
    Bullet bullet = {0};

    char xBuffer[32];
    char yBuffer[32];
    const int screenW = 1500;
    const int screenH = 1500;

    float player_radius = 50;

    InitWindow(screenW, screenH, "hello diego");

    // FIXME rename it to player
    Vector2 ballPosition = {(float)screenW / 2, (float)screenH / 2};

    Vector2 enemyPosition =
        getRandomPositionWithNoCollision(screenW, screenH, ballPosition);
    bool enemyActive = true;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        dt = GetFrameTime();
        BeginDrawing();

        if (IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D))
            ballPosition.x += (speed * dt);
        if (IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A))
            ballPosition.x -= (speed * dt);
        if (IsKeyDown(KEY_UP) || IsKeyDown(KEY_W))
            ballPosition.y -= (speed * dt);
        if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S))
            ballPosition.y += (speed * dt);

        if (IsKeyPressed(KEY_SPACE) && !bullet.active)
        {
            bullet.position.x = ballPosition.x;
            bullet.position.y = ballPosition.y - (player_radius + 10);
            bullet.active = true;
        }
        if (IsKeyPressed(KEY_Q))
        {
            break;
        }

        if (bullet.active)
        {
            bullet.position.y -= bulletSpeed;

            if (bullet.position.y < 0)
                bullet.active = false;

            // draw a bullet as a 20x20 square
            DrawRectangle(bullet.position.x - 15, bullet.position.y - 15, 30,
                          30, RED);
        }

        // draw enemy as 40x40 square
        DrawRectangle(enemyPosition.x, enemyPosition.y, 40, 40, RAYWHITE);

        ClearBackground(BLACK);
        // floatPosToString(ballPosition.x, 'x', xBuffer, sizeof(xBuffer));
        // floatPosToString(ballPosition.y, 'y', yBuffer, sizeof(yBuffer));

        scoreToString(score, scoreBuf, 32);
        DrawText(scoreBuf, 10, 10, 40, RAYWHITE);
        // DrawText(xBuffer, 10, 60, 40, RAYWHITE);
        // DrawText(yBuffer, 210, 60, 40, RAYWHITE);

        DrawCircleV(ballPosition, player_radius, GREEN);

        if (enemyActive && CheckCollisionRecs((Rectangle){enemyPosition.x, enemyPosition.y, 40, 40 }, (Rectangle){bullet.position.x, bullet.position.y, 20, 20 }))
        {
            bullet.active = false;
            enemyActive = false;
            score++; 
            enemyPosition = getRandomPositionWithNoCollision(screenW, screenH,
                                                             ballPosition);
            enemyActive = true;
        }
        EndDrawing();
    }

    CloseWindow();

    return 0;
}
