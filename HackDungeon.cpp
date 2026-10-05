#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ITEMTYPE_FOOD 0
#define ITEMTYPE_SWORD 1
#define ITEMTYPE_KEY 2

#define CELLTYPE_EMPTY 0
#define CELLTYPE_WALL 1
#define CELLTYPE_DOOR 2

#define GAMESTATE_PLAYING 0
#define GAMESTATE_DEAD 1
#define GAMESTATE_ESCAPED 2

#define ENEMYTYPE_SPIDER 0
#define ENEMYTYPE_SNAKE 1
#define ENEMYTYPE_BAT 2

#define SPIDER_RADIUS 5
#define SPIDER_MINHEALTH 5
#define SPIDER_MAXHEALTH 10
#define SPIDER_MINDAMAGE 0
#define SPIDER_MAXDAMAGE 2


#define WEAPONFISTS_MINDAMAGE 1
#define WEAPONFISTS_MAXDAMAGE 2

#define WEAPONSWORD_MINDAMAGE 3
#define WEAPONSWORD_MAXDAMAGE 6


struct Map
{
    unsigned int width;
    unsigned int height;
    unsigned int cells[16 * 32];
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

char name[100] = "Player";
Player player;
Map map;


unsigned int numEnemies;
Enemy enemies[10];

unsigned int numItems;
Item items[10];

char playerCharacter = '@';
char enemyCharacter[10] = "*~^";
char itemCharacter[10] = ".+P";
char mapCellCharacters[10] = " #]";

unsigned int screenWidth = 32;
unsigned int screenHeight = 16;
char screen[16 * 32];

int gameState;

unsigned char equalPos(Position p1, Position p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

int toMapIndex(int x, int y)
{
    return y * map.width + x;
}

int posToMapIndex(Position p)
{
    return toMapIndex(p.x, p.y);
}

void clearScreen()
{
    for (unsigned int i = 0; i < screenHeight; i++)
    {
        for (unsigned int j = 0; j < screenWidth; j++)
        {
            screen[i * screenWidth + j] = ' ';
        }
    }

}

unsigned int getMapCell(unsigned int x, unsigned int y, unsigned int type)
{
    if (x >= 0 && x < map.width && y >= 0 && y < map.height)
    {
        return map.cells[toMapIndex(x, y)];
    }
    else
    {
        return 1;
    }
}

void setMapCell(unsigned int x, unsigned int y, unsigned int type)
{
    if (x >= 0 && x < map.width && y >= 0 && y < map.height)
    {
        map.cells[toMapIndex(x, y)] = type;
    }
}

void setMapLine(int x, int y, int ax, int ay, int length, unsigned int type)
{
    int c = 0;

    while (c < length)
    {
        setMapCell(x + ax * c, y + ay * c, type);
        c++;
    }
}

void setMapHLine(int x, int y, int length, unsigned int type)
{
    setMapLine(x, y, 1, 0, length, type);
}

void setMapVLine(int x, int y, int length, unsigned int type)
{
    setMapLine(x, y, 0, 1, length, type);
}

unsigned char hasItem(unsigned char type)
{
    unsigned int i = 0;
    unsigned char found = 0;
    while (i < player.inventory.numItems && !found)
    {
        if (player.inventory.items[i] == type)
        {
            found = 1;
        }
        else
        {
            i ++;
        }
    }

    return found;
}


void drawMap()
{
    for (unsigned int i = 0; i < map.height; i++)
    {
        for (unsigned int j = 0; j < map.width; j++)
        {
            unsigned int cellType = map.cells[i * map.width + j];
            screen[i * map.width + j] = mapCellCharacters[cellType];
        }
    }
}

void drawEnemies()
{
    for (unsigned int i = 0; i < numEnemies; i++)
    {
        if (enemies[i].health > 0)
        {
            screen[posToMapIndex(enemies[i].position)] = enemyCharacter[enemies[i].type];
        }
    }
}

void drawItems()
{
    for (int i = 0; i < numItems; i++)
    {
        if(!items[i].collected)
        {
            screen[posToMapIndex(items[i].position)] = itemCharacter[items[i].type];
        }
    }
}

void drawPlayer()
{
    screen[posToMapIndex(player.position)] = playerCharacter;
}

void showStatus()
{
    printf("\n");
    printf("%s [ HP: %0.2f MP: %0.2f SP: %0.2f ]\n", name, player.stats.health, player.stats.magic, player.stats.strength);
    printf("\n");
    printf("Items [ ");
    for (unsigned int i = 0; i < player.inventory.numItems; i++)
    {
        printf("%c", itemCharacter[player.inventory.items[i]]);
        if (i < player.inventory.numItems - 1) { printf(" "); }
    }
    printf("]\n");

    printf("\n");
    for (unsigned int i = 0; i < numEnemies; i++)
    {
        if (enemies[i].health > 0)
        {
            printf("%c [ HP: %0.2f ]\n", enemyCharacter[enemies[i].type], enemies[i].health);
        }
    }

    printf("\n");
}


void showScreen()
{
    char screenLine[32 + 1];

    for (unsigned int i = 0; i < screenHeight; i++)
    {
        strncpy_s(screenLine, &screen[i * screenWidth], screenWidth);
        screenLine[32] = '\0';

        char line[32 + 2 + 1];

        sprintf_s(line, "%s\n", screenLine);

        printf(line);

    }

}

void goodBeep()
{
    Beep(220, 200);
    Beep(380, 200);
}

void badBeep()
{
    Beep(150, 200);
    Beep(80, 200);
}

void neutralBeep()
{
    Beep(60, 100);
}

int distance(Position p1, Position p2)
{
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int randRange(int min, int max)
{
    return min + rand() % (max - min + 1);
}

Position moveTo(Position pos, Position target)
{
    Position r;

    r = pos;

    int dX;
    int dY;
    if (pos.x == target.x) { dX = 0; }
    else if (pos.x < target.x) { dX = 1; }
    else { dX = -1;  }

    if (pos.y == target.y) { dY = 0; }
    else if (pos.y < target.y) { dY = 1; }
    else { dY = -1; }

    if (dX == 0) { r.y += dY; }
    else if (dY == 0) { r.x += dX; }
    else if(dX != 0 && dY != 0)
    {
        int move = rand() % 2;
        if (move == 0) { r.x += dX; }
        else { r.y += dY; }
    }

    return r;
}

void updateEnemy(int i)
{
    Enemy enemy = enemies[i];

    Position nextPos = enemy.position;

    char moveCancelled = 0;

    if(enemy.health > 0)
    {
        if(enemy.type == ENEMYTYPE_SPIDER)
        {
            if (distance(enemy.position, player.position) <= SPIDER_RADIUS)
            {
                nextPos = moveTo(enemy.position, player.position);
            }
        }

        unsigned int nextCell = map.cells[posToMapIndex(nextPos)];

        if (nextCell != CELLTYPE_EMPTY)
        {
            moveCancelled = 1;
        }

        // Check player

        if (!moveCancelled)
        {
            if (equalPos(nextPos, player.position))
            {
                if (enemy.type == ENEMYTYPE_SPIDER)
                {
                    int damage = SPIDER_MINDAMAGE + rand() % (SPIDER_MAXDAMAGE - SPIDER_MINDAMAGE + 1);
                    printf("** A spider attacked you!! [-%d] **\n", damage);
                    badBeep();
                    Sleep(1000);

                    player.stats.health -= damage;
                    if (player.stats.health < 0)
                    {
                        player.stats.health = 0;
                        printf("** Ooops! You were killed! **\n");
                        badBeep();
                        Sleep(1000);

                        gameState = GAMESTATE_DEAD;

                    }

                    moveCancelled = 1;
                }
            }
        }

        // Move

        if (!moveCancelled && !equalPos(enemy.position, nextPos))
        {
            enemy.position = nextPos;
            printf("Enemy moving\n");
            neutralBeep();
            Sleep(1000);

        }

        enemies[i] = enemy;


    }
}

void initPlayer()
{
    player.inventory.numItems = 0;
    for (unsigned int i = 0; i < player.inventory.numItems; i++)
    {
        player.inventory.items[i] = 0;
    }

    player.stats.health = 100;
    player.stats.magic = 50;
    player.stats.strength = 50;
}

void updatePlayer()
{
    printf("Use ARROWS to move\n");

    printf("\n");

    int action = _getch();
    action = _getch();

    // See https://learn.microsoft.com/en-us/cpp/c-runtime-library/reference/getch-getwch?view=msvc-170#:~:text=The%20_getch%20and%20_getwch%20functions%20read%20a%20single,The%20second%20call%20returns%20the%20key%20scan%20code.
    Position nextPos = player.position;
    char moveCancelled = 0;

    if (action == 72)
    {
        nextPos.y--;
    }
    else if (action == 80)
    {
        nextPos.y++;
    }
    else if (action == 75)
    {
        nextPos.x--;
    }
    else if (action == 77)
    {
        nextPos.x++;
    }


    // Check map

    unsigned int nextCell = map.cells[posToMapIndex(nextPos)];

    if (nextCell == CELLTYPE_WALL)
    {
        printf("** You hit a wall! **\n");
        badBeep();
        Sleep(1000);

        moveCancelled = 1;
    }
    else if (nextCell == CELLTYPE_DOOR)
    {
        if (hasItem(ITEMTYPE_KEY))
        {
            printf("** Well done! You escaped the dungeon! **\n");
            goodBeep();
            Sleep(1000);

            gameState = GAMESTATE_ESCAPED;
        }
        else
        {
            printf("** You need a key to open the door! **\n");
            badBeep();
            Sleep(1000);
        }

        moveCancelled = 1;

    }

    // Check enemies

    if (!moveCancelled)
    {
        for (unsigned int i = 0; i < numEnemies; i++)
        {
            if (equalPos(enemies[i].position, nextPos) && enemies[i].health > 0)
            {
                unsigned int damage;

                if (hasItem(ITEMTYPE_SWORD))
                {
                    damage = randRange(WEAPONSWORD_MINDAMAGE, WEAPONSWORD_MAXDAMAGE);
                    printf("** You attacked with sword [-%d]! **\n", damage);
                }
                else
                {
                    damage = randRange(WEAPONFISTS_MINDAMAGE, WEAPONFISTS_MAXDAMAGE);
                    printf("** You attacked with fists [-%d]! **\n", damage);
                }

                enemies[i].health -= damage;

                neutralBeep();
                Sleep(1000);

                if (enemies[i].health <= 0)
                {
                    enemies[i].health = 0;
                    printf("** You killed the enemy! **\n");
                    goodBeep();
                    Sleep(1000);
                }

                moveCancelled = 1;

            }
        }
    }

    // Check items

    if (!moveCancelled)
    {
        for (unsigned int i = 0; i < numItems; i++)
        {
            if (equalPos(items[i].position, nextPos) && !items[i].collected)
            {
                items[i].collected = 1;

                if (items[i].type == ITEMTYPE_FOOD)
                {
                    player.stats.health += 20.0f;
                    printf("Delicious food! [+20]\n");
                    goodBeep();
                    Sleep(1000);

                    moveCancelled = 1;
                }
                else
                {
                    player.inventory.items[player.inventory.numItems] = items[i].type;
                    player.inventory.numItems++;

                    if (items[i].type == ITEMTYPE_KEY)
                    {
                        printf("You found the key!\n");
                        goodBeep();
                        Sleep(1000);
                    }
                    else // items[i].type == ITEMTYPE_SWORD
                    {
                        printf("You found the sword!\n");
                        goodBeep();
                        Sleep(1000);
                    }

                    moveCancelled = 1;
                }

            }
        }
    }

    // Finally do the move

    if (!moveCancelled && !equalPos(player.position, nextPos))
    {

        player.position = nextPos;
        neutralBeep();
        Sleep(100);
    }

}


void initMap()
{
    map.width = 32;
    map.height = 16;

    for (unsigned int i = 0; i < map.height; i++)
    {
        for (unsigned int j = 0; j < map.width; j++)
        {
            map.cells[i * map.width + j] = 0;

            if (i == 0 || i == map.height - 1 || j == 0 || j == map.width - 1)
            {
                map.cells[i * map.width + j] = 1;
            }
        }
    }
}

void initEnemy(unsigned int i, int x, int y, unsigned int type)
{
    enemies[i].type = type;
    enemies[i].position.x = x;
    enemies[i].position.y = y;

    if (type == ENEMYTYPE_SPIDER)
    {
        enemies[i].health = SPIDER_MINHEALTH + rand() % (SPIDER_MAXHEALTH - SPIDER_MINHEALTH);
    }
}

void playDungeon(int dungeon)
{
    initPlayer();

    numEnemies = 0;
    numItems = 0;

    initMap();

    if (dungeon == 0)
    {
        player.position.x = 29;
        player.position.y = 3;

        numEnemies = 2;

        initEnemy(0, 10, 10, ENEMYTYPE_SPIDER);
        initEnemy(1, 20, 12, ENEMYTYPE_SPIDER);

        numItems = 3;

        items[0].type = ITEMTYPE_FOOD;
        items[0].position.x = 2;
        items[0].position.y = 4;
        items[0].collected = 0;

        items[1].type = ITEMTYPE_SWORD;
        items[1].position.x = 16;
        items[1].position.y = 14;
        items[1].collected = 0;

        items[2].type = ITEMTYPE_KEY;
        items[2].position.x = 29;
        items[2].position.y = 14;
        items[2].collected = 0;

        setMapCell(0, 8, 2);

        setMapHLine(8, 5, 10, 1);
        setMapHLine(15, 10, 10, 1);


    }

    gameState = GAMESTATE_PLAYING;

    while (gameState == GAMESTATE_PLAYING)
    {
        system("cls");
        clearScreen();
        drawMap();
        drawItems();
        drawEnemies();
        drawPlayer();
        showScreen();
        showStatus();

        updatePlayer();
        
        unsigned int i = 0;

        while(gameState == GAMESTATE_PLAYING && i < numEnemies)
        {
            system("cls");
            clearScreen();
            drawMap();
            drawItems();
            drawEnemies();
            drawPlayer();
            showScreen();
            showStatus();

            updateEnemy(i);

            i++;
        }

    }

}


int main()
{
    int code;

    code = -1;

    while (code != '0')
    {
        system("cls");

        printf("Hack dungeon\n");
        printf("============\n");

        printf("1.- Dungeon 1\n");
        printf("2.- Dungeon 2\n");
        printf("3.- Dungeon 3\n");
        printf("4.- Dungeon 4\n");

        printf("0.- Exit\n");

        code = _getch();

        if (code == '1') { playDungeon(0); }
        else if (code == '2') { playDungeon(1); }
        else if (code == '3') { playDungeon(2); }
        else if (code == '4') { playDungeon(3); }

    }




}

