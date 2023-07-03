#ifndef JOGO_H_INCLUDED
#define JOGO_H_INCLUDED

#include <raylib.h>

#define STD_SIZE_X 30
#define STD_SIZE_Y 30
#define SCREEN_BORDER 4
#define NUM_SHOOTS 50
#define MAX_PROJECTILES 50
#define MAX_SCORES 3


typedef enum { NORMAL, HARD } Difficulty;

typedef struct Projectile {
    Vector2 pos;
    Vector2 speed;
    bool active;
    Color color;
} Projectile;

typedef struct Score{
    char name[20];
    double score;
} Score;

typedef struct Hero {
    Rectangle pos;
    Color color;
    int speed;
    int special;
    Projectile projectiles[MAX_PROJECTILES];
    int max_projectiles; // novo campo
    Texture2D tex_hero;
} Hero;

typedef struct Enemy {
    Rectangle pos;
    Color color;
    int speed;
    int direction;
    int draw_enemy;
    int has_key;
    Projectile projectiles[MAX_PROJECTILES];
    int max_projectiles; // novo campo
    int cooldown;
    Texture2D tex;
} Enemy;

typedef struct Boss {
    Rectangle pos;
    Color color;
    int speed;
    int cooldown;
    int direction;
    int health;
    int draw_enemy;
    Projectile projectiles[MAX_PROJECTILES];
    int max_projectiles;
    Texture2D tex_boss;
} Boss;

typedef struct Map {
    Texture2D background;
    Rectangle barriers[40];
    int num_barriers;
    Rectangle door;
    Rectangle prev_door;
    Color color;
    Enemy enemies[10];
    Boss boss;
    bool boss_alive;
    int num_enemies;
    Rectangle special_item;
    int draw_special_item;
    int door_locked;
    bool door_open;
    int next_map;
    int prev_map;
} Map;

typedef struct Game { 
    Map maps[10];
    bool reset_required;
    int num_maps;
    int curr_map;
    Hero hero;
    int screenWidth;
    int screenHeight;
    int gameover;
    bool boss_dead;
} Game;



/* FUNÇÕES REFERENTE AO HEROI */
void update_hero_pos(Game *g);
void ShootProjectile(Hero *hero, Vector2 speed);

/* FUNÇÕES REFERENTE AOS INIMIGOS */
void DrawBossHealthBar(Boss *b);
void update_enemy_pos(Game *g, Enemy *e, int current_level);
void update_boss_pos(Boss *b,Game *g);
void BossShootProjectile(Boss *b, Vector2 speed);
void EnemyShootProjectile(Enemy *enemy, Vector2 speed);

/* FUNÇÕES REFERENTE AO JOGO*/
void dificuldade(Difficulty dificullty, Hero *hero, Map *maps, int num_maps);
void InitGame(Game *g, Difficulty difficulty);         // Initialize game
void UpdateGame(Game *g);       // Update game (one frame)
void DrawGame(Game *g);         // Draw game (one frame)
void UpdateDrawFrame(Game *g);  // Update and Draw (one frame)
void apelido();
void readScores(Score pontuacoes[MAX_SCORES]);
void sortScores(Score pontuacoes[MAX_SCORES]);
void updateScores(Score pontuacoes[MAX_SCORES], char *nome, double time);
void writeScores(Score pontuacoes[MAX_SCORES]);
double getTime();
Difficulty setDifficulty();


/* FUNÇÕES REFERENTE AOS MAPAS*/
void draw_map(Game *g);
void draw_borders(Game *g);
int barrier_collision(Map *m, Rectangle *t);

void map0_setup(Game *g);
void map1_setup(Game *g);
void map2_setup(Game *g);
void map3_setup(Game *g);
void map4_setup(Game *g);
void map5_setup(Game *g);
void map6_setup(Game *g);
void map7_setup(Game *g);
void ResetMap(Game *g, int curr_map);




#endif //JOGO_H_INCLUDE