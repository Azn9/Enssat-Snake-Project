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


struct path_link {
    int x;
    int y;
    struct path_link *next;
};

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
        int calculated[mapysize][mapxsize],
        bool *path_found
);

int getSpace(
        int x,
        int y,
        int mapxsize,
        int mapysize,
        char **map,
        int cost[mapysize][mapxsize],
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
        int chosen[mapysize][mapxsize],
        int last_x,
        int last_y,
        struct path_link *currentPathLink,
        int *path_length
);

char *color(
        int x,
        int y,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize]
);

void displayGrid(
        int mapxsize,
        int mapysize,
        int heuristic[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize]
);

void displayGrid2(
        int mapxsize,
        int mapysize,
        int heuristic[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize]
);

action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y,
        action last_action,
        int mapxsize,
        int mapysize,
        char **map
);

int distance(
        int source_x,
        int source_y,
        int destination_x,
        int destination_y
);

action random_not_killing(
        int x,
        int y,
        action last_action,
        enum actions planned_action,
        int mapxsize,
        int mapysize,
        char **map
);

void can_escape(
        int mapxsize,
        int mapysize,
        char **map,
        snake_list snake,
        int snake_length,
        int x,
        int y,
        int path_length,
        int path_x[path_length],
        int path_y[path_length],
        bool *can_escape,
        int *tail_path_x,
        int *tail_path_y
);

void copy(
        int mapysize,
        int mapxsize,
        int originalTable[mapysize][mapxsize],
        int copyTable[mapysize][mapxsize]
);

void find_longest_path(
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        struct path_link *path_link
);

