#include <raylib.h>
#include "jogo.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

// função para chamar o apelido do usuario
void apelido(char *player_name, int max_length) {
    DrawText("Digite seu apelido:", 10, 10, 20, BLACK);

    DrawRectangle(10, 40, 300, 30, LIGHTGRAY);
    DrawText(player_name, 15, 45, 20, BLACK);

    int key = GetCharPressed();

    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (strlen(player_name) < max_length - 1)) {
            int length = strlen(player_name);
            player_name[length] = (char)key;
            player_name[length + 1] = '\0';
        }

        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        int length = strlen(player_name);
        if(length > 0) {
            player_name[length - 1] = '\0';
        }
    }
}

// Função que lê os scores do arquivo
void readScores(Score pontuacoes[MAX_SCORES]){
    FILE *arq = fopen("pontuacoes.txt", "r");
    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    for(int i = 0; i < MAX_SCORES; i++){
        if(fscanf(arq, "%s %lf", pontuacoes[i].name, &pontuacoes[i].score) != 2){
            // Erro ao ler, definir pontuação como 0
            strcpy(pontuacoes[i].name, "Ninguém");
            pontuacoes[i].score = 10000000;
        }
    }
    fclose(arq);
}

// Função que ordena os scores
void sortScores(Score pontuacoes[MAX_SCORES]){
    for(int i =0; i < MAX_SCORES -1; i++){
        for(int j = i+1; j < MAX_SCORES; j++){
            if(pontuacoes[i].score > pontuacoes[j].score){
                Score aux = pontuacoes[i];
                pontuacoes[i] = pontuacoes[j];
                pontuacoes[j] = aux;
            }
        }
    }
}

// Função que da upgrade nos scores
void updateScores(Score pontuacoes[MAX_SCORES], char *nome, double time){
    if(time < pontuacoes[MAX_SCORES - 1].score){
        // Insere o novo valor no final do vetor
        pontuacoes[MAX_SCORES - 1].score = time;
        strcpy(pontuacoes[MAX_SCORES - 1].name, nome);
        pontuacoes[MAX_SCORES - 1].name[19] = '\0';
        // Ordena os scores
        sortScores(pontuacoes);
    }
}

// Função que escreve os scores no arquivo
void writeScores(Score pontuacoes[MAX_SCORES]){
    FILE *arq = fopen("pontuacoes.txt", "w");
    if(arq == NULL){
        printf("Erro ao abrir o arquivo\n");
        return;
    }
    for(int i = 0; i < MAX_SCORES; i++){
        fprintf(arq, "%s %.2lf\n", pontuacoes[i].name, pontuacoes[i].score);
    }
    fclose(arq);
}

double getTime(){
    clock_t t = clock();
    double timeInSeconds = ((double)t)/CLOCKS_PER_SEC;
    return timeInSeconds;
}      

// Initialize game variables
void InitGame(Game *g, Difficulty difficulty){

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        g->hero.projectiles[i].active = false;
    }

    g->maps[0].door_open = false;
    g->curr_map = 0;
    g->num_maps = 8;
    g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
    g->hero.tex_hero = LoadTexture("sprites/hero.png");
    g->hero.color = (Color) {255, 255, 255, 0};
    g->hero.speed = 6;
    g->hero.special = 0;
    g->gameover = 0;
    g->reset_required = false;

    map0_setup(g);
    map1_setup(g);
    map2_setup(g);
    map3_setup(g);
    map4_setup(g);
    map5_setup(g);
    map6_setup(g);
    map7_setup(g);

    dificuldade (difficulty, &g->hero, g->maps, g->num_maps);

}

//função pra poder executar a dificuldade
void dificuldade(Difficulty difficulty, Hero *hero, Map *maps, int num_maps) {
    if (difficulty == NORMAL) {
        hero->max_projectiles = 2;
        for (int i = 0; i < num_maps; i++) {
            for (int j = 0; j < maps[i].num_enemies; j++) {
                maps[i].enemies[j].max_projectiles = 1;
            }
        }
    } else if (difficulty == HARD) {
        hero->max_projectiles = 1;
        for (int i = 0; i < num_maps; i++) {
            for (int j = 0; j < maps[i].num_enemies; j++) {
                maps[i].enemies[j].max_projectiles = 2;
            }
        }
    }
}

Difficulty setDifficulty(){
    int selection = 0;
    while (!IsKeyPressed(KEY_ENTER)){
        BeginDrawing();
        ClearBackground(RAYWHITE);

        if(IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_DOWN))
        {
            selection = !selection;
        } 

        DrawText("Escolha a dificuldade", 20, 20, 20, BLACK);
        DrawText("Normal", 20, 50, 20, selection ? BLACK : GRAY);
        DrawText("Hard", 20, 80, 20, selection ? GRAY : BLACK);

        EndDrawing();
    }

    return selection ? HARD : NORMAL;
}

