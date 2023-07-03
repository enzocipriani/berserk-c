#include <raylib.h>
#include "jogo.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int main(void)
{
    Score scores[MAX_SCORES];
    // puxa os scores do arquivo
    readScores(scores);

    double startTime = getTime();
    Game game;
    game.screenWidth = 800;
    game.screenHeight = 480;

    InitWindow(game.screenWidth, game.screenHeight, "Jogo AEDS1");
    SetTargetFPS(60);

    char nickname[20] = {0};
    bool nickname_ok = false;

    Difficulty difficulty = setDifficulty(); // chama a função selecionarDificuldade

    InitGame(&game, difficulty); // passa a dificuldade escolhida para a função InitGame

    InitAudioDevice();
    Sound intro = LoadSound("sounds/background_sound.mp3");
    PlaySound(intro);

    while (!WindowShouldClose()) {
        if(!nickname_ok){
            BeginDrawing();
            ClearBackground(RAYWHITE);
            apelido(nickname, 20);
            EndDrawing();
            if (IsKeyPressed(KEY_ENTER)) nickname_ok = true;
            
        }else{
            UpdateDrawFrame(&game);
            if(game.gameover){
                if(game.reset_required){ // se o jogador morreu, reseta o jogo
                    while(!IsKeyDown(KEY_SPACE)){
                        BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("GAME OVER\nPressione ESPAÇO para reiniciar", GetScreenWidth()/2 - MeasureText("GAME OVER", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
                        EndDrawing();
                        
                    }
                    game.reset_required = 0;
                    game.gameover = 0;
                    ResetMap(&game, game.curr_map);
                    continue;
                }
                else if(game.boss_dead){
                    while(!IsKeyDown(KEY_ENTER)){
                        BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("Você venceu!\nPressione ENTER para sair", GetScreenWidth()/2 - MeasureText("Você venceu!", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
                        EndDrawing();
                    }   
                    double endTime = getTime();
                    double totalTime = endTime - startTime;
                    updateScores(scores, nickname, totalTime);
                    writeScores(scores);
                    break;
                } 
                else if (!game.boss_dead){
                    while(!IsKeyDown(KEY_ENTER)){
                        BeginDrawing();
                        ClearBackground(RAYWHITE);
                        DrawText("GAME OVER\nPressione ENTER para sair\nPress F For Elverton", GetScreenWidth()/2 - MeasureText("GAME OVER", 20)/2, GetScreenHeight()/2 - 50, 20, BLACK);
                        EndDrawing();
                    }
                    break;
                } 
            }
        }
    }
    int y = GetScreenHeight()/2 - 50;
    while(!IsKeyDown(KEY_SPACE)){
        BeginDrawing();
        ClearBackground(RAYWHITE);
        DrawText("Scores", GetScreenWidth()/2 - MeasureText("Scores", 20)/2, y, 20, BLACK);
        for(int i = 0; i < MAX_SCORES; i++){
            char scoreText[50];
            sprintf(scoreText, "%s %.2lf", scores[i].name, scores[i].score);
            DrawText(scoreText, GetScreenWidth()/2 - MeasureText(scoreText, 20)/2, y + (i+1)*30, 20, BLACK);
        }
        DrawText("Pressione Espaço para Sair", GetScreenWidth()/2 - MeasureText("ScPressione Espaço para Sairores", 20)/2 +30, y + 150, 20, BLACK);
        EndDrawing();
    }

    return 0;
}