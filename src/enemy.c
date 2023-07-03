#include <raylib.h>
#include "jogo.h"
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void EnemyShootProjectile(Enemy *enemy, Vector2 speed){
    int active_projectiles = 0;
    for(int i = 0; i < enemy->max_projectiles; i++){
        if(enemy->projectiles[i].active){
            active_projectiles++;
        }
    }

    if (active_projectiles < enemy->max_projectiles) {
        
        for(int i = 0; i < enemy->max_projectiles; i++){
            if(!enemy->projectiles[i].active){
                enemy->projectiles[i].pos = (Vector2){enemy->pos.x + enemy->pos.width / 2, enemy->pos.y + enemy->pos.height / 2};
                enemy->projectiles[i].speed = speed;
                enemy->projectiles[i].active = true;
                break;
            }
        }
    }
}

void BossShootProjectile(Boss *b, Vector2 speed){
    int active_projectiles = 0;
    for(int i = 0; i < b->max_projectiles; i++){
        if(b->projectiles[i].active){
            active_projectiles++;
        }
    }

    if (active_projectiles < b->max_projectiles) {
        
        for(int i = 0; i < b->max_projectiles; i++){
            if(!b->projectiles[i].active){
                b->projectiles[i].pos = (Vector2){b->pos.x + b->pos.width / 2, b->pos.y + b->pos.height / 2};
                b->projectiles[i].speed = speed;
                b->projectiles[i].active = true;
                break;
            }
        }
    }
}

void update_enemy_pos(Game *g, Enemy *e, int current_level)
{
    current_level += 1;
    Map *m = &g->maps[g->curr_map];

    if(current_level == 1 || current_level == 4 || current_level == 5) {
        // Movimento normal e aleatório
        if(e->direction == KEY_LEFT) {
            if(e->pos.x > SCREEN_BORDER)
                e->pos.x -= e->speed;
            else{
                e->direction = KEY_RIGHT + (rand() % 4);
            }
            if(barrier_collision(m, &e->pos)){
                e->pos.x += e->speed;
                e->direction = KEY_RIGHT + (rand() % 4);
            }

        } else if(e->direction == KEY_RIGHT) {
            if(e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
                e->pos.x += e->speed;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.x -= e->speed;
                e->direction = KEY_RIGHT + (rand() % 4);
            }

        } else if(e->direction == KEY_UP) {
            if(e->pos.y > SCREEN_BORDER)
                e->pos.y -= e->speed;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.y += e->speed;
                e->direction = KEY_RIGHT + (rand() % 4);
            }
        } else if(e->direction == KEY_DOWN) {
            if(e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
                e->pos.y += e->speed;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.y -= e->speed;
                e->direction = KEY_RIGHT + (rand() % 4);
            }
        }
        if(rand() % 100 == 1) // 1% de chance do inimigo mudar de direcao
            e->direction = KEY_RIGHT + (rand() % 4);
    } else if(current_level == 2 || current_level == 7) {
        // Movimento lento e aleatório com probabilidade de mover em direção ao herói
        int move_towards_hero = rand() % 100; // Gera um número aleatório entre 0 e 99
        if (move_towards_hero < 1) { // 50% de chance de mover em direção ao herói
            if (e->pos.x < g->hero.pos.x) {
                e->pos.x += e->speed / 2; // Move lentamente para a direita
                if (barrier_collision(m, &e->pos)) {
                    e->pos.x -= e->speed / 2;
                }
            } else if (e->pos.x > g->hero.pos.x) {
                e->pos.x -= e->speed / 2; // Move lentamente para a esquerda
                if (barrier_collision(m, &e->pos)) {
                    e->pos.x += e->speed / 2;
                }
            }
            if (e->pos.y < g->hero.pos.y) {
                e->pos.y += e->speed / 2; // Move lentamente para baixo
                if (barrier_collision(m, &e->pos)) {
                    e->pos.y -= e->speed / 2;
                }
            } else if (e->pos.y > g->hero.pos.y) {
                e->pos.y -= e->speed / 2; // Move lentamente para cima
                if (barrier_collision(m, &e->pos)) {
                    e->pos.y += e->speed / 2;
                }
            }
        } else {
            // Movimento aleatório lento
            if(e->direction == KEY_LEFT) {
            if(e->pos.x > SCREEN_BORDER)
                e->pos.x -= e->speed / 2;
            else{
                e->direction = KEY_RIGHT + (rand() % 4);
            }
            if(barrier_collision(m, &e->pos)){
                e->pos.x += e->speed / 2;
                e->direction = KEY_RIGHT + (rand() % 4);
            }

            } else if(e->direction == KEY_RIGHT) {
                if(e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
                    e->pos.x += e->speed / 2;
                else
                    e->direction = KEY_RIGHT + (rand() % 4);

                if(barrier_collision(m, &e->pos)){
                    e->pos.x -= e->speed / 2;
                    e->direction = KEY_RIGHT + (rand() % 4);
                }

            } else if(e->direction == KEY_UP) {
                if(e->pos.y > SCREEN_BORDER)
                    e->pos.y -= e->speed / 2;
                else
                    e->direction = KEY_RIGHT + (rand() % 4);

                if(barrier_collision(m, &e->pos)){
                    e->pos.y += e->speed / 2;
                    e->direction = KEY_RIGHT + (rand() % 4);
                }
            } else if(e->direction == KEY_DOWN) {
                if(e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
                    e->pos.y += e->speed / 2;
                else
                    e->direction = KEY_RIGHT + (rand() % 4);

                if(barrier_collision(m, &e->pos)){
                    e->pos.y -= e->speed / 2;
                    e->direction = KEY_RIGHT + (rand() % 4);
                }
            }
            if(rand() % 100 == 1) // 1% de chance do inimigo mudar de direcao
                e->direction = KEY_RIGHT + (rand() % 4);
        }
    } else if(current_level == 3 || current_level == 6) {
        // Movimento rápido e normal
        if(e->direction == KEY_LEFT) {
            if(e->pos.x > SCREEN_BORDER)
                e->pos.x -= e->speed * 2;
            else{
                e->direction = KEY_RIGHT + (rand() % 4);
            }
            if(barrier_collision(m, &e->pos)){
                e->pos.x += e->speed * 2;
                e->direction = KEY_RIGHT + (rand() % 4);
            }

        } else if(e->direction == KEY_RIGHT) {
            if(e->pos.x + e->pos.width < g->screenWidth - SCREEN_BORDER)
                e->pos.x += e->speed * 2;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.x -= e->speed * 2;
                e->direction = KEY_RIGHT + (rand() % 4);
            }

        } else if(e->direction == KEY_UP) {
            if(e->pos.y > SCREEN_BORDER)
                e->pos.y -= e->speed * 2;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.y += e->speed * 2;
                e->direction = KEY_RIGHT + (rand() % 4);
            }
        } else if(e->direction == KEY_DOWN) {
            if(e->pos.y + e->pos.height < g->screenHeight - SCREEN_BORDER)
                e->pos.y += e->speed * 2;
            else
                e->direction = KEY_RIGHT + (rand() % 4);

            if(barrier_collision(m, &e->pos)){
                e->pos.y -= e->speed * 2;
                e->direction = KEY_RIGHT + (rand() % 4);
            }
        }
        if(rand() % 100 == 10) // 10% de chance do inimigo mudar de direcao
            e->direction = KEY_RIGHT + (rand() % 4);
    } 
}