// Update game (one frame)
void UpdateGame(Game *g)
{
    update_hero_pos(g);
    Map *map = &g->maps[g->curr_map];
    for(int i = 0; i < map->num_enemies; i++){
        if(!map->enemies[i].draw_enemy) continue;
        update_enemy_pos(g, &map->enemies[i], g->curr_map);
        Vector2 speed = {g->hero.pos.x - map->enemies[i].pos.x, g->hero.pos.y - map->enemies[i].pos.y};
        float magnitude = sqrt(speed.x * speed.x + speed.y * speed.y);
        Vector2 direction = {speed.x / magnitude, speed.y / magnitude};
        float desired_speed = 5.0;
        Vector2 velocity = {direction.x * desired_speed, direction.y * desired_speed};
        if(fabs(velocity.x) > fabs(velocity.y)){
            velocity.y = 0;
        } else {
            velocity.x = 0;
        }
        if(map->enemies[i].cooldown <= 0){
            EnemyShootProjectile(&map->enemies[i], velocity);
            map->enemies[i].cooldown = 10;
        } else {
            map->enemies[i].cooldown--;
        }
        for(int j = 0; j < MAX_PROJECTILES; j++) {
            Projectile *projectile_enemy = &map->enemies[i].projectiles[j];
            if(projectile_enemy->active){
                projectile_enemy->pos.x += projectile_enemy->speed.x;
                projectile_enemy->pos.y += projectile_enemy->speed.y;
                if (CheckCollisionCircleRec (projectile_enemy->pos, 5, g->hero.pos)) {
                    g->gameover = 1;
                    g->reset_required = true;
                } 
                if (projectile_enemy->pos.x < 0 || projectile_enemy->pos.x > 800 || projectile_enemy->pos.y < 0 || projectile_enemy->pos.y > 480){
                    projectile_enemy->active = false;
                    continue;
                }
                for(int k = 0; k < map->num_barriers; k++){
                    if (CheckCollisionCircleRec(projectile_enemy->pos, 5, map->barriers[k])) {
                        projectile_enemy->active = false;
                        continue;
                    }
                }
            } 
        }
        if(!CheckCollisionRecs(g->hero.pos, map->enemies[i].pos))
            continue;
        
        if(g->hero.special){
            map->enemies[i].draw_enemy = 0;
            for(int j = 0; j < MAX_PROJECTILES; j++){
                map->enemies[i].projectiles[j].active = false;
                
            }
            if(map->enemies[i].has_key) {
                map->door_locked = 0;
            }
            continue;
        }
        g->gameover = 1;
        g->reset_required = true;
    }

    if(CheckCollisionRecs(g->hero.pos, map->special_item) && map->draw_special_item){
        g->hero.pos.width += 10;
        g->hero.pos.height += 10;
        g->hero.special = 1;
        map->draw_special_item = 0;
    }

    if(CheckCollisionRecs(g->hero.pos, map->door) && map->door_open){
        g->curr_map = map->next_map;
        g->hero.pos = (Rectangle) { 50, 200, STD_SIZE_X, STD_SIZE_Y};
        g->hero.special = 0;
    }

    if(map->prev_map != -1 && CheckCollisionRecs(g->hero.pos, map->prev_door)){
        g->curr_map = map->prev_map;
        g->hero.pos = (Rectangle) { g->screenWidth-50, g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->hero.special = 0;
    }

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        Projectile *projectile = &g->hero.projectiles[i];
        if (projectile->active) {
            projectile->pos.x += projectile->speed.x;
            projectile->pos.y += projectile->speed.y;
            
            // Verifica colisão do projétil com os inimigos
            
            for (int j = 0; j < map->num_enemies; j++) {
                if (map->enemies[j].draw_enemy && CheckCollisionCircleRec(projectile->pos, 5, map->enemies[j].pos)) {
                    InitAudioDevice();
                    Sound kill = LoadSound("sounds/kill.mp3");
                    PlaySound(kill);

                    if (map->enemies[j].has_key) {
                        map->door_open = true;
                    }
                    map->enemies[j].draw_enemy = 0;
                    for(int k = 0; k < MAX_PROJECTILES; k++){
                        map->enemies[j].projectiles[k].active = false;
                    }
                    projectile->active = false;
                    continue;
                }
            }      
            
            // Verifica colisão do projétil com as barreiras
            for (int j = 0; j < map->num_barriers; j++) {
                if (CheckCollisionCircleRec(projectile->pos, 5, map->barriers[j])) {
                    projectile->active = false;
                    continue;
                }
            }

            // Verifica colisão do projetil com o limite do mapa
            if(projectile->pos.x < 0 || projectile->pos.x > 800 || projectile->pos.y < 0 || projectile->pos.y > 480) {
                projectile->active = false;
                continue;               
            }
        }
    }
    if (map->boss_alive){
        update_boss_pos(&map->boss, g);
        Vector2 speed = {g->hero.pos.x - map->boss.pos.x, g->hero.pos.y - map->boss.pos.y};
        float magnitude = sqrt(speed.x * speed.x + speed.y * speed.y);
        Vector2 direction = {speed.x / magnitude, speed.y / magnitude};
        float desired_speed = 5.0;
        Vector2 velocity = {direction.x * desired_speed, direction.y * desired_speed};
        // tiro do boss com cooldown
        if (map->boss.cooldown == 0) {
            map->boss.cooldown = 10;
            BossShootProjectile(&map->boss, velocity);
        } else {
            map->boss.cooldown--;
        }

        // verifica a colisao do heroi com o boss
        if (CheckCollisionRecs(g->hero.pos, map->boss.pos)){
            g->gameover = 1;
            g->reset_required = false;
            g->boss_dead = false;
        }
        //verifica os tiros do boss com o heroi
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            Projectile *projectile_enemy = &map->boss.projectiles[i];
            // update da posicao do projetil do boss
            if (projectile_enemy->active) {
                projectile_enemy->pos.x += projectile_enemy->speed.x;
                projectile_enemy->pos.y += projectile_enemy->speed.y;
            }

            if (projectile_enemy->active) {
                // verifica a colisao do projetil do boss com o heroi
                if (CheckCollisionCircleRec(projectile_enemy->pos, 5, g->hero.pos)) {
                    g->gameover = 1;
                    g->reset_required = false;
                    g->boss_dead = false;
                    continue;
            }
            }
            // verifica a colisao do projetil do boss com as barreiras
            for (int j = 0; j < map->num_barriers; j++) {
                if (CheckCollisionCircleRec(projectile_enemy->pos, 5, map->barriers[j])) {
                    projectile_enemy->active = false;
                    continue;
                }
            }
            // verifica a colisao do projetil do boss com o limite do mapa
            if(projectile_enemy->pos.x < 0 || projectile_enemy->pos.x > 800 || projectile_enemy->pos.y < 0 || projectile_enemy->pos.y > 480) {
                projectile_enemy->active = false;
                continue;               
            }
        }

        // verifica a colisao do projetil com o boss
        for (int i = 0; i < MAX_PROJECTILES; i++) {
            Projectile *projectile = &g->hero.projectiles[i];
            if (projectile->active) {
                if (CheckCollisionCircleRec(projectile->pos, 5, map->boss.pos)) {
                    map->boss.health -= 1;
                    projectile->active = false;
                    if (map->boss.health == 0){
                        map->boss_alive = 0;
                        g->gameover = 1;
                        g->boss_dead = true;
                    }
                    continue;
                }
            }
        }
    }
}

