#include <raylib.h>
#include "jogo.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>


//atirar 
void ShootProjectile(Hero *hero, Vector2 speed) { // verifica se o número de projeteis ativos é menor que o valor de max_projectiles antes de permiter que um novo projétil seja disparado.
    int active_projectiles = 0;

    InitAudioDevice();
    Sound tiro = LoadSound("sounds/tiro.wav");
    PlaySound(tiro);
    
    for (int i = 0; i < hero->max_projectiles; i++) {
        if (hero->projectiles[i].active) {
            active_projectiles++;
        }
    }

    if (active_projectiles < hero->max_projectiles) {
        for (int i = 0; i < hero->max_projectiles; i++) {
            if (!hero->projectiles[i].active) {
                hero->projectiles[i].pos = (Vector2){hero->pos.x + hero->pos.width / 2, hero->pos.y + hero->pos.height / 2};
                hero->projectiles[i].speed = speed;
                hero->projectiles[i].active = true;
                break;
            }
        }
    }

    StopSound(tiro);
}

void update_hero_pos(Game *g){
    
    Hero *h = &g->hero;
    Map *m = &g->maps[g->curr_map];

    if(IsKeyDown(KEY_A) || IsKeyDown(KEY_LEFT)) {
        g->hero.tex_hero = LoadTexture("sprites/hero_left.png");
        if (IsKeyPressed(KEY_SPACE)) {
        // Define a velocidade do projétil (pode ser ajustada conforme necessário)
        Vector2 projectileSpeed = {-9, 0}; // Velocidade horizontal
        ShootProjectile(&g->hero, projectileSpeed);
    }
        if(h->pos.x > SCREEN_BORDER)
            h->pos.x -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x += h->speed;

    } else if(IsKeyDown(KEY_D) || IsKeyDown(KEY_RIGHT)) {
        g->hero.tex_hero = LoadTexture("sprites/hero.png");
        if (IsKeyPressed(KEY_SPACE)) {
        // Define a velocidade do projétil (pode ser ajustada conforme necessário)
        Vector2 projectileSpeed = {9, 0}; // Velocidade horizontal
        ShootProjectile(&g->hero, projectileSpeed);
    }
        if(h->pos.x + h->pos.width < g->screenWidth - SCREEN_BORDER)
            h->pos.x += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.x -= h->speed;

    } else if(IsKeyDown(KEY_W) || IsKeyDown(KEY_UP)) {
        if (IsKeyPressed(KEY_SPACE)) {
        // Define a velocidade do projétil (pode ser ajustada conforme necessário)
        Vector2 projectileSpeed = {0, -9}; // Velocidade horizontal
        ShootProjectile(&g->hero, projectileSpeed);
    }
        if(h->pos.y > SCREEN_BORDER)
            h->pos.y -= h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y += h->speed;

    } else if(IsKeyDown(KEY_S) || IsKeyDown(KEY_DOWN)) {
        if (IsKeyPressed(KEY_SPACE)) {
        // Define a velocidade do projétil (pode ser ajustada conforme necessário)
        Vector2 projectileSpeed = {0, 9}; // Velocidade horizontal
        ShootProjectile(&g->hero, projectileSpeed);
    }
        if(h->pos.y + h->pos.height < g->screenHeight - SCREEN_BORDER)
            h->pos.y += h->speed;
        if(barrier_collision(m, &h->pos)) h->pos.y -= h->speed;
    }

    
}