void update_boss_pos(Boss *boss,Game *g){

    Map *m = &g->maps[g->curr_map];
    // movimento do boss e colições com as barreiras
    if(boss->direction == KEY_LEFT) {
        if(boss->pos.x > SCREEN_BORDER)
            boss->pos.x -= boss->speed;
        else{
            boss->direction = KEY_RIGHT + (rand() % 4);
        }
        if(barrier_collision(m, &boss->pos)){
            boss->pos.x += boss->speed;
            boss->direction = KEY_RIGHT + (rand() % 4);
        }

    } else if(boss->direction == KEY_RIGHT) {
        if(boss->pos.x + boss->pos.width < g->screenWidth - SCREEN_BORDER)
            boss->pos.x += boss->speed;
        else
            boss->direction = KEY_RIGHT + (rand() % 4);

        if(barrier_collision(m, &boss->pos)){
            boss->pos.x -= boss->speed;
            boss->direction = KEY_RIGHT + (rand() % 4);
        }

    } else if(boss->direction == KEY_UP) {
        if(boss->pos.y > SCREEN_BORDER)
            boss->pos.y -= boss->speed;
        else
            boss->direction = KEY_RIGHT + (rand() % 4);

        if(barrier_collision(m, &boss->pos)){
            boss->pos.y += boss->speed;
            boss->direction = KEY_RIGHT + (rand() % 4);
        }
    } else if(boss->direction == KEY_DOWN) {
        if(boss->pos.y + boss->pos.height < g->screenHeight - SCREEN_BORDER)
            boss->pos.y += boss->speed;
        else
            boss->direction = KEY_RIGHT + (rand() % 4);

        if(barrier_collision(m, &boss->pos)){
            boss->pos.y -= boss->speed;
            boss->direction = KEY_RIGHT + (rand() % 4);
        }
    }
    // chance do boss mudar de direção
    if(rand() % 100 == 50) // 50% de chance do boss mudar de direcao
        boss->direction = KEY_RIGHT + (rand() % 4);
}

void DrawBossHealthBar(Boss *boss) {
    // Defina as coordenadas e dimensões da barra de vida
    float barX = 600;
    float barY = 20;
    float barWidth = 15 * boss->health;
    float barHeight = 10;

    // Calcule a porcentagem de vida restante do boss
    float healthPercentage = (float)boss->health / 10;

    // Defina a cor da barra de vida com base na porcentagem de vida restante
    Color healthColor;
    if (healthPercentage > 0.7) {
        healthColor = GREEN;  // Verde para mais de 70% de vida
    } else if (healthPercentage > 0.3) {
        healthColor = YELLOW; // Amarelo para mais de 30% de vida
    } else {
        healthColor = RED;    // Vermelho para menos de 30% de vida
    }

    // Desenhe a barra de vida
    DrawRectangle(barX, barY, barWidth, barHeight, BLACK);           // Fundo preto
    DrawRectangle(barX, barY, barWidth * healthPercentage, barHeight, healthColor); // Barra de vida

    // Desenhe o texto da vida acima da barra de vida
    DrawText(TextFormat("Boss HP: %d/%d", boss->health, 10), barX, barY - 20, 20, WHITE);
}