// Draw game (one frame)
void DrawGame(Game *g)
{
    
    BeginDrawing();

    ClearBackground(RAYWHITE);
    DrawRectangle(0, 0, g->screenWidth, g->screenHeight, GRAY);
    draw_borders(g);
    draw_map(g);

    for (int i = 0; i < MAX_PROJECTILES; i++) {
        if (g->hero.projectiles[i].active) {
            DrawCircleV(g->hero.projectiles[i].pos, 5, BLUE);
        }
    }

    Map *map = &g->maps[g->curr_map];

    for (int i = 0; i < map->num_enemies; i++) {
        for(int j = 0; j < MAX_PROJECTILES; j++){
            
            if (map->enemies[i].projectiles[j].active) {
                DrawCircleV(map->enemies[i].projectiles[j].pos, 5, BLUE);
            }
        }
    }
    if(map->boss_alive){
        for(int j = 0; j < MAX_PROJECTILES; j++){
            if (map->boss.projectiles[j].active) {
                DrawCircleV(map->boss.projectiles[j].pos, 5, ORANGE);
            }
        }
    }

    if (map->door_locked) {
        DrawRectangleRec(map->door, RED);
    } else {
        DrawRectangleRec(map->door, map->door_open ? GREEN : RED);
    }

    DrawRectangleRec(g->hero.pos, g->hero.color);
    
    DrawTexture(g->hero.tex_hero, g->hero.pos.x, g->hero.pos.y, (Color) {255, 255, 255, 255});

    EndDrawing();
}

// Update and Draw (one frame)
void UpdateDrawFrame(Game *g)
{
    UpdateGame(g);
    DrawGame(g);
}

