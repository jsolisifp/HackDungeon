#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define ITEMTYPE_FOOD 0
#define ITEMTYPE_SWORD 1
#define ITEMTYPE_KEY 2

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


int numEnemies;
Enemy enemies[10];

int numItems;
Item items[10];

char playerCharacter = '@';
char enemyCharacter[10] = "*~^";
char itemCharacter[10] = ".+P";
char mapCellCharacters[10] = " #]";

unsigned int screenWidth = 32;
unsigned int screenHeight = 16;
char screen[16 * 32];

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

unsigned int getMapCell(int x, int y, unsigned int type)
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

void setMapCell(int x, int y, unsigned int type)
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
    int i = 0;
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
    for (int i = 0; i < numEnemies; i++)
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

void playDungeon(int dungeon)
{
    player.inventory.numItems = 0;
    for (unsigned int i = 0; i < player.inventory.numItems; i++)
    {
        player.inventory.items[i] = 0;
    }

    player.stats.health = 100;
    player.stats.magic = 50;
    player.stats.strength = 50;

    numEnemies = 0;
    numItems = 0;

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

    if (dungeon == 0)
    {
        player.position.x = 29;
        player.position.y = 3;

        numEnemies = 1;

        enemies[0].type = 0;
        enemies[0].position.x = 10;
        enemies[0].position.y = 10;

        enemies[0].health = 5;

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

    char openedDoor = 0;

    while (player.stats.health > 0 && !openedDoor)
    {
        system("cls");
        clearScreen();
        drawMap();
        drawItems();
        drawEnemies();
        drawPlayer();
        showScreen();

        printf("%s [ HP: %0.2f MP: %0.2f SP: %0.2f ]\n", name, player.stats.health, player.stats.magic, player.stats.strength);
        printf("Items [ ");
        for (int i = 0; i < player.inventory.numItems; i++)
        {
            printf("%c", itemCharacter[player.inventory.items[i]]);
            if (i < player.inventory.numItems - 1) { printf(" "); }
        }
        printf("]\n");

        for (int i = 0; i < numEnemies; i++)
        {
            printf("%c [ HP: %0.2f ]\n", enemyCharacter[enemies[i].type], enemies[i].health);
        }

        printf("1.- Go up\n");
        printf("2.- Go down\n");
        printf("3.- Go left\n");
        printf("4.- Go right\n");

        int action = -1;

        printf("> ");
        scanf_s("%d", &action);

        if (action == 1)
        {
            Position nextPos = player.position;

            nextPos.y--;

            if (map.cells[posToMapIndex(nextPos)] != 1) { player.position = nextPos;  }
            else { printf("You hit a wall!\n"); Sleep(2000); }
        }
        else if (action == 2)
        {
            Position nextPos = player.position;

            nextPos.y++;

            if (map.cells[posToMapIndex(nextPos)] != 1) { player.position = nextPos; }
            else { printf("You hit a wall!\n"); Sleep(2000); }
        }
        else if (action == 3)
        {
            Position nextPos = player.position;

            nextPos.x--;

            if (map.cells[posToMapIndex(nextPos)] != 1) { player.position = nextPos; }
            else { printf("You hit a wall!\n"); Sleep(2000); }
        }
        else if (action == 4)
        {
            Position nextPos = player.position;

            nextPos.x++;

            if (map.cells[posToMapIndex(nextPos)] != 1) { player.position = nextPos; }
            else { printf("You hit a wall!\n"); Sleep(2000); }
        }

        for (int i = 0; i < numItems; i++)
        {
            if(equalPos(items[i].position, player.position) && !items[i].collected)
            {
                items[i].collected = 1;

                if(items[i].type == ITEMTYPE_FOOD)
                {
                    player.stats.health += 20.0f;
                    printf("Delicious food! [+20 HP]\n"); Sleep(2000);
                }
                else
                {
                    player.inventory.items[player.inventory.numItems] = items[i].type;
                    player.inventory.numItems++;

                    if (items[i].type == ITEMTYPE_KEY)
                    {
                        printf("You found the key!\n"); Sleep(2000);
                    }
                    else // items[i].type == ITEMTYPE_SWORD
                    {
                        printf("You found the sword!\n"); Sleep(2000);
                    }
                }

            }
        }

        for (int i = 0; i < numEnemies; i++)
        {
            if (equalPos(enemies[i].position, player.position) && enemies[i].health > 0)
            {
                if (hasItem(ITEMTYPE_SWORD))
                {
                    enemies[i].health = 0;
                    printf("You killed the enemy with the sword!\n"); Sleep(2000);
                }
                else
                {
                    player.stats.health = 0;
                    printf("Ooops! You were killed by the enemy!\n"); Sleep(2000);
                }

            }
        }

        if(map.cells[posToMapIndex(player.position)] == 2)
        {
            if (hasItem(ITEMTYPE_KEY))
            {
                printf("Well done! You escaped the dungeon!\n"); Sleep(2000);
                openedDoor = 1;
            }
            else
            {
                printf("You need a key to open the door!\n"); Sleep(2000);
            }
        }
    }

}


int main()
{
    int code;

    code = -1;

    while (code != 0)
    {
        system("cls");

        printf("Hack dungeon\n");
        printf("============\n");

        printf("1.- Dungeon 1\n");
        printf("2.- Dungeon 2\n");
        printf("3.- Dungeon 3\n");
        printf("4.- Dungeon 4\n");

        printf("0.- Exit\n");

        printf("> ");
        scanf_s("%d", &code);

        if (code == 1) { playDungeon(0); }
        else if (code == 2) { playDungeon(1); }
        else if (code == 3) { playDungeon(2); }
        else if (code == 4) { playDungeon(3); }

    }




}

