// MapEditor.cpp : Este archivo contiene la función "main". La ejecución del programa comienza y termina ahí.
//

#include <stdio.h>
#include <conio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <windows.h>
#include <HackDungeon.hpp>

struct MapEnemy
{
    unsigned int type;
    Position position;
};

struct MapItem
{
    unsigned int type;
    Position position;
};

Position cursor;

Position player;
Map map;

unsigned int numEnemies;
MapEnemy enemies[10];

unsigned int numItems;
MapItem items[10];

char playerCharacter = '@';
char enemyCharacter[10] = "*~^";
char itemCharacter[10] = ".+P";
char mapCellCharacters[10] = " #]";

unsigned int screenWidth = MAP_WIDTH;
unsigned int screenHeight = MAP_HEIGHT;
char screen[MAP_HEIGHT * MAP_WIDTH];

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
        screen[posToMapIndex(enemies[i].position)] = enemyCharacter[enemies[i].type];
    }
}

void drawItems()
{
    for (int i = 0; i < numItems; i++)
    {
        screen[posToMapIndex(items[i].position)] = itemCharacter[items[i].type];
    }
}

void drawPlayer()
{
    screen[posToMapIndex(player)] = PLAYER_CHAR;
}

void drawCursor()
{
    screen[posToMapIndex(cursor)] = 'O';
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

        fread(&player.x, sizeof(int), 1, f);
        fread(&player.y, sizeof(int), 1, f);

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

        fwrite(&player.x, sizeof(int), 1, f);
        fwrite(&player.y, sizeof(int), 1, f);

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


void editMap(int dungeon)
{

    cursor.x = MAP_WIDTH / 2 + 3;
    cursor.y = MAP_HEIGHT / 2;

    int loaded = loadMap(dungeon);

    if (!loaded)
    {
        player.x = MAP_WIDTH / 2;
        player.y = MAP_HEIGHT / 2;

        numEnemies = 0;
        numItems = 0;

        initMap();
    }

    int code;

    code = -1;

    while (code != '0')
    {
        system("cls");
        clearScreen();
        drawMap();
        drawItems();
        drawEnemies();


        drawPlayer();
        drawCursor();
        showScreen();

        printf("\n");
        printf(" ARROWS move cursor\n");
        printf("\n");
        printf(" P move PLAYER to cursor\n");
        printf(" W put a WALL    D put a DOOR\n");
        printf(" F put FOOD      S put SWORD    K put KEY\n");
        printf(" I put SPIDER    B put BAT      N put SNAKE\n");
        printf("\n");
        printf(" C to CLEAR cell\n");
        printf("\n");
        printf(" 0 to SAVE & EXIT\n");

        code = _getch();

        Position nextPos = cursor;
        char moveCancelled = 0;

        if (code == 0 || code == 224)
        {
            int specialCode = _getch();

            if (specialCode == 72)
            {
                nextPos.y--;
            }
            else if (specialCode == 80)
            {
                nextPos.y++;
            }
            else if (specialCode == 75)
            {
                nextPos.x--;
            }
            else if (specialCode == 77)
            {
                nextPos.x++;
            }
        }


        // Check map

        unsigned int nextCell = map.cells[posToMapIndex(nextPos)];

        if (nextPos.x < 0) { nextPos.x = 0;  }
        else if (nextPos.x >= MAP_WIDTH - 1) { nextPos.x = MAP_WIDTH - 1;  }

        if (nextPos.y < 0) { nextPos.y = 0; }
        else if (nextPos.y >= MAP_HEIGHT - 1) { nextPos.y = MAP_HEIGHT - 1; }

        cursor = nextPos;

        if (code != 0 && code != 224)
        {
            int upperCode = toupper(code);
            if (upperCode == 'P') { player = cursor; }
            else if (upperCode == 'W') { map.cells[posToMapIndex(cursor)] = CELLTYPE_WALL; }
            else if (upperCode == 'D') { map.cells[posToMapIndex(cursor)] = CELLTYPE_DOOR; }
            else if (upperCode == 'F' || upperCode == 'S' || upperCode == 'K')
            {
                if (numItems < MAX_ITEMS)
                {
                    MapItem item;
                    if (upperCode == 'F') { item.type = ITEMTYPE_FOOD; }
                    else if (upperCode == 'S') { item.type = ITEMTYPE_SWORD; }
                    else // upperCode == 'K'
                    { item.type = ITEMTYPE_KEY; }

                    item.position = cursor;

                    items[numItems] = item;

                    numItems++;
                }
                else
                {
                    printf("Max items reached");
                    Sleep(1000);
                }
            }
            else if (upperCode == 'I' || upperCode == 'B' || upperCode == 'N')
            {
                if (numEnemies < MAX_ENEMIES)
                {
                    MapEnemy enemy;
                    if (upperCode == 'I') { enemy.type = ENEMYTYPE_SPIDER; }
                    else if (upperCode == 'B') { enemy.type = ENEMYTYPE_BAT; }
                    else // upperCode == 'N'
                    { enemy.type = ENEMYTYPE_SNAKE; }

                    enemy.position = cursor;

                    enemies[numEnemies] = enemy;

                    numEnemies++;
                }
                else
                {
                    printf("Max enemies reached");
                    Sleep(1000);
                }
            }
            else if (upperCode == 'C')
            {
                for (int i = 0; i < numEnemies; i++)
                {
                    if (equalPos(enemies[i].position, cursor))
                    {
                        for (int j = i; j < numEnemies - 1; j++)
                        {
                            enemies[j] = enemies[j + 1];
                        }

                        numEnemies--;
                    }
                }

                for (int i = 0; i < numItems; i++)
                {
                    if (equalPos(items[i].position, cursor))
                    {
                        for (int j = i; j < numItems - 1; j++)
                        {
                            items[j] = items[j + 1];
                        }

                        numItems--;
                    }
                }

                map.cells[posToMapIndex(cursor)] = CELLTYPE_EMPTY;
            }
        }

    }

    saveMap(dungeon);


}

int main()
{
    int code;

    code = -1;

    while (code != '0')
    {
        system("cls");

        printf(" +============+\n");
        printf(" | Map editor |\n");
        printf(" +============+\n");
        printf(" \n");
        printf(" 1.- Dungeon 1\n");
        printf(" 2.- Dungeon 2\n");
        printf(" 3.- Dungeon 3\n");
        printf(" 4.- Dungeon 4\n");

        printf(" 0.- Exit\n");

        code = _getch();

        if (code == '1') { editMap(0); }
        else if (code == '2') { editMap(1); }
        else if (code == '3') { editMap(2); }
        else if (code == '4') { editMap(3); }

    }

}

