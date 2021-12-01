// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand, abs
#include <stdio.h> // printf
#include <unistd.h>
#include <math.h>

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the paths
extern const char BONUS; // ascii used for the bonuses
extern bool DEBUG;

char *binome = "Astar"; // student names here

void computeCell(
        int x,
        int y,
        int sourcex,
        int sourcey,
        int destinationx,
        int destinationy,
        int origin_x,
        int origin_y,
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
        int destination_x,
        int destination_y,
        int mapxsize,
        int mapysize,
        int heuristic_ptr2[mapysize][mapxsize],
        int calculated_ptr2[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int chosen[mapysize][mapxsize]
);

char *color(int x, int y, int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]);

void displayGrid(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize],
                 int chosen[mapysize][mapxsize]);

action
calculate_direction(int x, int y, int next_x, int next_y, action last_action, int mapxsize, int mapysize, char **map);

int distance(int source_x, int source_y, int destination_x, int destination_y);

action random_not_killing(int x, int y, action last_action, enum actions planned_action, int mapxsize, int mapysize,
                          char **map);

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
                map[search_y][search_x] == WALL) {
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

    int x = s->x;
    int y = s->y;

    computeCell(x, y, x, y, apple_x, apple_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated);
    displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    calculated[y][x] = 1;
    cost[y][x] = 1;
    heuristic[y][x] = 1;

    chosen[apple_y][apple_x] = 1;

    int next_x = x;
    int next_y = y;

    foundPath(apple_x, apple_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen);
    displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    if (DEBUG)
        printf("NEXT: %d %d\n", next_x, next_y);

    if (next_x == x && next_y == y) {
        return random_not_killing(x, y, last_action, last_action, mapxsize, mapysize, map);
    }

    return calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);

    //computeCell(apple_x, apple_y, apple_x, apple_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated);

    /*calculated[y][x] = 1;
    heuristic[y][x] = 0;

    calculated[apple_y][apple_x] = 1;
    heuristic[apple_y][apple_x] = 1;

    int next_x = x;
    int next_y = y;

    //displayGrid(mapxsize, mapysize, calculated, cost, chosen);
    displayGrid(mapxsize, mapysize, heuristic, cost, chosen);



    //foundPath(x, y, apple_x, apple_y, mapxsize, mapysize, heuristic, calculated, &next_x, &next_y, chosen);
    foundPath(apple_x, apple_y, x, y, mapxsize, mapysize, heuristic, calculated, &next_x, &next_y, chosen);

    displayGrid(mapxsize, mapysize, heuristic, cost, chosen);
    printf("NEXT: %d %d\n", next_x, next_y);

    //displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    if (next_x == x && next_y == y) {
        return random_not_killing(x, y, last_action, last_action, mapxsize, mapysize, map);
    }

    return calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);
    return -1;*/
}

