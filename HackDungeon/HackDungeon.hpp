#ifndef HACKDUNGEON_HPP
#define HACKDUNGEON_HPP

#define PLAYER_HEALTH 10
#define PLAYER_SEERADIUS 3

#define ITEMTYPE_FOOD 0
#define ITEMTYPE_SWORD 1
#define ITEMTYPE_KEY 2

#define CELLTYPE_EMPTY 0
#define CELLTYPE_WALL 1
#define CELLTYPE_DOOR 2

#define ENEMYTYPE_SPIDER 0
#define ENEMYTYPE_SNAKE 1
#define ENEMYTYPE_BAT 2

#define GAMESTATE_PLAYING 0
#define GAMESTATE_DEAD 1
#define GAMESTATE_ESCAPED 2

#define SPIDER_RADIUS 3
#define SPIDER_MINHEALTH 2
#define SPIDER_MAXHEALTH 5
#define SPIDER_MINDAMAGE 1
#define SPIDER_MAXDAMAGE 2

#define BAT_RADIUS 6
#define BAT_MINHEALTH 5
#define BAT_MAXHEALTH 10
#define BAT_MINDAMAGE 2
#define BAT_MAXDAMAGE 5

#define SNAKE_RADIUS 5
#define SNAKE_MINHEALTH 10
#define SNAKE_MAXHEALTH 20
#define SNAKE_MINDAMAGE 5
#define SNAKE_MAXDAMAGE 8

#define WEAPONFISTS_MINDAMAGE 1
#define WEAPONFISTS_MAXDAMAGE 2

#define WEAPONSWORD_MINDAMAGE 4
#define WEAPONSWORD_MAXDAMAGE 10

#define MAP_WIDTH 32
#define MAP_HEIGHT 16

#define PLAYER_CHAR '@'
#define ENEMY_CHARS "*~^"
#define ITEM_CHARS ".+P"
#define CELL_CHARS " #]"

#define MAX_ITEMS 10
#define MAX_ENEMIES 10

#define FOOD_HEALTHUP 10.0f

struct Map
{
    unsigned int width;
    unsigned int height;
    unsigned int cells[MAP_HEIGHT * MAP_WIDTH];
};

struct Position
{
    int x;
    int y;
};

struct Stats
{
    float health;
    float magic;
    float strength;
};

struct Inventory
{
    unsigned int numItems;
    unsigned char items[10];
};

struct Player
{
    Position position;
    Stats stats;
    Inventory inventory;
};

struct Enemy
{
    unsigned int type;
    Position position;
    float health;
};

struct Item
{
    unsigned char type;
    Position position;
    unsigned char collected;
};


#endif