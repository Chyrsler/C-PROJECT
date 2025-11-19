#include <stdio.h>

char map[20][21] = {
    "||||||||||||||||||||",
    "|1****|******#*****|",
    "|*|||||*|||||*|||**|",
    "|*|**#***|***|*#|**|",
    "|*|*||||*|*#*|*|*|*|",
    "|***||***|*|||*|*|*|",
    "||*|||*#*|*|||***|*|",
    "|****#****|***|||*|",
    "|*|||||||*|||*|2*#|",
    "|*|*****|***|*|***|",
    "|*|*|||*|*|*|*|||*|",
    "|***|*|***|*|***|*|",
    "|*|||*|*|||*|*|||*|",
    "|*#***|***#***|***|",
    "|||||*|||||*|||||*|",
    "|*****#******#****|",
    "|*|||||||*|||||||*|",
    "|*#******|***#****|",
    "|*|||*|||*|||*|||*|",
    "||||||||||||||||||||"
};

void checkSurroundings(int x, int y) {
    printf("You stand quietly, trying to sense the area..\n");
    
    //ini ntar bs diganti buat narasi 
    if (map[x-1][y] != '|')
        printf("Go above\n");

    if (map[x+1][y] != '|')
        printf("Go below\n");

    if (map[x][y-1] != '|')
        printf("Go left\n");

    if (map[x][y+1] != '|')
        printf("Go right\n");
}

void coordinates(int *playerX, int *playerY, int *enemyX, int *enemyY) {
    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
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

void movePlayer(int *x, int *y, char dir) {
    int dx = 0, dy = 0;

    if (dir == 'w') dx = -1;
    if (dir == 's') dx = +1;
    if (dir == 'a') dy = -1;
    if (dir == 'd') dy = +1;

    //cek klo arah gerak kena tembok atw g
    if (map[*x + dx][*y + dy] == '|') {
        printf("You bump into a wall. Can't move there.\n");
        return;
    }

    *x += dx;
    *y += dy;

    //gerak sampe ada tembok/opsi jalan lain
    while (1) {
        int openPaths = countOpenPaths(*x, *y);

        //klo ada lbh dr satu, berhenti
        if (openPaths > 2) {
            printf("You reach a junction and stop.\n");
            break;
        }
        
        //cm ada satu jalan balik
        if (openPaths <= 1) {
            printf("You reach a dead end.\n");
            break;
        }

        // jalan terus sampe ada tembok
        if (map[*x + dx][*y + dy] != '|') {
            *x += dx;
            *y += dy;
        } else {
            //kena tembok
            break;
        }
    }

}

void showMap(int playerX, int playerY) {
    //cuma debugging aja
    printf("\n\n");

    for (int i = 0; i < 20; i++) {
        for (int j = 0; j < 20; j++) {
            if (i == playerX && j == playerY)
                printf("P"); //posisi player
            else
                printf("%c", map[i][j]);
        }
        printf("\n");
    }
    printf("\n\n");
}

int main() {
    int playerX = 0, playerY = 0;
    int enemyX = 0, enemyY = 0;

    coordinates(&playerX, &playerY, &enemyX, &enemyY);

    printf("Player coordinates: %d, %d\n", playerX, playerY); //cek koordinat player bener g

    while (1) {
        showMap(playerX, playerY);
        checkSurroundings(playerX, playerY);

        printf("Move (w/a/s/d): ");
        char move;
        scanf(" %c", &move);

        movePlayer(&playerX, &playerY, move);
    }
}