void expand(int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize], struct path_link *path_link);

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

    int tail_x = 0;
    int tail_y = 0;

    for (int search_y = 0; search_y < mapysize; search_y++) {
        for (int search_x = 0; search_x < mapxsize; search_x++) {
            cost[search_y][search_x] = 0;
            heuristic[search_y][search_x] = 0;
            calculated[search_y][search_x] = 0;
            chosen[search_y][search_x] = 0;

            char map_char = map[search_y][search_x];

            if (map_char == SNAKE_BODY || map_char == SNAKE_HEAD ||
                map_char == WALL) {
                cost[search_y][search_x] = -1;
                heuristic[search_y][search_x] = -1;
                calculated[search_y][search_x] = 1;
            }

            if (map_char == SNAKE_HEAD) {
                calculated[search_y][search_x] = -1;
            }

            if (map_char == BONUS) {
                apple_x = search_x;
                apple_y = search_y;
            }

            if (map_char == SNAKE_TAIL) {
                tail_x = search_x;
                tail_y = search_y;
            }
        }
    }

    int cost_empty[mapysize][mapxsize];
    int heuristic_empty[mapysize][mapxsize];
    int calculated_empty[mapysize][mapxsize];
    int chosen_empty[mapysize][mapxsize];

    copy(mapysize, mapxsize, cost, cost_empty);
    copy(mapysize, mapxsize, heuristic, heuristic_empty);
    copy(mapysize, mapxsize, calculated, calculated_empty);
    copy(mapysize, mapxsize, chosen, chosen_empty);

    int x = s->x;
    int y = s->y;

    bool path_found = false;

    computeCell(x, y, x, y, apple_x, apple_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    if (DEBUG)
        displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    int next_x = x;
    int next_y = y;

    if (path_found) {
        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        chosen[apple_y][apple_x] = 1;

        struct path_link *pathLink = malloc(sizeof(struct path_link));
        pathLink->x = apple_x;
        pathLink->y = apple_y;
        pathLink->next = NULL;
        int path_length = 0;

        foundPath(apple_x, apple_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen,
                  apple_x, apple_y, pathLink, &path_length);


        displayGrid2(mapxsize, mapysize, heuristic, cost, chosen);

        struct path_link * currentPathLink = pathLink;
        printf("OP:\n");
        while (currentPathLink->next != NULL) {
            printf("%d:%d -> ", currentPathLink->x, currentPathLink->y);

            currentPathLink = currentPathLink->next;
        }
        printf("POMME\n");

        //TODO Found longest path
        find_longest_path(mapxsize, mapysize, cost, chosen, pathLink);

        displayGrid2(mapxsize, mapysize, heuristic, cost, chosen);

        currentPathLink = pathLink;
        printf("NP:\n");
        while (currentPathLink->next != NULL) {
            printf("%d:%d -> ", currentPathLink->x, currentPathLink->y);

            currentPathLink = currentPathLink->next;
        }
        printf("POMME\n");








        displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

        int path_x[path_length];
        int path_y[path_length];

        for (int i = 0; i < path_length; i++) {
            path_x[i] = 0;
            path_y[i] = 0;
        }

        int index = 0;
        //struct path_link *
                currentPathLink = pathLink;
        while (currentPathLink->next != NULL) {
            if (index >= path_length) {
                fprintf(stderr, "ERREUR !\n");
            }

            path_x[index] = currentPathLink->x;
            path_y[index] = currentPathLink->y;

            index++;

            currentPathLink = currentPathLink->next;
        }

        if (DEBUG)
            fprintf(stderr, "Path length : %d\n", path_length);

        if (DEBUG)
            for (int i = 0; i < path_length; i++) {
                fprintf(stderr, "%d:%d -> ", path_x[i], path_y[i]);
            }

        if (DEBUG)
            fprintf(stderr, "POMME\n");

        if (DEBUG)
            fprintf(stderr, "NEXT: %d %d\n", next_x, next_y);

        currentPathLink = pathLink;
        while (currentPathLink->next != NULL) {
            struct path_link *next_ptr = currentPathLink->next;
            free(currentPathLink);
            currentPathLink = next_ptr;
        }
        free(currentPathLink);

        int snake_length = 1;
        struct snake_link *currentSnakeLink = s;
        while (currentSnakeLink->next != NULL) {
            snake_length++;
            currentSnakeLink = currentSnakeLink->next;
        }

        if (DEBUG)
            fprintf(stderr, "Snake length : %d\n", snake_length);

        bool can_escape_value = false;
        int next_tail_x = x;
        int next_tail_y = y;

        can_escape(mapxsize, mapysize, map, s, snake_length, apple_x, apple_y, path_length, path_x, path_y,
                   &can_escape_value, &next_tail_x, &next_tail_y);

        if (!(next_x == x && next_y == y)) {
            if (!can_escape_value) {
                //TODO recalculer path vers vraie queue

                if (DEBUG)
                    fprintf(stderr, "Follow tail 1\n");

                //Reset
                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                path_found = false;
                computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated,
                            &path_found);

                calculated[y][x] = 1;
                cost[y][x] = 1;
                heuristic[y][x] = 1;

                chosen[tail_x][tail_y] = 1;

                next_x = x;
                next_y = y;

                pathLink = malloc(sizeof(struct path_link));
                pathLink->x = tail_x;
                pathLink->y = tail_y;
                pathLink->next = NULL;
                path_length = 0;

                foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y,
                          chosen,
                          tail_x, tail_y, pathLink, &path_length);

                if (next_x == x && next_y == y) {
                    if (DEBUG)
                        fprintf(stderr, "ON VA RESTER COINCE !!1\n");
                }
            } else {
                return calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);
            }
        } else {
            if (can_escape_value) {
                if (DEBUG)
                    fprintf(stderr, "Follow tail 2\n");

                //Reset
                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                path_found = false;
                computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated,
                            &path_found);

                calculated[y][x] = 1;
                cost[y][x] = 1;
                heuristic[y][x] = 1;

                chosen[tail_x][tail_y] = 1;

                next_x = x;
                next_y = y;

                pathLink = malloc(sizeof(struct path_link));
                pathLink->x = tail_x;
                pathLink->y = tail_y;
                pathLink->next = NULL;
                path_length = 0;

                foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y,
                          chosen,
                          tail_x, tail_y, pathLink, &path_length);

                struct path_link * currentPathLink = pathLink;
                while (currentPathLink->next != NULL) {
                    printf("%d:%d -> ", currentPathLink->x, currentPathLink->y);

                    currentPathLink = currentPathLink->next;
                }

                //TODO Found longest path
                find_longest_path(mapxsize, mapysize, cost, chosen, pathLink);

                currentPathLink = pathLink;
                while (currentPathLink->next != NULL) {
                    printf("%d:%d -> ", currentPathLink->x, currentPathLink->y);

                    currentPathLink = currentPathLink->next;
                }

                if (next_x == x && next_y == y) {
                    if (DEBUG)
                        fprintf(stderr, "ON VA RESTER COINCE !!2\n");
                }
            } else {
                return random_not_killing(x, y, last_action, last_action, mapxsize, mapysize, map);
            }
        }
    } else {
        if (DEBUG)
            printf("NPF\n");

        copy(mapysize, mapxsize, cost_empty, cost);
        copy(mapysize, mapxsize, heuristic_empty, heuristic);
        copy(mapysize, mapxsize, calculated_empty, calculated);
        copy(mapysize, mapxsize, chosen_empty, chosen);

        path_found = false;
        computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

        if (path_found) {
            if (DEBUG)
                fprintf(stderr, "Follow tail 3\n");

            calculated[y][x] = 1;
            cost[y][x] = 1;
            heuristic[y][x] = 1;

            chosen[tail_x][tail_y] = 1;

            next_x = x;
            next_y = y;

            struct path_link *pathLink = malloc(sizeof(struct path_link));
            pathLink->x = tail_x;
            pathLink->y = tail_y;
            pathLink->next = NULL;
            int path_length = 0;

            foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y,
                      chosen,
                      tail_x, tail_y, pathLink, &path_length);

            return calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);
        } else {
            return random_not_killing(x, y, last_action, last_action, mapxsize, mapysize, map);
        }
    }

    if (next_x == x && next_y == y) {
        return random_not_killing(x, y, last_action, last_action, mapxsize, mapysize, map);
    }

    return calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);
}