void computeCell(
        int x,
        int y,
        int sourcex,
        int sourcey,
        int destinationx,
        int destinationy,
        int origin_x,
        int origin_y,
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
                currentCost + 1 +
                calculWeight(x - 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x - 1, y, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (y > 0 && cost[y - 1][x] == 0) {
        cost[y - 1][x] = currentCost + 1;
        heuristic[y - 1][x] =
                currentCost + 1 +
                calculWeight(x, y - 1, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x, y - 1, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (x < mapxsize - 1 && cost[y][x + 1] == 0) {
        cost[y][x + 1] = currentCost + 1;
        heuristic[y][x + 1] =
                currentCost + 1 +
                calculWeight(x + 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x + 1, y, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (y < mapysize - 1 && cost[y + 1][x] == 0) {
        cost[y + 1][x] = currentCost + 1;
        heuristic[y + 1][x] =
                currentCost + 1 +
                calculWeight(x, y + 1, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x, y + 1, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
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

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy, origin_x, origin_y, currentCost + 1, mapxsize, mapysize, cost,
                heuristic, calculated);
}

void foundPath(
        int x,
        int y,
        int destination_x,
        int destination_y,
        int mapxsize,
        int mapysize,
        int heuristic_ptr2[mapysize][mapxsize],
        int calculated_ptr2[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int chosen[mapysize][mapxsize]
) {
    if (x == destination_x && y == destination_y) {
        if (DEBUG)
            printf("Arrivé !");
        return;
    }

    if (DEBUG) {
        printf("Destination X:%d Y:%d\n", destination_x, destination_y);
        printf("Processing X:%d Y:%d\n", x, y);
    }

    int lessx = 0;
    int lessy = 0;
    int lessweight = 999999999;
    int currentWeight;

    if (DEBUG)
        printf("X:%d chosen[y][x - 1]:%d calculated_ptr2[y][x - 1]:%d\n", x, chosen[y][x - 1],
               calculated_ptr2[y][x - 1]);
    if (x > 0 && chosen[y][x - 1] == 0) {
        currentWeight = heuristic_ptr2[y][x - 1];

        if (currentWeight > 0 && currentWeight <= lessweight) {
            lessweight = currentWeight;
            lessx = x - 1;
            lessy = y;
        }
    }

    if (DEBUG)
        printf("X:%d chosen[y][x + 1]:%d calculated_ptr2[y][x + 1]:%d\n", x, chosen[y][x + 1],
               calculated_ptr2[y][x + 1]);
    if (x < mapxsize - 1 && chosen[y][x + 1] == 0) {
        currentWeight = heuristic_ptr2[y][x + 1];

        if (currentWeight > 0 && currentWeight <= lessweight) {
            lessweight = currentWeight;
            lessx = x + 1;
            lessy = y;
        }
    }

    if (DEBUG)
        printf("YX:%d chosen[y - 1][x]:%d calculated_ptr2[y - 1][x]:%d\n", y, chosen[y - 1][x],
               calculated_ptr2[y - 1][x]);
    if (y > 0 && chosen[y - 1][x] == 0) {
        currentWeight = heuristic_ptr2[y - 1][x];

        if (currentWeight > 0 && currentWeight <= lessweight) {
            lessweight = currentWeight;
            lessx = x;
            lessy = y - 1;
        }
    }

    if (DEBUG)
        printf("Y:%d chosen[y + 1][x]:%d calculated_ptr2[y + 1][x]:%d\n", y, chosen[y + 1][x],
               calculated_ptr2[y + 1][x]);
    if (y < mapysize - 1 && chosen[y + 1][x] == 0) {
        currentWeight = heuristic_ptr2[y + 1][x];

        if (currentWeight > 0 && currentWeight <= lessweight) {
            lessweight = currentWeight;
            lessx = x;
            lessy = y + 1;
        }
    }

    if (lessweight == 999999999) { //impossible
        if (DEBUG)
            printf("Pas de chemin\n");
        return;
    }

    chosen[lessy][lessx] = 1;

    displayGrid(mapxsize, mapysize, heuristic_ptr2, cost, chosen);

    if (lessx == destination_x && lessy == destination_y) {
        //chosen[lessy][lessx] = 1;

        //printf("Less is X:%d Y:%d\n", lessx, lessy);

        //displayGrid(mapxsize, mapysize, chosen, chosen, chosen);

        if (DEBUG)
            printf("Found path !");

        *next_x = x;
        *next_y = y;
        return;
    }

    foundPath(lessx, lessy, destination_x, destination_y, mapxsize, mapysize, heuristic_ptr2, calculated_ptr2, cost,
              next_x,
              next_y, chosen);
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

action
calculate_direction(int x, int y, int next_x, int next_y, action last_action, int mapxsize, int mapysize, char **map) {
    if (x == next_x) {
        if (y > next_y) {
            if (last_action == SOUTH) {
                return random_not_killing(x, y, last_action, NORTH, mapxsize, mapysize, map);
            }
            return NORTH;
        } else {
            if (last_action == NORTH) {
                return random_not_killing(x, y, last_action, SOUTH, mapxsize, mapysize, map);
            }
            return SOUTH;
        }
    } else if (x > next_x) {
        if (last_action == EAST) {
            return random_not_killing(x, y, last_action, WEST, mapxsize, mapysize, map);
        }
        return WEST;
    } else {
        if (last_action == WEST) {
            return random_not_killing(x, y, last_action, EAST, mapxsize, mapysize, map);
        }
        return EAST;
    }
}

action random_not_killing(int x, int y, action last_action, enum actions planned_action, int mapxsize, int mapysize,
                          char **map) {
    action action;
    bool ok = false;

    if (DEBUG)
        printf("Random\n");

    do {
        action = rand() % 4;

        switch (action) {
            case NORTH:
                if (map[y - 1][x] != WALL
                    && map[y - 1][x] != SNAKE_BODY
                    && map[y - 1][x] != SNAKE_TAIL
                    && last_action != SOUTH)
                    ok = true;
                break;
            case EAST:
                if (map[y][x + 1] != WALL
                    && map[y][x + 1] != SNAKE_BODY
                    && map[y][x + 1] != SNAKE_TAIL
                    && last_action != WEST)
                    ok = true;
                break;
            case SOUTH:
                if (map[y + 1][x] != WALL
                    && map[y + 1][x] != SNAKE_BODY
                    && map[y + 1][x] != SNAKE_TAIL
                    && last_action != NORTH)
                    ok = true;
                break;
            case WEST:
                if (map[y][x - 1] != WALL
                    && map[y][x - 1] != SNAKE_BODY
                    && map[y][x - 1] != SNAKE_TAIL
                    && last_action != EAST)
                    ok = true;
                break;
        }
    } while (!ok);

    return action;
}

char *color(int x, int y, int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize]) {
    if (chosen[y][x] == 1) {
        return "\033[0;36m";
    }

    switch (cost[y][x]) {
        case 0:
            return "\033[0m";
        case -1:
            return "\033[0;34m";
    }

    //if (chosen[y][x] == 0)
    return "\033[0;32m";
}

void displayGrid(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize],
                 int chosen[mapysize][mapxsize]) {
    if (!DEBUG)
        return;

    printf("\n");
    for (int y = 0; y < mapysize + 1; y++) {
        for (int x = 0; x < mapxsize + 1; x++) {
            if (y == 0) {
                if (x == 0) {
                    printf("\033[0m     \033[0m");
                } else {
                    printf("\033[0m %3d \033[0m", x - 1);
                }
            } else {
                if (x == 0) {
                    printf("\033[0m %3d \033[0m", y - 1);
                } else {
                    printf("%s %3d \033[0m", color(x - 1, y - 1, mapxsize, mapysize, cost, chosen),
                           heuristic[y - 1][x - 1]);
                }
            }
        }
        printf("\n");
    }
    printf("\n");
}