void draw_borders(Game *g)
{
    DrawRectangle(0, 0, SCREEN_BORDER, g->screenHeight, BLACK);
    DrawRectangle(0, 0, g->screenWidth, SCREEN_BORDER, BLACK);
    DrawRectangle(g->screenWidth-SCREEN_BORDER, 0, g->screenWidth, g->screenHeight, BLACK);
    DrawRectangle(0, g->screenHeight-SCREEN_BORDER, g->screenWidth, g->screenHeight, BLACK);
}

void draw_map(Game *g){
    Map *map = &g->maps[g->curr_map];
    DrawTexture(map->background, 0, 0, (Color) {255, 255, 255, 255});

    for(int i = 0; i < map->num_barriers; i++){
        DrawRectangleRec(map->barriers[i], WHITE);
    }

    if(map->door_locked)
        DrawRectangleRec(map->door, RED);
    else
        DrawRectangleRec(map->door, GREEN);

    if(map->prev_map != -1){
        DrawRectangleRec(map->prev_door, GREEN);
    }

    for(int i=0; i< map->num_enemies; i++){
        map->enemies[i].tex = LoadTexture("sprites/enemy.png");
        if(!map->enemies[i].draw_enemy) continue;
        //DrawRectangleRec(map->enemies[i].pos, PINK);
        DrawTexture(map->enemies[i].tex, map->enemies[i].pos.x, map->enemies[i].pos.y, (Color) {255, 255, 255, 255});
    }

    if(map->boss_alive){
        DrawRectangleRec(map->boss.pos, map->boss.color);
        DrawTexture(map->boss.tex_boss, map->boss.pos.x, map->boss.pos.y, (Color) {255, 255, 255, 255});
        DrawBossHealthBar(&map->boss);
    }

    if(map->draw_special_item)
        DrawRectangleRec(map->special_item, PURPLE);
}

int barrier_collision(Map *map, Rectangle *pos) {
    for (int i = 0; i < map->num_barriers; i++) {
        Rectangle *barrier = &(map->barriers[i]);
        if (CheckCollisionRecs(*pos, *barrier)) {
            return 1; // Colisão detectada
        }
    }
    return 0; // Sem colisão
}

