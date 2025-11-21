#include <stdio.h>
#include <stdlib.h>

char map[10][11] = {
    "||||||||||",
    "|1***#***|",
    "||*||||*||",
    "|*#***#*2|",
    "|*|||||*|",
    "|***#***|",
    "||*|||*||",
    "|***#***|",
    "|*|||||*|",
    "||||||||||"
};

void checkSurroundings(int x, int y) {
    printf("You stand quietly, trying to sense the area..\n");
    
    //ini ntar bs diganti buat narasi 
    if (map[x-1][y] != '|')
        printf("> Go north (w)\n");

    if (map[x+1][y] != '|')
        printf("> Go south (s)\n");

    if (map[x][y-1] != '|')
        printf("> Go west (a)\n");

    if (map[x][y+1] != '|')
        printf("> Go east (d)\n");
}

void coordinates(int *playerX, int *playerY, int *enemyX, int *enemyY) {
    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (map[i][j] == '1') {
                *playerX = i;
                *playerY = j;
            }
            if (map[i][j] == '2') {
                *enemyX = i;
                *enemyY = j;
            }
        }
    }
}

int countOpenPaths(int x, int y) {
    //cek ada brp arah playerny bs gerak
    int count = 0;

    if (map[x-1][y] != '|') count++;
    if (map[x+1][y] != '|') count++;
    if (map[x][y-1] != '|') count++;
    if (map[x][y+1] != '|') count++;

    return count;
}

void movePlayer(int *x, int *y, int enemyX, int enemyY, char dir) {
    int dx = 0, dy = 0;

    if (dir == 'w') dx = -1;
    if (dir == 's') dx = +1;
    if (dir == 'a') dy = -1;
    if (dir == 'd') dy = +1;

    // cek tembok
    if (map[*x + dx][*y + dy] == '|') {
        printf("You bump into a wall. Can't move there.\n");
        return;
    }
    
    *x += dx;
    *y += dy;

    // cek collision sm musuh
    if (*x == enemyX && *y == enemyY) {
        printf("YOU LOSE.\n");
        exit(0);
    }
}

void showMap(int playerX, int playerY, int enemyX, int enemyY) {
    //cuma debugging aja
    printf("\n\n");

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i == playerX && j == playerY)
                printf("P");      // Player
            else if (i == enemyX && j == enemyY)
                printf("J");      // Enemy
            else
                printf("%c", map[i][j]);  // Normal map tile
        }
        printf("\n");
    }
    printf("\n\n");
}

int playerInSight(int enemyX, int enemyY, int playerX, int playerY) {
    // kalo bener, return 1 (artinya bakal msk chase mode buat musuh)
    // baris yang sama
    if (enemyX == playerX) {
        int start = enemyY < playerY ? enemyY + 1 : playerY + 1;
        int end   = enemyY < playerY ? playerY - 1 : enemyY - 1;
        for (int y = start; y <= end; y++)
            if (map[enemyX][y] == '|') return 0; // blocked
        return 1;
    }

    // kolum yang sama
    if (enemyY == playerY) {
        int start = enemyX < playerX ? enemyX + 1 : playerX + 1;
        int end   = enemyX < playerX ? playerX - 1 : enemyX - 1;
        for (int x = start; x <= end; x++)
            if (map[x][enemyY] == '|') return 0; // blocked
        return 1;
    }

    return 0;
}

void enemyRandomMove(int *ex, int *ey) {
    //klo bkn chase mode, musuh bakal gerak random
    int dirs[4][2] = {{-1,0},{1,0},{0,-1},{0,1}};
    int valid[4];
    int count = 0;

    for (int i = 0; i < 4; i++) {
        int nx = *ex + dirs[i][0];
        int ny = *ey + dirs[i][1];
        if (map[nx][ny] == '*' || map[nx][ny] == '#') {
            valid[count++] = i;
        }
    }

    if (count == 0) return;
    int pick = valid[rand() % count];
    
    *ex += dirs[pick][0];
    *ey += dirs[pick][1];
}

void enemyChaseMove(int *ex, int *ey, int px, int py) {
    int dx = 0, dy = 0;

    if (px < *ex) dx = -1;
    else if (px > *ex) dx = 1;

    if (py < *ey) dy = -1;
    else if (py > *ey) dy = 1;

    if (dx != 0 && (map[*ex + dx][*ey] == '*' || map[*ex + dx][*ey] == '#')) {
        *ex += dx;
        return;
    }

    if (dy != 0 && (map[*ex][*ey + dy] == '*' || map[*ex][*ey + dy] == '#')) {
        *ey += dy;
        return;
    }
}

void warnDirection(int enemyX, int enemyY, int playerX, int playerY) {
    int dx = enemyX - playerX;
    int dy = enemyY - playerY;
    printf("You sense danger ");

    if (dx > 0) printf("from the south");
    else if (dx < 0) printf("from the north");

    if (dy > 0) printf("%sfrom the east", (dx != 0 ? " and " : ""));
    else if (dy < 0) printf("%sfrom the west", (dx != 0 ? " and " : ""));
    printf(".\n");
}


void enemyTurn(int *enemyX, int *enemyY, int playerX, int playerY) {
    int chasing = playerInSight(*enemyX, *enemyY, playerX, playerY);

    if (chasing) {
        enemyChaseMove(enemyX, enemyY, playerX, playerY);
    } else {
        enemyRandomMove(enemyX, enemyY);
    }
    
    int dist = abs(*enemyX - playerX) + abs(*enemyY - playerY);
        
    if (dist <= 2) {
        warnDirection(*enemyX, *enemyY, playerX, playerY);
    }
    
    if (*enemyX == playerX && *enemyY == playerY) {
        printf("YOU LOSE.\n");
        exit(0);
    }
}

int main() {
    int playerX = 0, playerY = 0;
    int enemyX = 0, enemyY = 0;

    coordinates(&playerX, &playerY, &enemyX, &enemyY);

    while (1) {
        showMap(playerX, playerY, enemyX, enemyY);
        checkSurroundings(playerX, playerY);

        printf("Move (w/a/s/d): ");
        char move;
        scanf(" %c", &move);

        movePlayer(&playerX, &playerY, enemyX, enemyY, move);
        enemyTurn(&enemyX, &enemyY, playerX, playerY);
    }
}
