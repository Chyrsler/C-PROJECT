
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int keys = 0;
int steps = 0;

char map[10][11] = {
    "||||||||||",
    "|1***#***|",
    "||*||||*||",
    "|*******2|",
    "|*|||||*|",
    "|*******|",
    "||||||&||",
    "|*******|",
    "|*|||||*|",
    "|||||||E||"
};

//ngecek ada berapa arah player bisa gerak
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

//ngambil starting koordinat dari player & enemy (kalau mau buat map lbh dr 1)
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

//logika gerak player
void movePlayer(int *x, int *y, int enemyX, int enemyY, char dir, int *win) {
    int dx = 0, dy = 0;
    steps++;
    
    //korespondensi sm array
    if (dir == 'w') dx = -1;
    if (dir == 's') dx = +1;
    if (dir == 'a') dy = -1;
    if (dir == 'd') dy = +1;

    char nextTile = map[*x + dx][*y + dy];

    if (nextTile == '|') {
        printf("You bump into a wall.\n");
        return;
    }

    if (nextTile == '&') {
        if (keys > 0) {
            printf("You use a key to unlock the door.\n");
            keys--;
            map[*x + dx][*y + dy] = '*'; //Ngubah pintu "&"jadi "*" 
        } else {
            printf("The door is locked. You need a key.\n");
            return;
        }
    }

    // ngambil kunci
    if (nextTile == '#') {
        printf("You found a key.\n");
        keys++;
        map[*x + dx][*y + dy] = '*'; //Ngurangin jumlah kunci di map
    }

    if(nextTile == 'E'){
        printf("YOU ESCAPED THE MAZE!\n");
        *win = 0;
        return;
    }

    //update tempat player stlh gerak
    *x += dx;
    *y += dy;

    if (*x == enemyX && *y == enemyY) {
        printf("YOU LOSE.\n");
        exit(0);
    }
}

//Ngeprint map (mungkin buat debugging aja?)
void showMap(int playerX, int playerY, int enemyX, int enemyY) {
    //cuma debugging aja
    printf("\n\n");

    for (int i = 0; i < 10; i++) {
        for (int j = 0; j < 10; j++) {
            if (i == playerX && j == playerY)
                printf("P"); // Player
            else if (i == enemyX && j == enemyY)
                printf("J"); // Enemy
            else
                printf("%c", map[i][j]); //Normal map
        }
        printf("\n");
    }
    printf("\n\n");
}

//Logika enemy behaviour (kalo bisa ngeliat player)
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

//Logika enemy behaviour (kalo ga bisa ngeliat player)
void enemyRandomMove(int *ex, int *ey) {
    //klo chasing = false, musuh bakal gerak random
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

//Function enemy behaviour kalo playerInSight true
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

//Ngasih warning musuh dateng dari arah mana
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

//Enemy behaviour
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

int isLeaderboard(char *newUser, int newScore) {
    FILE *fp = fopen("topTen.txt", "r");

    char names[20][50];
    int scores[20];
    int count = 0; //counter buat ngestore ke array (anggap sebagai iteration/i)

    //baca nama2 yang sudah disimpan di topTen.txt
    if (fp != NULL) {
        while (fscanf(fp, "%s %d", names[count], &scores[count]) == 2) { //failsafe, klo nilai names/scores ilang maka g bakal dibaca sebagai data
            count++;
            if (count >= 20) break; //baca 20 baris pertama saja
        }
        fclose(fp);
    }

    //nyimpan nama, score user di anggota array names & count
    strcpy(names[count], newUser);
    scores[count] = newScore;
    count++;

    //SORTING ALGORITHM (selection sorting)
    for (int i = 0; i < count - 1; i++) {
        for (int j = i + 1; j < count; j++) {
            if (scores[j] < scores[i]) {
                //sorting dari kecil -> besar dari score & berkorespondensi sama nama
                int tempScore = scores[i];
                scores[i] = scores[j];
                scores[j] = tempScore;

                char tempName[50];
                strcpy(tempName, names[i]);
                strcpy(names[i], names[j]);
                strcpy(names[j], tempName);
            }
        }
    }

    int newCount = count;
    if (newCount > 10) newCount = 10;

    fp = fopen("topTen.txt", "w");
    for (int i = 0; i < newCount; i++) {
        fprintf(fp, "%s %d\n", names[i], scores[i]);
    }
    fclose(fp);

    //Cek kalau score & nama masuk ke top 10, kalau iya return 1 
    for (int i = 0; i < newCount; i++) {
        if (strcmp(names[i], newUser) == 0 && scores[i] == newScore) {
            return 1;  
        }
    }

    return 0; //g masuk top 10
}

//print current leadboard
void printLeaderboard() {
    FILE *fp = fopen("topTen.txt", "r");
    
    if (fp == NULL) {
        printf("Leaderboard is empty.\n");
        return;
    }

    char name[50];
    int score;
    int rank = 1;

    printf("\n===== LEADERBOARD =====\n");
    printf(" Rank | Username            | Steps\n");
    printf("-------------------------------------\n");

    while (fscanf(fp, "%s %d", name, &score) == 2) {
        printf(" %4d | %-20s | %d\n", rank, name, score);
        rank++;
    }

    printf("-------------------------------------\n\n");

    fclose(fp);
}

int main() {
    int playerX = 0, playerY = 0;
    int enemyX = 0, enemyY = 0;
    int win = 1; //loop breaker
    char username[20];

    coordinates(&playerX, &playerY, &enemyX, &enemyY);
    
    while (win) {
        // showMap(playerX, playerY, enemyX, enemyY);
        checkSurroundings(playerX, playerY);

        printf("Move (w/a/s/d): ");
        char move;
        scanf(" %c", &move);
        system("cls");

        movePlayer(&playerX, &playerY, enemyX, enemyY, move, &win);
        enemyTurn(&enemyX, &enemyY, playerX, playerY);
    }
    printf("Enter your username: ");
    scanf("%s", username);

    if(isLeaderboard(username, steps)){
        printf("CONGRATULATIONS! YOU MADE IT TO THE LEADERBOARD\n");
    }
    else printf("sorry you didn't make it to the leaderboard :(\n");
    printLeaderboard();
}