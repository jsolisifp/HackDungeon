#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "HackDungeon.hpp"

char name[100] = "Player";
Player player;
Map map;


unsigned int numEnemies;
Enemy enemies[MAX_ENEMIES];

unsigned int numItems;
Item items[MAX_ITEMS];

char playerCharacter = '@';
char enemyCharacter[10] = "*~^";
char itemCharacter[10] = ".+P";
char mapCellCharacters[10] = " #]";

unsigned int screenWidth = MAP_WIDTH;
unsigned int screenHeight = MAP_HEIGHT;
char screen[MAP_HEIGHT * MAP_WIDTH];

int gameState;

unsigned char equalPos(Position p1, Position p2)
{
    return p1.x == p2.x && p1.y == p2.y;
}

int distance(Position p1, Position p2)
{
    return abs(p1.x - p2.x) + abs(p1.y - p2.y);
}

int randRange(int min, int max)
{
    return min + rand() % (max - min + 1);
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

int loadMap(int dungeon)
{
    char fileName[100];
    sprintf_s(fileName, "dungeon%d.map", dungeon + 1);

    FILE* f;

    fopen_s(&f, fileName, "rb");

    if (f)
    {
        fread(&map.width, sizeof(unsigned int), 1, f);
        fread(&map.height, sizeof(unsigned int), 1, f);
        for (int i = 0; i < map.height; i++)
        {
            for (int j = 0; j < map.width; j++)
            {
                fread(&map.cells[i * map.width + j], sizeof(unsigned int), 1, f);
            }
        }

        fread(&player.position.x, sizeof(int), 1, f);
        fread(&player.position.y, sizeof(int), 1, f);

        fread(&numItems, sizeof(unsigned int), 1, f);
        for (int i = 0; i < numItems; i++)
        {
            fread(&items[i].type, sizeof(unsigned int), 1, f);
            fread(&items[i].position.x, sizeof(int), 1, f);
            fread(&items[i].position.y, sizeof(int), 1, f);
        }

        fread(&numEnemies, sizeof(unsigned int), 1, f);
        for (int i = 0; i < numEnemies; i++)
        {
            fread(&enemies[i].type, sizeof(unsigned int), 1, f);
            fread(&enemies[i].position.x, sizeof(int), 1, f);
            fread(&enemies[i].position.y, sizeof(int), 1, f);
        }

        fclose(f);

        return 1;
    }
    else
    {
        return 0;
    }

}

void saveMap(int dungeon)
{
    char fileName[100];
    sprintf_s(fileName, "dungeon%d.map", dungeon + 1);

    FILE* f;

    fopen_s(&f, fileName, "wb");

    if (f != 0)
    {
        fwrite(&map.width, sizeof(unsigned int), 1, f);
        fwrite(&map.height, sizeof(unsigned int), 1, f);
        for (int i = 0; i < map.height; i++)
        {
            for (int j = 0; j < map.width; j++)
            {
                fwrite(&map.cells[i * map.width + j], sizeof(unsigned int), 1, f);
            }
        }

        fwrite(&player.position.x, sizeof(int), 1, f);
        fwrite(&player.position.y, sizeof(int), 1, f);

        fwrite(&numItems, sizeof(unsigned int), 1, f);
        for (int i = 0; i < numItems; i++)
        {
            fwrite(&items[i].type, sizeof(unsigned int), 1, f);
            fwrite(&items[i].position.x, sizeof(int), 1, f);
            fwrite(&items[i].position.y, sizeof(int), 1, f);
        }

        fwrite(&numEnemies, sizeof(unsigned int), 1, f);
        for (int i = 0; i < numEnemies; i++)
        {
            fwrite(&enemies[i].type, sizeof(unsigned int), 1, f);
            fwrite(&enemies[i].position.x, sizeof(int), 1, f);
            fwrite(&enemies[i].position.y, sizeof(int), 1, f);
        }

        fclose(f);

    }
    else
    {
        printf("Cannot save file");
        Sleep(1000);
    }

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
        if (enemies[i].health > 0 && distance(enemies[i].position, player.position) < PLAYER_SEERADIUS)
        {
            printf("%c [ HP: %0.2f ]\n", enemyCharacter[enemies[i].type], enemies[i].health);
        }
    }

    printf("\n");
}


