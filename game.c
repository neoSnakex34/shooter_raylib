#include "bullet.h"
#include "entity.h"
#include "glob_macros.h"
#include "player.h"
#include "raylib-5.5_linux_amd64/include/raylib.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define INT_TO_STRING_BUF_SIZE 32
#define SCREEN_W 1500
#define SCREEN_H 1500
#define ENEMY_COUNT 10

void scoreToString(int score, char *buf, size_t bufSize)
{
    snprintf(buf, bufSize, "score: %d", score);
}

/*
  init game
  this should be designed in a sense
  in which one could easily restart the game by calling it
  now is just a stub initializer
  should be refactored asap
*/
void init(int *score, bool *gameOver, float *dt, Entity *enemies,
          Player *player)
{
    *score = 0;
    *gameOver = false;
    *dt = 1;

    for (size_t i = 0; i < ENEMY_COUNT; ++i)
    {
        enemies[i].position = getRandomPositionAvoidingCircle(
            SCREEN_W, SCREEN_H, &player->entity, PLAYER_RADIUS * 2);
        enemies[i].active = true;
    }
};

/*
  entry point
*/
int main(void)
{

    // SetRandomSeed(time(NULL));
    srand(time(NULL));
    int score;
    bool gameOver;
    float dt;

    Bullet bullet = {.entity.active = false, .entity.speed = BULLET_SPEED};
    Entity enemies[ENEMY_COUNT];

    Player player = {.entity.position.x = SCREEN_W / 2.0,
                     .entity.position.y = SCREEN_H / 2.0,
                     .speed = 360.0f,
                     .entity.active = true};
    init(&score, &gameOver, &dt, enemies, &player);
    char scoreBuf[INT_TO_STRING_BUF_SIZE];

    InitAudioDevice();
    Sound shootFx = LoadSound("assets/fx/shoot.ogg");
    Sound enemyDeadFx = LoadSound("assets/fx/enemy_dead.ogg");
    Music theme = LoadMusicStream("assets/music/main_track.ogg");
    SetMusicVolume(theme, 0.4f);
    PlayMusicStream(theme);

    InitWindow(SCREEN_W, SCREEN_H, "SHOOT!");

    // TODO move this
    int dirX = 0;
    int dirY = 0;
    float playerStepFactor;

    SetTargetFPS(60);
    while (!WindowShouldClose())
    {

        BeginDrawing();
        UpdateMusicStream(theme);
        dt = GetFrameTime();
        playerStepFactor = player.speed * dt;
        ClearBackground(BLACK);

        /*
            exit if needed
        */
        if (IsKeyDown(KEY_ESCAPE))
        {
            break;
        }

        /*
            player movement
        */
        dirX = 0;
        dirY = 0;
        if (IsKeyDown(KEY_W))
        {
            dirY = NEGATIVE;
        }
        if (IsKeyDown(KEY_S))
        {
            dirY = POSITIVE;
        }
        if (IsKeyDown(KEY_D))
        {
            dirX = POSITIVE;
        }
        if (IsKeyDown(KEY_A))
        {
            dirX = NEGATIVE;
        }

        /*
            draw enemies type one as stated in enemy count
        */
        for (size_t i = 0; i < ENEMY_COUNT; ++i)
        {
            if (enemies[i].active)
                DrawRectangle(enemies[i].position.x, enemies[i].position.y,
                              ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE,
                              RAYWHITE);
        }

        /*
            player position updated
            based on the movement selected
        */
        updatePlayerPosition(&player, &dt, &dirX, &dirY, SCREEN_W, SCREEN_H);
        if (player.entity.active)
        {
            DrawCircleV(player.entity.position, 50, GREEN);
        }

        if (!bullet.entity.active && !gameOver)
        {
            if (IsKeyPressed(KEY_UP))
            {
                // draw bullet
                initShootFromEntity(&bullet, &player.entity, "up");
            }

            if (IsKeyPressed(KEY_DOWN))
            {
                initShootFromEntity(&bullet, &player.entity, "down");
            }

            if (IsKeyPressed(KEY_RIGHT))
            {
                initShootFromEntity(&bullet, &player.entity, "right");
            }

            if (IsKeyPressed(KEY_LEFT))
            {
                initShootFromEntity(&bullet, &player.entity, "left");
            }

            /*
                play sound
            */
            if (bullet.entity.active)
                PlaySound(shootFx);
        }

        /*
            actually move the bullet
        */
        if (bullet.entity.active)
        {
            // TODO here play the sound?
            updateBullet(&bullet, SCREEN_W, SCREEN_H);
            DrawRectangle(bullet.entity.position.x - BULLET_TYPE_ONE_SIZE / 2.0,
                          bullet.entity.position.y - BULLET_TYPE_ONE_SIZE / 2.0,
                          BULLET_TYPE_ONE_SIZE, BULLET_TYPE_ONE_SIZE, YELLOW);
        }

        /*
            player collision check with enemies
        */
        if (atLeastOneIsActive(enemies, ENEMY_COUNT))
        {
            for (size_t i = 0; i < ENEMY_COUNT; ++i)
            {
                Entity *enemy = &enemies[i];

                if (enemy->active &&
                    CheckCollisionCircleRec(
                        player.entity.position, PLAYER_RADIUS,
                        (Rectangle){enemy->position.x, enemy->position.y,
                                    ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE}))
                {
                    gameOver = true;
                }
            }
        }

        if (atLeastOneIsActive(enemies, ENEMY_COUNT))
        {

            for (size_t i = 0; i < ENEMY_COUNT; ++i)
            {
                Entity *enemy = &enemies[i];
                if (enemy->active &&
                    CheckCollisionRecs(
                        (Rectangle){enemy->position.x, enemy->position.y,
                                    ENEMY_TYPE_ONE_SIZE, ENEMY_TYPE_ONE_SIZE},
                        (Rectangle){
                            bullet.entity.position.x, bullet.entity.position.y,
                            BULLET_TYPE_ONE_SIZE, BULLET_TYPE_ONE_SIZE}))
                {
                    printf("shoot!\n");
                    bullet.entity.active = false;
                    enemy->active = false;
                    score++;
                    enemy->position = getRandomPositionAvoidingCircle(
                        SCREEN_W, SCREEN_H, &player.entity, PLAYER_RADIUS);
                    enemy->active = true;
                }
            }
        }

        if (gameOver)
        {
            StopMusicStream(theme);
            ClearBackground(WHITE);
            player.entity.active = false;
            for (size_t i = 0; i < ENEMY_COUNT; ++i)
            {
                enemies[i].active = false;
            }

            bullet.entity.active = false;

            char *gameOverText = "Game Over\npress esc to exit";
            int textWidth = MeasureText(gameOverText, 50);

            int textStartX = (SCREEN_W / 2) - textWidth / 2;
            int textStartY = (SCREEN_H / 2) - 50 / 2;

            DrawText(gameOverText, textStartX, textStartY, 50, BLACK);
        }

        EndDrawing();
    }

    CloseAudioDevice();
    UnloadMusicStream(theme);
    UnloadSound(shootFx);
    UnloadSound(enemyDeadFx);
    CloseWindow();

    return 0;
}