void find_longest_path(int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize], struct path_link *path_link) {
    struct path_link *currentPathLink = path_link;
    while (currentPathLink->next != NULL) {
        expand(mapxsize, mapysize, cost, chosen, currentPathLink);
        currentPathLink = currentPathLink->next;
    }
}

void expand(int mapxsize, int mapysize, int cost[mapysize][mapxsize], int chosen[mapysize][mapxsize], struct path_link *path_link) {
    int x = path_link->x;
    int y = path_link->y;

    struct path_link * next_path_link = path_link->next;
    int next_x = next_path_link->x;
    int next_y = next_path_link->y;

    if (x == next_x) { //colonne
        int test_x = x;
        while (test_x > 0 && cost[y][test_x] >= 0 && chosen[y][test_x] == 0 && cost[next_y][test_x] >= 0 && chosen[next_y][test_x] == 0) {
            test_x--;
        }

        int test_x_upper = x;
        while (test_x_upper < mapxsize && cost[y][test_x_upper] >= 0 && chosen[y][test_x_upper] == 0 && cost[next_y][test_x_upper] >= 0 && chosen[next_y][test_x_upper] == 0) {
            test_x_upper++;
        }

        int diff_below = x - test_x;
        int diff_upper = test_x_upper - x;

        if (diff_below < diff_upper) {
            struct path_link *pathLink = path_link;
            struct path_link *end = next_path_link;
            for (int i = 1; i < diff_upper; i++) {
                struct path_link * newpathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x + i;
                newpathLink->y = y;
                newpathLink->next = NULL;

                pathLink->next = newpathLink;
                pathLink = newpathLink;

                cost[y][x+i] = -2;
                cost[next_y][x+i] = -2;

                struct path_link * newpendathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x + (diff_upper - i);
                newpathLink->y = next_y;
                newpathLink->next = end;
                end = newpendathLink;
            }
            pathLink->next = end;
        } else {
            struct path_link *pathLink = path_link;
            struct path_link *end = next_path_link;
            for (int i = 1; i < diff_below; i++) {
                struct path_link * newpathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x - i;
                newpathLink->y = y;
                newpathLink->next = NULL;

                pathLink->next = newpathLink;
                pathLink = newpathLink;

                cost[y][x-i] = -2;
                cost[next_y][x-i] = -2;

                struct path_link * newpendathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x - (diff_below - i);
                newpathLink->y = next_y;
                newpathLink->next = end;
                end = newpendathLink;
            }
            pathLink->next = end;
        }
    } else { //ligne
        int test_y = y;
        while (test_y > 0 && cost[test_y][x] >= 0 && chosen[test_y][x] == 0 && cost[test_y][next_x] >= 0 && chosen[test_y][next_x] == 0) {
            test_y--;
        }

        int test_y_upper = y;
        while (test_y_upper < mapxsize && cost[test_y_upper][x] >= 0 && chosen[test_y_upper][x] == 0 && cost[test_y_upper][next_x] >= 0 && chosen[test_y_upper][next_x] == 0) {
            test_y_upper++;
        }

        int diff_below = y - test_y;
        int diff_upper = test_y_upper - y;

        if (diff_below < diff_upper) {
            struct path_link *pathLink = path_link;
            struct path_link *end = next_path_link;
            for (int i = 1; i < diff_upper; i++) {
                struct path_link * newpathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x;
                newpathLink->y = y + i;
                newpathLink->next = NULL;

                pathLink->next = newpathLink;
                pathLink = newpathLink;

                cost[y+i][x] = -2;
                cost[y+i][next_x] = -2;

                struct path_link * newpendathLink = malloc(sizeof(struct path_link));
                newpathLink->x = next_x;
                newpathLink->y = y + (diff_upper - i);
                newpathLink->next = end;
                end = newpendathLink;
            }
            pathLink->next = end;
        } else {
            struct path_link *pathLink = path_link;
            struct path_link *end = next_path_link;
            for (int i = 1; i < diff_below; i++) {
                struct path_link * newpathLink = malloc(sizeof(struct path_link));
                newpathLink->x = x;
                newpathLink->y = y - i;
                newpathLink->next = NULL;

                pathLink->next = newpathLink;
                pathLink = newpathLink;

                cost[y-i][x] = -2;
                cost[y-i][next_x] = -2;

                struct path_link * newpendathLink = malloc(sizeof(struct path_link));
                newpathLink->x = next_x;
                newpathLink->y = y - (diff_below - i);
                newpathLink->next = end;
                end = newpendathLink;
            }
            pathLink->next = end;
        }
    }
}

