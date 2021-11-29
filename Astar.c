#include <stdio.h>
#include <stdlib.h> // abs

void displayGrid();

void computeCell(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy, int previousCost);

#define mapxsize 10
#define mapysize 10

int distance(int source_x, int source_y, int destination_x, int destination_y);

void tracePath(int sourcex, int sourcey, int destinationy, int destinationx);

int cost[mapysize][mapxsize];
int heuristic[mapysize][mapxsize];
int calculated[mapysize][mapxsize];
int chosen[mapysize][mapxsize];

int main() {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapxsize; x++) {
            cost[y][x] = 0;
            heuristic[y][x] = 0;
            calculated[y][x] = 0;
            chosen[y][x] = 0;
        }
    }

    for (int y = 0; y < mapysize; y++) {
        calculated[y][0] = -1;
        calculated[y][mapxsize - 1] = -1;

        cost[y][0] = -1;
        cost[y][mapxsize - 1] = -1;

        heuristic[y][0] = -1;
        heuristic[y][mapxsize - 1] = -1;
    }

    for (int x = 0; x < mapxsize; x++) {
        calculated[0][x] = -1;
        calculated[mapysize - 1][x] = -1;

        cost[0][x] = -1;
        cost[mapysize - 1][x] = -1;

        heuristic[0][x] = -1;
        heuristic[mapysize - 1][x] = -1;
    }

    calculated[4][3] = -1;
    cost[4][3] = -1;
    heuristic[4][3] = -1;

    calculated[5][3] = -1;
    cost[5][3] = -1;
    heuristic[5][3] = -1;

    calculated[6][3] = -1;
    cost[6][3] = -1;
    heuristic[6][3] = -1;

    calculated[7][3] = -1;
    cost[7][3] = -1;
    heuristic[7][3] = -1;

    calculated[8][3] = -1;
    cost[8][3] = -1;
    heuristic[8][3] = -1;

    calculated[4][4] = -1;
    cost[4][4] = -1;
    heuristic[4][4] = -1;

    calculated[4][5] = -1;
    cost[4][5] = -1;
    heuristic[4][5] = -1;

    calculated[4][6] = -1;
    cost[4][6] = -1;
    heuristic[4][6] = -1;

    calculated[3][6] = -1;
    cost[3][6] = -1;
    heuristic[3][6] = -1;

    calculated[2][6] = -1;
    cost[2][6] = -1;
    heuristic[2][6] = -1;

    /*calculated[1][6] = -1;
    cost[1][6] = -1;
    heuristic[1][6] = -1;*/

    computeCell(1, 1, 1, 1, 8, 8, 0);

    calculated[1][1] = 1;
    cost[1][1] = 0;
    heuristic[1][1] = 0;

    tracePath(8, 8, 1, 1);

    displayGrid();

    return 0;
}

void tracePath(int sourcex, int sourcey, int destinationy, int destinationx) {
    if (sourcex == destinationx && sourcey == destinationy) {
        return;
    }

    int lessx = 0;
    int lessy = 0;
    int lessweight = 999999999;
    int currentWeight;

    if (sourcex > 0 && chosen[sourcey][sourcex - 1] == 0) {
        currentWeight = heuristic[sourcey][sourcex - 1];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex - 1;
            lessy = sourcey;
        }
    }

    if (sourcex < mapxsize - 1 && chosen[sourcey][sourcex + 1] == 0) {
        currentWeight = heuristic[sourcey][sourcex + 1];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex + 1;
            lessy = sourcey;
        }
    }

    if (sourcey > 0 && chosen[sourcey - 1][sourcex] == 0) {
        currentWeight = heuristic[sourcey - 1][sourcex];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex;
            lessy = sourcey - 1;
        }
    }

    if (sourcex < mapysize - 1 && chosen[sourcey + 1][sourcex] == 0) {
        currentWeight = heuristic[sourcey + 1][sourcex];

        if (currentWeight >= 0 && currentWeight < lessweight) {
//            lessweight = currentWeight;
            lessx = sourcex;
            lessy = sourcey + 1;
        }
    }

    if (lessweight == 999999999) { //impossible
        printf("Pas de chemin");
        return;
    }

    chosen[lessy][lessx] = 1;

    displayGrid();

    tracePath(lessx, lessy, destinationx, destinationy);
}

int calculWeight(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy) {
    if (calculated[y][x] != 0) {
        return heuristic[y][x];
    }

    int cost = distance(x, y, destinationx, destinationy);

    return cost;
}

void computeCell(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy, int previousCost) {
    if (x < 0 || y < 0 || x > mapxsize - 1 || y > mapysize - 1) {
        return;
    }

    if (calculated[y][x] != 0) {
        printf("\033[0;32mCalculated !\033[0m\n");
        return;
    }

    if (cost[destinationy][destinationx] != 0) {
        printf("\033[0;32mWeight found !\033[0m\n");
        return;
    }

    int currentCost = previousCost;

    if (x > 0 && cost[y][x - 1] == 0) {
        cost[y][x - 1] = currentCost + 1;
        heuristic[y][x - 1] = currentCost + 1 + calculWeight(x - 1, y, sourcex, sourcey, destinationx, destinationy);
    }

    if (y > 0 && cost[y - 1][x] == 0) {
        cost[y - 1][x] = currentCost + 1;
        heuristic[y - 1][x] = currentCost + 1 + calculWeight(x, y - 1, sourcex, sourcey, destinationx, destinationy);
    }

    if (x < mapxsize - 1 && cost[y][x + 1] == 0) {
        cost[y][x + 1] = currentCost + 1;
        heuristic[y][x + 1] = currentCost + 1 + calculWeight(x + 1, y, sourcex, sourcey, destinationx, destinationy);
    }

    if (y < mapysize - 1 && cost[y + 1][x] == 0) {
        cost[y + 1][x] = currentCost + 1;
        heuristic[y + 1][x] = currentCost + 1 + calculWeight(x, y + 1, sourcex, sourcey, destinationx, destinationy);
    }

    displayGrid();

    calculated[y][x] = 1;

    int lessx;
    int lessy;
    int lessweight = 999999999;

    for (int testy = 0; testy < mapysize; testy++) {
        for (int testx = 0; testx < mapxsize; testx++) {
            if (calculated[testy][testx] != 0) {
                continue;
            }

            int currentWeight = heuristic[testy][testx];

            if (currentWeight > 0) {
                if (currentWeight < lessweight) {
                    lessweight = currentWeight;
                    lessx = testx;
                    lessy = testy;
                }
            }
        }
    }

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy, currentCost + 1);
}

char *color(int x, int y) {
    switch (cost[y][x]) {
        case 0:
            return "\033[0m";
        case -1:
            return "\033[0;34m";
    }

    if (chosen[y][x] == 0)
        return "\033[0;32m";
    else
        return "\033[0;36m";
}

void displayGrid() {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapysize; x++) {
            printf("%s %3d \033[0m", color(x, y), heuristic[y][x]);
        }
        printf("\n");
    }
}

int distance(int source_x, int source_y, int destination_x, int destination_y) {
    return abs(source_x - destination_x) + abs(source_y - destination_y);
}