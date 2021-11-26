#include <stdio.h>

void displayGrid();

void computeCell(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy);

#define mapxsize 100
#define mapysize 100

int distance(int source_x, int source_y, int destination_x, int destination_y);

void tracePath(int sourcex, int sourcey, int destinationy, int destinationx);

int weights[mapysize][mapxsize];
int calculated[mapysize][mapxsize];
int chosen[mapysize][mapxsize];

int main() {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapxsize; x++) {
            weights[y][x] = 0;
            calculated[y][x] = 0;
            chosen[y][x] = 0;
        }
    }

    for (int y = 0; y < mapysize; y++) {
        calculated[y][0] = -1;
        calculated[y][mapxsize - 1] = -1;

        weights[y][0] = -1;
        weights[y][mapxsize - 1] = -1;
    }

    for (int x = 0; x < mapxsize; x++) {
        calculated[0][x] = -1;
        calculated[mapysize - 1][x] = -1;

        weights[0][x] = -1;
        weights[mapysize - 1][x] = -1;
    }

    calculated[4][3] = -1;
    weights[4][3] = -1;

    calculated[5][3] = -1;
    weights[5][3] = -1;

    calculated[6][3] = -1;
    weights[6][3] = -1;

    calculated[7][3] = -1;
    weights[7][3] = -1;

    calculated[8][3] = -1;
    weights[8][3] = -1;

    calculated[4][4] = -1;
    weights[4][4] = -1;

    calculated[4][5] = -1;
    weights[4][5] = -1;

    computeCell(1, 1, 1, 1, 98, 98);

    tracePath(98, 98, 1, 1);

    displayGrid();

    return 0;
}

void tracePath(int sourcex, int sourcey, int destinationy, int destinationx) {
    if (sourcex == destinationx && sourcey == destinationy) {
        return;
    }

    int lessx;
    int lessy;
    int lessweight = 999999999;
    int currentWeight;

    if (sourcex > 0 && chosen[sourcey][sourcex - 1] == 0) {
        currentWeight = weights[sourcey][sourcex - 1];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex - 1;
            lessy = sourcey;
        }
    }

    if (sourcex < mapxsize - 1 && chosen[sourcey][sourcex + 1] == 0) {
        currentWeight = weights[sourcey][sourcex + 1];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex + 1;
            lessy = sourcey;
        }
    }

    if (sourcey > 0 && chosen[sourcey - 1][sourcex] == 0) {
        currentWeight = weights[sourcey - 1][sourcex];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex;
            lessy = sourcey - 1;
        }
    }

    if (sourcex < mapysize - 1 && chosen[sourcey + 1][sourcex] == 0) {
        currentWeight = weights[sourcey + 1][sourcex];

        if (currentWeight >= 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = sourcex;
            lessy = sourcey + 1;
        }
    }

    chosen[lessy][lessx] = 1;

    displayGrid();

    tracePath(lessx, lessy, destinationx, destinationy);
}

int calculWeight(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy) {
    if (calculated[y][x] != 0) {
        return weights[y][x];
    }

    int wstart = distance(x, y, sourcex, sourcey);
    int wend = distance(x, y, destinationx, destinationy);

    if (wstart > wend) {
        return 2 * wend + wstart;
    } else {
        return 2 * wstart + wend;
    }
}

void computeCell(int x, int y, int sourcex, int sourcey, int destinationx, int destinationy) {
    if (calculated[y][x] != 0) {
        printf("\033[0;32mCalculated !\033[0m\n");
        return;
    }

    if (weights[destinationy][destinationx] != 0) {
        printf("\033[0;32mWeight found !\033[0m\n");
        return;
    }

    weights[y][x] = calculWeight(x, y, sourcex, sourcey, destinationx, destinationy);

    if (x > 0) {
        weights[y][x - 1] = calculWeight(x - 1, y, sourcex, sourcey, destinationx, destinationy);
    }

    if (y > 0) {
        weights[y - 1][x] = calculWeight(x, y - 1, sourcex, sourcey, destinationx, destinationy);
    }

    if (x < mapxsize - 1) {
        weights[y][x + 1] = calculWeight(x + 1, y, sourcex, sourcey, destinationx, destinationy);
    }

    if (y < mapysize - 1) {
        weights[y + 1][x] = calculWeight(x, y + 1, sourcex, sourcey, destinationx, destinationy);
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

            int currentWeight = weights[testy][testx];

            if (currentWeight > 0) {
                if (currentWeight < lessweight) {
                    lessweight = currentWeight;
                    lessx = testx;
                    lessy = testy;
                }
            }
        }
    }

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy);
}

char *color(int x, int y) {
    switch (weights[y][x]) {
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
            printf("%s %3d \033[0m", color(x, y), weights[y][x]);
        }
        printf("\n");
    }
}

int distance(int source_x, int source_y, int destination_x, int destination_y) {
    return (source_x - destination_x) * (source_x - destination_x) + (source_y - destination_y) * (source_y - destination_y);
}