void copy(int mapysize, int mapxsize, int originalTable[mapysize][mapxsize], int copyTable[mapysize][mapxsize]) {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapxsize; x++) {
            copyTable[y][x] = originalTable[y][x];
        }
    }
}

void can_escape(
        int mapxsize,
        int mapysize,
        char **map,
        snake_list snake,
        int snake_lenght,
        int x,
        int y,
        int path_length,
        int path_x[path_length],
        int path_y[path_length],
        bool *can_escape,
        int *tail_path_x,
        int *tail_path_y
) {
    if (DEBUG)
        fprintf(stderr, "Call canescape !\n");

    int new_snake_length = snake_lenght - path_length;
    int min_length = path_length;
    if (snake_lenght < path_length) {
        min_length = snake_lenght;
    }

    //deplacer le snake
    int cost[mapysize][mapxsize];
    int heuristic[mapysize][mapxsize];
    int calculated[mapysize][mapxsize];
    int chosen[mapysize][mapxsize];

    for (int search_y = 0; search_y < mapysize; search_y++) {
        for (int search_x = 0; search_x < mapxsize; search_x++) {
            cost[search_y][search_x] = 0;
            heuristic[search_y][search_x] = 0;
            calculated[search_y][search_x] = 0;
            chosen[search_y][search_x] = 0;

            if (map[search_y][search_x] == WALL) {
                cost[search_y][search_x] = -1;
                heuristic[search_y][search_x] = -1;
                calculated[search_y][search_x] = 1;
            }
        }
    }

    for (int i = 0; i < min_length; i++) {
        int current_y = path_y[i];
        int current_x = path_x[i];

        cost[current_y][current_x] = -1;
        heuristic[current_y][current_x] = -1;
        calculated[current_y][current_x] = -1;
    }

    int index = 0;
    struct snake_link *current_snake_link = snake;
    while (index < new_snake_length && current_snake_link->next != NULL) {
        int current_x = current_snake_link->x;
        int current_y = current_snake_link->y;

        if (current_snake_link->c == SNAKE_BODY) {
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = 1;
        }

        if (current_snake_link->c == SNAKE_HEAD) {
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = -1;
        }

        index++;
        current_snake_link = current_snake_link->next;
    }

    if (DEBUG)
        displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    //tester si y'a un path vers la nouvelle queue

    int tail_x = current_snake_link->x;
    int tail_y = current_snake_link->y;

    if (DEBUG)
        fprintf(stderr, "Tail : %d:%d\n", tail_x, tail_y);

    bool path_found = false;
    computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    if (DEBUG)
        displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

    int next_x = x;
    int next_y = y;

    if (path_found) {
        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        chosen[tail_y][tail_x] = 1;

        struct path_link *pathLink = malloc(sizeof(struct path_link));
        pathLink->x = tail_x;
        pathLink->y = tail_y;
        pathLink->next = NULL;
        int path_length_temp = 0;

        foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen,
                  tail_x, tail_y, pathLink, &path_length_temp);

        struct path_link *currentPathLink = pathLink;
        while (currentPathLink->next != NULL) {
            struct path_link *next_ptr = currentPathLink->next;
            free(currentPathLink);
            currentPathLink = next_ptr;
        }
        free(currentPathLink);

        if (DEBUG) {
            fprintf(stderr, "NextTail : %d:%d\n", next_x, next_y);
            fprintf(stderr, "Current : %d:%d\n", x, y);
        }

        *can_escape = true;
        *tail_path_x = next_x;
        *tail_path_y = next_y;
    }
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
        int calculated[mapysize][mapxsize],
        bool *path_found
) {
    if (x < 0 || y < 0 || x > mapxsize - 1 || y > mapysize - 1) {
        return;
    }

    if (calculated[y][x] > 0) {
        //fprintf(stderr, "\033[0;32mCalculated !\033[0m\n");
        return;
    }

    if (cost[destinationy][destinationx] > 0) {
        //fprintf(stderr, "\033[0;32mWeight found !\033[0m\n");
        *path_found = true;
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

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy, origin_x, origin_y, currentCost + 1,
                mapxsize, mapysize, cost,
                heuristic, calculated, path_found);
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
        int chosen[mapysize][mapxsize],
        int last_x,
        int last_y,
        struct path_link *currentPathLink,
        int *path_length
) {
    if (x == destination_x && y == destination_y) {
        if (DEBUG)
            fprintf(stderr, "Arrivé !");
        return;
    }

    if (DEBUG) {
        fprintf(stderr, "Destination X:%d Y:%d\n", destination_x, destination_y);
        fprintf(stderr, "Processing X:%d Y:%d\n", x, y);
    }

    int lessx = 0;
    int lessy = 0;
    int lessweight = 999999999;
    int currentWeight;

    if (DEBUG)
        fprintf(stderr, "X:%d chosen[y][x - 1]:%d calculated_ptr2[y][x - 1]:%d\n", x, chosen[y][x - 1],
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
        fprintf(stderr, "X:%d chosen[y][x + 1]:%d calculated_ptr2[y][x + 1]:%d\n", x, chosen[y][x + 1],
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
        fprintf(stderr, "YX:%d chosen[y - 1][x]:%d calculated_ptr2[y - 1][x]:%d\n", y, chosen[y - 1][x],
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
        fprintf(stderr, "Y:%d chosen[y + 1][x]:%d calculated_ptr2[y + 1][x]:%d\n", y, chosen[y + 1][x],
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
            fprintf(stderr, "Pas de chemin\n");

        if (last_x == x && last_y == y) {
            return;
        }

        foundPath(last_x, last_y, destination_x, destination_y, mapxsize, mapysize, heuristic_ptr2, calculated_ptr2,
                  cost,
                  next_x,
                  next_y, chosen, last_x, last_y, currentPathLink, path_length);
        return;
    }

    *path_length = *(path_length) + 1;
    struct path_link *pathLink = malloc(sizeof(struct path_link));
    pathLink->x = lessx;
    pathLink->y = lessy;
    pathLink->next = NULL;
    (*currentPathLink).next = pathLink;

    chosen[lessy][lessx] = 1;

    displayGrid(mapxsize, mapysize, heuristic_ptr2, cost, chosen);

    if (lessx == destination_x && lessy == destination_y) {
        //chosen[lessy][lessx] = 1;

        //fprintf(stderr, "Less is X:%d Y:%d\n", lessx, lessy);

        //displayGrid(mapxsize, mapysize, chosen, chosen, chosen);

        if (DEBUG)
            fprintf(stderr, "Found path !\n");

        *next_x = x;
        *next_y = y;
        return;
    }

    foundPath(lessx, lessy, destination_x, destination_y, mapxsize, mapysize, heuristic_ptr2, calculated_ptr2, cost,
              next_x,
              next_y, chosen, x, y, pathLink, path_length);
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
        fprintf(stderr, "Random\n");

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
        case -2:
            return "\033[0;35m";
    }

    //if (chosen[y][x] == 0)
    return "\033[0;32m";
}

void displayGrid(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize],
                 int chosen[mapysize][mapxsize]) {
    if (!DEBUG)
        return;

    fprintf(stderr, "\n");
    for (int y = 0; y < mapysize + 1; y++) {
        for (int x = 0; x < mapxsize + 1; x++) {
            if (y == 0) {
                if (x == 0) {
                    fprintf(stderr, "\033[0m     \033[0m");
                } else {
                    fprintf(stderr, "\033[0m %3d \033[0m", x - 1);
                }
            } else {
                if (x == 0) {
                    fprintf(stderr, "\033[0m %3d \033[0m", y - 1);
                } else {
                    fprintf(stderr, "%s %3d \033[0m", color(x - 1, y - 1, mapxsize, mapysize, cost, chosen),
                            heuristic[y - 1][x - 1]);
                }
            }
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}

void displayGrid2(int mapxsize, int mapysize, int heuristic[mapysize][mapxsize], int cost[mapysize][mapxsize],
                 int chosen[mapysize][mapxsize]) {

    fprintf(stderr, "\n");
    for (int y = 0; y < mapysize + 1; y++) {
        for (int x = 0; x < mapxsize + 1; x++) {
            if (y == 0) {
                if (x == 0) {
                    fprintf(stderr, "\033[0m     \033[0m");
                } else {
                    fprintf(stderr, "\033[0m %3d \033[0m", x - 1);
                }
            } else {
                if (x == 0) {
                    fprintf(stderr, "\033[0m %3d \033[0m", y - 1);
                } else {
                    fprintf(stderr, "%s %3d \033[0m", color(x - 1, y - 1, mapxsize, mapysize, cost, chosen),
                            heuristic[y - 1][x - 1]);
                }
            }
        }
        fprintf(stderr, "\n");
    }
    fprintf(stderr, "\n");
}