void showScreen()
{
    char screenLine[MAP_WIDTH + 1];

    for (unsigned int i = 0; i < screenHeight; i++)
    {
        strncpy_s(screenLine, &screen[i * screenWidth], screenWidth);
        screenLine[MAP_WIDTH] = '\0';

        char line[MAP_WIDTH + 2 + 1];

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
    Beep(60, 200);
}

void dangerBeep()
{
    Beep(440, 200);
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

    int radius;
    int damage;

    if (enemy.type == ENEMYTYPE_SPIDER)
    {
        radius = SPIDER_RADIUS;
        damage = randRange(SPIDER_MINDAMAGE, SPIDER_MAXDAMAGE);
    }
    else if (enemy.type == ENEMYTYPE_BAT)
    {
        radius = BAT_RADIUS;
        damage = randRange(BAT_MINDAMAGE, BAT_MAXDAMAGE);
    }
    else // enemy.type == ENEMYTYPE_SNAKE
    {
        radius = SNAKE_RADIUS;
        damage = randRange(SNAKE_MINDAMAGE, SNAKE_MAXDAMAGE);
    }

    if(enemy.health > 0)
    {
        if (distance(enemy.position, player.position) <= radius)
        {
            nextPos = moveTo(enemy.position, player.position);
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
                char name[100];
                if (enemy.type == ENEMYTYPE_SPIDER) { sprintf_s(name, "SPIDER"); }
                else if (enemy.type == ENEMYTYPE_BAT) { sprintf_s(name, "BAT"); }
                else// enemy.type == ENEMYTYPE_SNAKE)
                { sprintf_s(name, "SNAKE"); }
                printf("** A %s attacked you!! [-%d] **\n", name, damage);
                badBeep();
                Sleep(1000);

                player.stats.health -= damage;
                if (player.stats.health <= 0)
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

        // Move

        if (!moveCancelled && !equalPos(enemy.position, nextPos))
        {
            enemy.position = nextPos;
            printf("Enemy moving\n");
            dangerBeep();
            Sleep(200);

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

    player.stats.health = PLAYER_HEALTH;
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
                    player.stats.health += FOOD_HEALTHUP;
                    printf("Delicious food! [+10]\n");
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
        Sleep(200);

    }

}


void initMap()
{
    map.width = MAP_WIDTH;
    map.height = MAP_HEIGHT;

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

void initEnemy(unsigned int i)
{
    unsigned int type = enemies[i].type;

    if (type == ENEMYTYPE_SPIDER)
    {
        enemies[i].health = SPIDER_MINHEALTH + rand() % (SPIDER_MAXHEALTH - SPIDER_MINHEALTH);
    }
    else if (type == ENEMYTYPE_BAT)
    {
        enemies[i].health = BAT_MINHEALTH + rand() % (BAT_MAXHEALTH - BAT_MINHEALTH);
    }
    else // type == ENEMYTYPE_SNAKE
    {
        enemies[i].health = SNAKE_MINHEALTH + rand() % (SNAKE_MAXHEALTH - SNAKE_MINHEALTH);
    }
}

void initItem(unsigned int i)
{
    items[i].collected = 0;
}

void initEnemy(unsigned int i, int x, int y, unsigned int type)
{
    if (type == ENEMYTYPE_SPIDER)
    {
        enemies[i].health = SPIDER_MINHEALTH + rand() % (SPIDER_MAXHEALTH - SPIDER_MINHEALTH);
    }
    else if (type == ENEMYTYPE_BAT)
    {
        enemies[i].health = BAT_MINHEALTH + rand() % (BAT_MAXHEALTH - BAT_MINHEALTH);
    }
    else // type == ENEMYTYPE_SNAKE
    {
        enemies[i].health = SNAKE_MINHEALTH + rand() % (SNAKE_MAXHEALTH - SNAKE_MINHEALTH);
    }
}

void playDungeon(int dungeon)
{
    loadMap(dungeon);

    initPlayer();

    for (int i = 0; i < numEnemies; i++) { initEnemy(i); }
    for (int i = 0; i < numItems; i++) { initItem(i); }

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

        printf(" +==============+\n");
        printf(" | Hack Dungeon |\n");
        printf(" +==============+\n");
        printf("\n");
        printf(" 1.- Dungeon 1\n");
        printf(" 2.- Dungeon 2\n");
        printf("\n");
        printf(" 0.- Exit\n");

        code = _getch();

        if (code == '1') { playDungeon(0); }
        else if (code == '2') { playDungeon(1); }

    }




}

