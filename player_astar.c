// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf
#include <unistd.h>
#include <math.h>
#include <stdlib.h> // abs

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the paths
extern const char BONUS; // ascii used for the bonuses

char *binome = "Astar"; // student names here

void computeCell(
        int x,
        int y,
        int sourcex,
        int sourcey,
        int destinationx,
        int destinationy,
        int previousCost,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize]
);

int calculWeight(
        int x,
        int y,
        int destinationx,
        int destinationy,
        int mapxsize,
        int mapysize,
        int calculated[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize]
);

void foundPath(
        int x,
        int y,
        int apple_x,
        int apple_y,
        int mapxsize,
        int mapysize,
        int heuristic_ptr2[mapysize][mapxsize],
        int calculated_ptr2[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int chosen[mapysize][mapxsize]
);

char *color(int x, int y, int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]);
void displayGrid(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]);

action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y
);

int distance(int source_x, int source_y, int destination_x, int destination_y);

/*
  snake function:
  This function randomly select a valid move for Snake based on its current position on the game map.
 */
action snake(
        char **map, // array of char modeling the game map
        int mapxsize, // x size of the map
        int mapysize, // y size of the map
        snake_list s, // snake
        action last_action // last action made, -1 in the beginning
) {
    int cost[mapysize][mapxsize];
    int heuristic[mapysize][mapxsize];
    int calculated[mapysize][mapxsize];
    int chosen[mapysize][mapxsize];

    int apple_x = 0;
    int apple_y = 0;

    for (int search_y = 0; search_y < mapysize; search_y++) {
        for (int search_x = 0; search_x < mapxsize; search_x++) {
            cost[search_y][search_x] = 0;
            heuristic[search_y][search_x] = 0;
            calculated[search_y][search_x] = 0;
            chosen[search_y][search_x] = 0;

            if (map[search_y][search_x] == SNAKE_BODY || map[search_y][search_x] == SNAKE_HEAD ||
                map[search_y][search_x] == SNAKE_TAIL || map[search_y][search_x] == WALL) {
                cost[search_y][search_x] = -1;
                heuristic[search_y][search_x] = -1;
                calculated[search_y][search_x] = -1;
            }

            if (map[search_y][search_x] == BONUS) {
                apple_x = search_x;
                apple_y = search_y;
            }
        }
    }

    /*for (int search_y = 0; search_y < mapysize; search_y++) {
        for (int search_x = 0; search_x < mapxsize; search_x++) {
            printf("%d", calculated[search_y][search_x]);
        }
        printf("\n");
    }

    return -1;*/

    int x = s->x;
    int y = s->y;

    computeCell(x, y, x, y, apple_x, apple_y, 0, mapxsize, mapysize, cost, heuristic, calculated);

    calculated[y][x] = -1;
    heuristic[y][x] = -1;

    calculated[apple_y][apple_x] = 1;
    heuristic[apple_y][apple_x] = 1;

    int next_x = x;
    int next_y = y;

    foundPath(x, y, apple_x, apple_y, mapxsize, mapysize, heuristic, calculated, &next_x, &next_y, chosen);

    displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    if (next_x == x && next_y == y) {
        return last_action;
    }

    return calculate_direction(x, y, next_x, next_y);
}

void computeCell(
        int x,
        int y,
        int sourcex,
        int sourcey,
        int destinationx,
        int destinationy,
        int previousCost,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize]
) {
    if (x < 0 || y < 0 || x > mapxsize - 1 || y > mapysize - 1) {
        return;
    }

    if (calculated[y][x] > 0) {
        //printf("\033[0;32mCalculated !\033[0m\n");
        return;
    }

    if (cost[destinationy][destinationx] > 0) {
        //printf("\033[0;32mWeight found !\033[0m\n");
        return;
    }

    int currentCost = previousCost;

    if (x > 0 && cost[y][x - 1] == 0) {
        cost[y][x - 1] = currentCost + 1;
        heuristic[y][x - 1] =
                currentCost + 1 + calculWeight(x - 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic);
    }

    if (y > 0 && cost[y - 1][x] == 0) {
        cost[y - 1][x] = currentCost + 1;
        heuristic[y - 1][x] =
                currentCost + 1 + calculWeight(x, y - 1, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic);
    }

    if (x < mapxsize - 1 && cost[y][x + 1] == 0) {
        cost[y][x + 1] = currentCost + 1;
        heuristic[y][x + 1] =
                currentCost + 1 + calculWeight(x + 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic);
    }

    if (y < mapysize - 1 && cost[y + 1][x] == 0) {
        cost[y + 1][x] = currentCost + 1;
        heuristic[y + 1][x] =
                currentCost + 1 + calculWeight(x, y + 1, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic);
    }

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

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy, currentCost + 1, mapxsize, mapysize, cost,
                heuristic, calculated);
}

void foundPath(
        int x,
        int y,
        int apple_x,
        int apple_y,
        int mapxsize,
        int mapysize,
        int heuristic_ptr2[mapysize][mapxsize],
        int calculated_ptr2[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int chosen[mapysize][mapxsize]
) {
    if (x == apple_x && y == apple_y) {
        return;
    }

    int lessx = 0;
    int lessy = 0;
    int lessweight = 999999999;
    int currentWeight;

    if (x > 0 && chosen[y][x - 1] == 0) {
        currentWeight = heuristic_ptr2[y][x - 1];

        if (currentWeight > 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = x - 1;
            lessy = y;
        }
    }

    if (x < mapxsize - 1 && chosen[y][x + 1] == 0) {
        currentWeight = heuristic_ptr2[y][x + 1];

        if (currentWeight > 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = x + 1;
            lessy = y;
        }
    }

    if (y > 0 && chosen[y - 1][x] == 0) {
        currentWeight = heuristic_ptr2[y - 1][x];

        if (currentWeight > 0 && currentWeight < lessweight) {
            lessweight = currentWeight;
            lessx = x;
            lessy = y - 1;
        }
    }

    if (x < mapysize - 1 && chosen[y + 1][x] == 0) {
        currentWeight = heuristic_ptr2[y + 1][x];

        if (currentWeight > 0 && currentWeight < lessweight) {
//            lessweight = currentWeight;
            lessx = x;
            lessy = y + 1;
        }
    }

    if (lessweight == 999999999) { //impossible
        printf("Pas de chemin");
        return;
    }

    chosen[lessy][lessx] = 1;

    //TODO
    *next_x = lessx;
    *next_y = lessy;
    return;

    //foundPath(lessx, lessy, apple_x, apple_y, heuristic_ptr2, calculated_ptr2, next_x, next_y);
}

int calculWeight(
        int x,
        int y,
        int destinationx,
        int destinationy,
        int mapxsize,
        int mapysize,
        int calculated[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize]
) {
    if (calculated[y][x] != 0) {
        return heuristic[y][x];
    }

    return distance(x, y, destinationx, destinationy);
}

int distance(int source_x, int source_y, int destination_x, int destination_y) {
    return abs(source_x - destination_x) + abs(source_y - destination_y);
}

action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y
) {
    if (x == next_x) {
        if (y > next_y) {
            return NORTH;
        } else {
            return SOUTH;
        }
    } else if (x > next_x) {
        return WEST;
    } else {
        return EAST;
    }
}

char *color(int x, int y, int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]) {
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

void displayGrid(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]) {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapysize; x++) {
            printf("%s %3d \033[0m", color(x, y, mapxsize, mapysize, cost, chosen), heuristic[y][x]);
        }
        printf("\n");
    }
}