void ResetMap(Game *g, int curr_map) {
    // Call the appropriate setup function for the current map
    switch (curr_map) {
        case 0:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[0].background = LoadTexture("backgrounds/mp0.png");
            g->maps[0].num_barriers = 4;
            g->maps[0].barriers[0] = (Rectangle) {g->screenWidth/3, 100, 10, 0.6 * g->screenHeight};
            g->maps[0].barriers[1] = (Rectangle) {2*g->screenWidth/30, 200, 220, 0.025 * g->screenHeight};
            g->maps[0].color = GRAY;
            g->maps[0].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[0].num_enemies = 2;
            g->maps[0].door_locked = 0;

            for(int i=0; i< g->maps[0].num_enemies; i++){
                g->maps[0].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[0].enemies[i].color = WHITE;
                g->maps[0].enemies[i].speed = 3;
                g->maps[0].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[0].enemies[i].draw_enemy = 1;
                g->maps[0].enemies[i].has_key = 1;
                g->maps[0].enemies[i].max_projectiles = 2;
                
            }
            g->maps[0].enemies[0].has_key = 1;
            g->maps[0].prev_map = -1;
            g->maps[0].next_map = 1;
            break;
        case 1:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[1].num_barriers = 2;
            g->maps[1].barriers[0] = (Rectangle) {g->screenWidth/5, 125, 500, 0.025 * g->screenHeight};
            g->maps[1].barriers[1] = (Rectangle) {g->screenWidth/5, 400, 500, 0.025 * g->screenHeight};
            g->maps[1].color = GRAY;
            g->maps[1].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[1].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[1].num_enemies = 2;
            g->maps[1].door_locked = 0;

            for(int i=0; i< g->maps[1].num_enemies; i++){
                g->maps[1].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3 - 50, STD_SIZE_X, STD_SIZE_Y};
                g->maps[1].enemies[i].color = GRAY;
                g->maps[1].enemies[i].speed = 7;
                g->maps[1].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[1].enemies[i].draw_enemy = 1;
                g->maps[1].enemies[i].has_key = 0;
                g->maps[1].enemies[i].max_projectiles = 2;
            }
            g->maps[1].enemies[0].has_key = 1;
            g->maps[1].prev_map = 0;
            g->maps[1].next_map = 2;
            break;
        case 2:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[2].num_barriers = 3;
            g->maps[2].barriers[0] = (Rectangle) {g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
            g->maps[2].barriers[1] = (Rectangle) {2*g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
            g->maps[2].barriers[2] = (Rectangle) {3*g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
            g->maps[2].color = GRAY;
            g->maps[2].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[2].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[2].num_enemies = 2;
            g->maps[2].door_locked = 0;

            for(int i=0; i< g->maps[2].num_enemies; i++){
                g->maps[2].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[2].enemies[i].color = BLACK;
                g->maps[2].enemies[i].speed = 2;
                g->maps[2].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[2].enemies[i].draw_enemy = 1;
                g->maps[2].enemies[i].has_key = 0;
                g->maps[2].enemies[i].max_projectiles = 2;
            }
            g->maps[2].enemies[0].has_key = 1;
            g->maps[2].prev_map = 1;
            g->maps[2].next_map = 3;
            break;
        case 3:
             // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[3].num_barriers = 3;
            g->maps[3].barriers[0] = (Rectangle) {g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
            g->maps[3].barriers[1] = (Rectangle) {3*g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
            g->maps[3].color = GRAY;
            g->maps[3].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[3].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[3].num_enemies = 3;
            g->maps[3].door_locked = 0;

            for(int i=0; i< g->maps[3].num_enemies; i++){
                g->maps[3].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[3].enemies[i].color = BLACK;
                g->maps[3].enemies[i].speed = 4;
                g->maps[3].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[3].enemies[i].draw_enemy = 1;
                g->maps[3].enemies[i].has_key = 0;
            }
            g->maps[3].enemies[0].has_key = 1;
            g->maps[3].prev_map = 2;
            g->maps[3].next_map = 4;
            break;
        case 4:
             // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[4].num_barriers = 3;
            g->maps[4].barriers[0] = (Rectangle) {g->screenWidth/4, 0, 8, 0.6 * g->screenHeight};
            g->maps[4].barriers[1] = (Rectangle) {2*g->screenWidth/4, 200, 8, 0.6 * g->screenHeight};
            g->maps[4].barriers[2] = (Rectangle) {3*g->screenWidth/4, 0, 8, 0.6 * g->screenHeight};
            g->maps[4].color = GRAY;
            g->maps[4].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[4].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[4].num_enemies = 3;
            g->maps[4].door_locked = 0;

            for(int i=0; i< g->maps[4].num_enemies; i++){
                g->maps[4].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[4].enemies[i].color = BLACK;
                g->maps[4].enemies[i].speed = 3;
                g->maps[4].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[4].enemies[i].draw_enemy = 1;
                g->maps[4].enemies[i].has_key = 0;
            }
            g->maps[4].enemies[0].has_key = 1;
            g->maps[4].prev_map = 3;
            g->maps[4].next_map = 5;
            break;
        case 5:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[5].num_barriers = 3;
            g->maps[5].barriers[0] = (Rectangle) {g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
            g->maps[5].barriers[1] = (Rectangle) {2*g->screenWidth/8, 225, 400, 0.025 * g->screenHeight};
            g->maps[5].barriers[2] = (Rectangle) {3*g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
            g->maps[5].color = GRAY;
            g->maps[5].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[5].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[5].num_enemies = 3;
            g->maps[5].door_locked = 0;

            for(int i=0; i< g->maps[5].num_enemies; i++){
                g->maps[5].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[5].enemies[i].color = BLACK;
                g->maps[5].enemies[i].speed = 3;
                g->maps[5].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[5].enemies[i].draw_enemy = 1;
                g->maps[5].enemies[i].has_key = 0;
            }
            g->maps[5].enemies[0].has_key = 1;
            g->maps[5].prev_map = 4;
            g->maps[5].next_map = 6;
            break;
        case 6:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }
            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[6].num_barriers = 4;
            g->maps[6].barriers[0] = (Rectangle) {g->screenWidth/4, 0, 8, 0.35 * g->screenHeight};
            g->maps[6].barriers[1] = (Rectangle) {g->screenWidth/4, 275, 8, 0.5 * g->screenHeight};
            g->maps[6].barriers[2] = (Rectangle) {3*g->screenWidth/4, 0, 8, 0.35 * g->screenHeight};
            g->maps[6].barriers[3] = (Rectangle) {3*g->screenWidth/4, 275, 8, 0.5 * g->screenHeight};
            g->maps[6].color = GRAY;
            g->maps[6].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[6].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
            g->maps[6].num_enemies = 4;
            g->maps[6].door_locked = 0;

            for(int i=0; i< g->maps[6].num_enemies; i++){
                g->maps[6].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[6].enemies[i].color = BLACK;
                g->maps[6].enemies[i].speed = 4;
                g->maps[6].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[6].enemies[i].draw_enemy = 1;
                g->maps[6].enemies[i].has_key = 0;
            }
            g->maps[6].enemies[0].has_key = 1;
            g->maps[6].prev_map = 5;
            g->maps[6].next_map = 7;
            break;
        case 7:
            // resetando os tiros
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->hero.projectiles[i]);
                h->active = 0;  // Define o tiro do herói como morto
                for (int j = 0; j < g->maps->num_enemies; j++) {
                    Projectile *e = &(g->maps->enemies[j].projectiles[i]);
                    e->active = 0;  // Define o tiro do inimigo como morto
                }
            }

            //resetando os tiros do boss
            for (int i = 0; i < MAX_PROJECTILES; i++) {
                Projectile *h = &(g->maps[7].boss.projectiles[i]);
                h->active = 0;  // Define o tiro do boss como morto
            }

            g->hero.pos = (Rectangle) { 150, 300, STD_SIZE_X, STD_SIZE_Y};
            g->maps[7].num_barriers = 8;
            g->maps[7].barriers[0] = (Rectangle) {3*g->screenWidth/7, 165, 100, 0.025 * g->screenHeight};
            g->maps[7].barriers[1] = (Rectangle) {3*g->screenWidth/7, 265, 100, 0.025 * g->screenHeight};
            g->maps[7].barriers[2] = (Rectangle) {3*g->screenWidth/9.5, 90, 300, 0.025 * g->screenHeight};
            g->maps[7].barriers[3] = (Rectangle) {3*g->screenWidth/9.5, 355, 300, 0.025 * g->screenHeight};
            g->maps[7].barriers[4] = (Rectangle) {3*g->screenWidth/9.5, 90, 10, 0.57 * g->screenHeight};
            g->maps[7].barriers[5] = (Rectangle) {3*g->screenWidth/4.35, 90, 10, 0.19 * g->screenHeight};
            g->maps[7].barriers[6] = (Rectangle) {3*g->screenWidth/4.35, 290, 10, 0.163 * g->screenHeight};
            g->maps[7].barriers[7] = (Rectangle) {3*g->screenWidth/5.5, 165, 10, 0.234 * g->screenHeight};
            g->maps[7].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
            g->maps[7].num_enemies = 0;
            g->maps[7].door_locked = 0;

            for(int i=0; i< g->maps[7].num_enemies; i++){
                g->maps[7].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
                g->maps[7].enemies[i].color = BLACK;
                g->maps[7].enemies[i].speed = 4;
                g->maps[7].enemies[i].direction = KEY_RIGHT + (rand() % 4);
                g->maps[7].enemies[i].draw_enemy = 1;
                g->maps[7].enemies[i].has_key = 0;
            }

            g->maps[7].boss_alive = true;
            if(g->maps[7].boss_alive){
                g->maps[7].boss.pos = (Rectangle) { 600, 250, STD_SIZE_X + 20, STD_SIZE_Y + 20};
                g->maps[7].boss.color = ORANGE;
                g->maps[7].boss.speed = 5;
                g->maps[7].boss.max_projectiles = 10;
                g->maps[7].boss.health = 10;
            }

            g->maps[7].enemies[0].has_key = 1;
            g->maps[7].special_item = (Rectangle) {4*g->screenWidth/5, 20, 15, 15};
            g->maps[7].draw_special_item = 1;
            g->maps[7].next_map = 9;
            break;
    }
}

// Maps Setup
void map0_setup(Game *g){
    g->maps[0].background = LoadTexture("backgrounds/mp0.png");
    g->maps[0].num_barriers = 4;
    g->maps[0].barriers[0] = (Rectangle) {g->screenWidth/3, 100, 10, 0.6 * g->screenHeight};
    g->maps[0].barriers[1] = (Rectangle) {2*g->screenWidth/30, 200, 220, 0.025 * g->screenHeight};
    g->maps[0].color = GRAY;
    g->maps[0].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[0].num_enemies = 2;
    g->maps[0].door_locked = 0;
    

    for(int i=0; i< g->maps[0].num_enemies; i++){
        g->maps[0].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[0].enemies[i].color = BLACK;
        g->maps[0].enemies[i].speed = 3;
        g->maps[0].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[0].enemies[i].draw_enemy = 1;
        g->maps[0].enemies[i].has_key = 1;
    }

    g->maps[0].enemies[0].has_key = 1;
    g->maps[0].prev_map = -1;
    g->maps[0].next_map = 1;
    

}

void map1_setup(Game *g){
    g->maps[1].background = LoadTexture("backgrounds/mp1.png");
    g->maps[1].num_barriers = 2;
    g->maps[1].barriers[0] = (Rectangle) {g->screenWidth/5, 125, 500, 0.025 * g->screenHeight};
    g->maps[1].barriers[1] = (Rectangle) {g->screenWidth/5, 400, 500, 0.025 * g->screenHeight};
    g->maps[1].color = GRAY;
    g->maps[1].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[1].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[1].num_enemies = 2;
    g->maps[1].door_locked = 1;

    for(int i=0; i< g->maps[1].num_enemies; i++){
        g->maps[1].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3 - 50, STD_SIZE_X, STD_SIZE_Y};
        g->maps[1].enemies[i].color = GRAY;
        g->maps[1].enemies[i].speed = 7;
        g->maps[1].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[1].enemies[i].draw_enemy = 1;
        g->maps[1].enemies[i].has_key = 0;
        g->maps[1].enemies[i].max_projectiles = 2;
    }

    g->maps[1].enemies[0].has_key = 1;
    g->maps[1].prev_map = 0;
    g->maps[1].next_map = 2;
}

void map2_setup(Game *g){
    g->maps[2].background = LoadTexture("backgrounds/mp2.png");
    g->maps[2].num_barriers = 3;
    g->maps[2].barriers[0] = (Rectangle) {g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
    g->maps[2].barriers[1] = (Rectangle) {2*g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
    g->maps[2].barriers[2] = (Rectangle) {3*g->screenWidth/4, 125, 8, 0.5 * g->screenHeight};
    g->maps[2].color = GRAY;
    g->maps[2].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[2].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[2].num_enemies = 2;
    g->maps[2].door_locked = 0;

    for(int i=0; i< g->maps[2].num_enemies; i++){
        g->maps[2].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[2].enemies[i].color = BLACK;
        g->maps[2].enemies[i].speed = 2;
        g->maps[2].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[2].enemies[i].draw_enemy = 1;
        g->maps[2].enemies[i].has_key = 0;
        g->maps[2].enemies[i].max_projectiles = 2;
    }

    g->maps[2].enemies[0].has_key = 1;
    g->maps[2].prev_map = 1;
    g->maps[2].next_map = 3;
}

void map3_setup(Game *g){
    g->maps[3].background = LoadTexture("backgrounds/mp3.png");
    g->maps[3].num_barriers = 2;
    g->maps[3].barriers[0] = (Rectangle) {g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
    g->maps[3].barriers[1] = (Rectangle) {3*g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
    g->maps[3].color = GRAY;
    g->maps[3].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[3].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[3].num_enemies = 3;
    g->maps[3].door_locked = 0;

    for(int i=0; i < g->maps[3].num_enemies; i++){
        g->maps[3].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[3].enemies[i].color = BLACK;
        g->maps[3].enemies[i].speed = 4;
        g->maps[3].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[3].enemies[i].draw_enemy = 1;
        g->maps[3].enemies[i].has_key = 0;
        g->maps[3].enemies[i].max_projectiles = 2;
    }

    g->maps[3].enemies[0].has_key = 1;
    g->maps[3].prev_map = 2;
    g->maps[3].next_map = 4;
}

void map4_setup(Game *g){
    g->maps[4].background = LoadTexture("backgrounds/mp4.png");
    g->maps[4].num_barriers = 3;
    g->maps[4].barriers[0] = (Rectangle) {g->screenWidth/4, 0, 8, 0.6 * g->screenHeight};
    g->maps[4].barriers[1] = (Rectangle) {2*g->screenWidth/4, 200, 8, 0.6 * g->screenHeight};
    g->maps[4].barriers[2] = (Rectangle) {3*g->screenWidth/4, 0, 8, 0.6 * g->screenHeight};
    g->maps[4].color = GRAY;
    g->maps[4].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[4].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[4].num_enemies = 3;
    g->maps[4].door_locked = 0;

    for(int i=0; i< g->maps[4].num_enemies; i++){
        g->maps[4].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[4].enemies[i].color = BLACK;
        g->maps[4].enemies[i].speed = 4;
        g->maps[4].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[4].enemies[i].draw_enemy = 1;
        g->maps[4].enemies[i].has_key = 0;
        g->maps[4].enemies[i].max_projectiles = 2;
    }

    g->maps[4].enemies[0].has_key = 1;
    g->maps[4].prev_map = 3;
    g->maps[4].next_map = 5;
}

void map5_setup(Game *g){
    g->maps[5].background = LoadTexture("backgrounds/mp5.png");
    g->maps[5].num_barriers = 3;
    g->maps[5].barriers[0] = (Rectangle) {g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
    g->maps[5].barriers[1] = (Rectangle) {2*g->screenWidth/8, 225, 400, 0.025 * g->screenHeight};
    g->maps[5].barriers[2] = (Rectangle) {3*g->screenWidth/4, 100, 8, 0.6 * g->screenHeight};
    g->maps[5].color = GRAY;
    g->maps[5].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[5].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[5].num_enemies = 3;
    g->maps[5].door_locked = 0;

    for(int i=0; i< g->maps[5].num_enemies; i++){
        g->maps[5].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[5].enemies[i].color = BLACK;
        g->maps[5].enemies[i].speed = 3;
        g->maps[5].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[5].enemies[i].draw_enemy = 1;
        g->maps[5].enemies[i].has_key = 0;
        g->maps[5].enemies[i].max_projectiles = 2;
    }

    g->maps[5].enemies[0].has_key = 1;
    g->maps[5].prev_map = 4;
    g->maps[5].next_map = 6;
}

void map6_setup(Game *g){
    g->maps[6].background = LoadTexture("backgrounds/mp5.png");
    g->maps[6].num_barriers = 4;
    g->maps[6].barriers[0] = (Rectangle) {g->screenWidth/4, 0, 8, 0.35 * g->screenHeight};
    g->maps[6].barriers[1] = (Rectangle) {g->screenWidth/4, 275, 8, 0.5 * g->screenHeight};
    g->maps[6].barriers[2] = (Rectangle) {3*g->screenWidth/4, 0, 8, 0.35 * g->screenHeight};
    g->maps[6].barriers[3] = (Rectangle) {3*g->screenWidth/4, 275, 8, 0.5 * g->screenHeight};
    g->maps[6].color = GRAY;
    g->maps[6].door = (Rectangle) {g->screenWidth-(SCREEN_BORDER+5), g->screenHeight/3, SCREEN_BORDER, 50};
    g->maps[6].prev_door = (Rectangle) {SCREEN_BORDER, g->screenHeight/3, 5, 50};
    g->maps[6].num_enemies = 4;
    g->maps[6].door_locked = 0;

    for(int i=0; i< g->maps[6].num_enemies; i++){
        g->maps[6].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[6].enemies[i].color = BLACK;
        g->maps[6].enemies[i].speed = 4;
        g->maps[6].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[6].enemies[i].draw_enemy = 1;
        g->maps[6].enemies[i].has_key = 0;
        g->maps[6].enemies[i].max_projectiles = 2;
    }

    g->maps[6].enemies[0].has_key = 1;
    g->maps[6].prev_map = 5;
    g->maps[6].next_map = 7;
}


void map7_setup(Game *g){
    g->maps[7].background = LoadTexture("backgrounds/mp5.png");
    g->maps[7].num_barriers = 8;
    g->maps[7].barriers[0] = (Rectangle) {3*g->screenWidth/7, 165, 100, 0.025 * g->screenHeight};
    g->maps[7].barriers[1] = (Rectangle) {3*g->screenWidth/7, 265, 100, 0.025 * g->screenHeight};
    g->maps[7].barriers[2] = (Rectangle) {3*g->screenWidth/9.5, 90, 300, 0.025 * g->screenHeight};
    g->maps[7].barriers[3] = (Rectangle) {3*g->screenWidth/9.5, 355, 300, 0.025 * g->screenHeight};
    g->maps[7].barriers[4] = (Rectangle) {3*g->screenWidth/9.5, 90, 10, 0.57 * g->screenHeight};
    g->maps[7].barriers[5] = (Rectangle) {3*g->screenWidth/4.35, 90, 10, 0.19 * g->screenHeight};
    g->maps[7].barriers[6] = (Rectangle) {3*g->screenWidth/4.35, 290, 10, 0.163 * g->screenHeight};
    g->maps[7].barriers[7] = (Rectangle) {3*g->screenWidth/5.5, 165, 10, 0.234 * g->screenHeight};
    g->maps[7].num_enemies = 0;

    for(int i=0; i< g->maps[7].num_enemies; i++){
        g->maps[7].enemies[i].pos = (Rectangle) { 2*g->screenWidth/3, 2*g->screenHeight/3, STD_SIZE_X, STD_SIZE_Y};
        g->maps[7].enemies[i].color = BLACK;
        g->maps[7].enemies[i].speed = 4;
        g->maps[7].enemies[i].direction = KEY_RIGHT + (rand() % 4);
        g->maps[7].enemies[i].draw_enemy = 1;
        g->maps[7].enemies[i].has_key = 0;
        g->maps[7].enemies[i].max_projectiles = 2;
    }

    g->maps[7].boss_alive = true;
    

    if(g->maps[7].boss_alive){
        g->maps[7].boss.pos = (Rectangle) { 600, 250, STD_SIZE_X + 20, STD_SIZE_Y + 20};
        g->maps[7].boss.tex_boss = LoadTexture("sprites/boss.png");
        g->maps[7].boss.color = (Color) {255, 255, 255, 0};
        g->maps[7].boss.speed = 5;
        g->maps[7].boss.max_projectiles = 10;
        g->maps[7].boss.health = 10;
    }
    
    g->maps[7].enemies[0].has_key = 1;
    //g->maps[7].special_item = (Rectangle) {4*g->screenWidth/5, 20, 15, 15};
    //g->maps[7].draw_special_item = 1;
}