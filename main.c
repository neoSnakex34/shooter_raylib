#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define PLAYER_RADIUS 50
#define RIGHT 1
#define LEFT -1
#define UP -1
#define DOWN 1

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

// x  | width
// y  | height
// 1  | right
// -1 | left
void positionStepAndNormalize(int direction, float stepFactor, float *pos,
                              const int screenDim)
{
    if ((direction != -1) && (direction != 1))
    {
        printf("POSITION MUST EITHER BE 1 OR -1");
        return;
    }
    // stepfactor will usually be speed*dt
    *pos += direction * stepFactor;
    *pos = (float)((int)(*pos + screenDim) % screenDim);
}

Vector2 getRandomPositionWithNoCollision(int maxW, int maxH,
                                         Vector2 avoidTarget)
// TODO implement right checks for positioning
{
    int x = rand() % maxW + 1;
    int y = rand() % maxH + 1;

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
    // WARNING this is kinda risky memorywise
    float *positionDirectionToModifyAsPointer = NULL;
    int direction = UP; // fallback to up

    // char xBuffer[32];
    // char yBuffer[32];
    const int screenW = 1500;
    const int screenH = 1500;

    InitAudioDevice();
    Sound shootFx = LoadSound("assets/fx/shoot.ogg");
    Sound enemyDeadFx = LoadSound("assets/fx/enemy_dead.ogg");


    InitWindow(screenW, screenH, "Shoot!");

    // FIXME rename it to player
    Vector2 ballPosition = {(float)screenW / 2, (float)screenH / 2};
    bool playerActive = true;
    
    Vector2 enemyPosition =
        getRandomPositionWithNoCollision(screenW, screenH, ballPosition);
    bool enemyActive = true;
    bool gameOver = false;

    // debug purposes
    int c = 0;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        dt = GetFrameTime();
        float stepFactor = speed * dt;
        BeginDrawing();

        if (IsKeyPressed(KEY_Q))
        {
            break;
        }

        if (IsKeyDown(KEY_D))
            positionStepAndNormalize(1, stepFactor, &ballPosition.x, screenW);
        if (IsKeyDown(KEY_A))
            positionStepAndNormalize(-1, stepFactor, &ballPosition.x, screenW);
        if (IsKeyDown(KEY_W))
            positionStepAndNormalize(-1, stepFactor, &ballPosition.y, screenH);
        if (IsKeyDown(KEY_S))
            positionStepAndNormalize(1, stepFactor, &ballPosition.y, screenH);

        if (!bullet.active && !gameOver)
        {
            if (IsKeyPressed(KEY_UP))
            {
                bullet.position.x = ballPosition.x;
                bullet.position.y = ballPosition.y - (PLAYER_RADIUS + 10);
                positionDirectionToModifyAsPointer = &bullet.position.y;
                direction = UP;
                bullet.active = true;
            }

            if (IsKeyPressed(KEY_RIGHT))
            {
                bullet.position.x = ballPosition.x + (PLAYER_RADIUS + 10);
                bullet.position.y = ballPosition.y;
                positionDirectionToModifyAsPointer = &bullet.position.x;
                direction = RIGHT;
                bullet.active = true;
            }

            if (IsKeyPressed(KEY_LEFT))
            {
                bullet.position.x = ballPosition.x - (PLAYER_RADIUS + 10);
                bullet.position.y = ballPosition.y;
                positionDirectionToModifyAsPointer = &bullet.position.x;
                direction = LEFT;
                bullet.active = true;
            }
            if (IsKeyPressed(KEY_DOWN))
            {

                bullet.position.x = ballPosition.x;
                bullet.position.y = ballPosition.y + (PLAYER_RADIUS + 10);
                positionDirectionToModifyAsPointer = &bullet.position.y;
                direction = DOWN;
                bullet.active = true;
            }

            // if an action triggers a bullet
            // i will play the sound
            if (bullet.active)
                PlaySound(shootFx);
        }

        if (bullet.active && positionDirectionToModifyAsPointer)
        {
            *positionDirectionToModifyAsPointer += bulletSpeed * direction;

            if ((bullet.position.y < 0) || (bullet.position.y > screenH) ||
                (bullet.position.x < 0) || (bullet.position.x > screenW))
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

        // draw main player
        if (playerActive) DrawCircleV(ballPosition, PLAYER_RADIUS, GREEN);

        // DrawRectangle(ballPosition.x-((PLAYER_RADIUS*sqrt(2))/2),
        // ballPosition.y-((PLAYER_RADIUS*sqrt(2))/2), PLAYER_RADIUS*sqrt(2),
        // PLAYER_RADIUS*sqrt(2), YELLOW);

        // there will be a little bit of a threshold for the player hitbox, for
        // now
        if (enemyActive &&
            CheckCollisionCircleRec(
                ballPosition, PLAYER_RADIUS,
                (Rectangle){enemyPosition.x, enemyPosition.y, 40, 40}))
        {
            gameOver = true;
        }

        if (gameOver)
        {
            ClearBackground(RAYWHITE);
            playerActive = false;
            enemyActive = false;
            bullet.active = false; 
            DrawText("Game Over, press q to quit", screenW/2, screenH/2, 50, BLACK);
        }
        if (enemyActive &&
            CheckCollisionRecs(
                (Rectangle){enemyPosition.x, enemyPosition.y, 40, 40},
                (Rectangle){bullet.position.x, bullet.position.y, 20, 20}))
        {
            bullet.active = false;
            enemyActive = false;
            PlaySound(enemyDeadFx);
            score++;
            enemyPosition = getRandomPositionWithNoCollision(screenW, screenH,
                                                             ballPosition);
            enemyActive = true;
        }
        EndDrawing();
    }

    UnloadSound(shootFx);
    UnloadSound(enemyDeadFx);
    CloseAudioDevice();
    CloseWindow();

    return 0;
}
