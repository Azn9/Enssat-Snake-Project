/*
 * Naming, formatting and code style based on https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html
 */


// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand, abs
#include <stdio.h> // printf
#include <unistd.h>

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

char *binome = "Azid Mouad & Joly Axel"; // student names here


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

action validate_action_or_random(
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

void find_farest(
        int x,
        int y,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        struct path_link *pathLink,
        int *path_length
);

void initialize_map(
        int mapxsize,
        int mapysize,
        char **map,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *apple_x,
        int *apple_y,
        int *tail_x,
        int *tail_y
);

void follow_tail(
        int x,
        int y,
        int tail_x,
        int tail_y,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        bool *dound_tail_path,
        int *snake_index,
        snake_list snake
);

void calculate_path(
        struct path_link *pathLink,
        int path_length,
        int path_x[path_length],
        int path_y[path_length]
);

void free_all_path(struct path_link *pathLink);





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
    int cost_empty[mapysize][mapxsize];
    int heuristic_empty[mapysize][mapxsize];
    int calculated_empty[mapysize][mapxsize];
    int chosen_empty[mapysize][mapxsize];

    int cost[mapysize][mapxsize];
    int heuristic[mapysize][mapxsize];
    int calculated[mapysize][mapxsize];
    int chosen[mapysize][mapxsize];

    int apple_x = 0;
    int apple_y = 0;

    int tail_x = 0;
    int tail_y = 0;

    int x = s->x;
    int y = s->y;

    bool path_found = false;

    initialize_map(mapxsize, mapysize, map, cost, heuristic, calculated, chosen, &apple_x, &apple_y, &tail_x, &tail_y);

    copy(mapysize, mapxsize, cost, cost_empty);
    copy(mapysize, mapxsize, heuristic, heuristic_empty);
    copy(mapysize, mapxsize, calculated, calculated_empty);
    copy(mapysize, mapxsize, chosen, chosen_empty);

    computeCell(x, y, x, y, apple_x, apple_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    int next_x = x;
    int next_y = y;

    struct path_link *pathLink = malloc(sizeof(struct path_link));
    pathLink->x = apple_x;
    pathLink->y = apple_y;
    pathLink->next = NULL;
    int path_length = 0;

    if (path_found) {
        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        chosen[apple_y][apple_x] = 1;

        foundPath(apple_x, apple_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen, apple_x, apple_y, pathLink, &path_length);

        int path_x[path_length];
        int path_y[path_length];

        calculate_path(pathLink, path_length, path_x, path_y);

        bool can_escape_value = false;
        int next_tail_x = x;
        int next_tail_y = y;

        can_escape(mapxsize, mapysize, map, s, apple_x, apple_y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

        if (!(next_x == x && next_y == y)) {
            if (DEBUG)
                printf("Can go to apple\n");

            if (!can_escape_value) {
                if (DEBUG)
                    printf("Can't escape\n");

                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                bool found_tail_path = false;
                int snake_index = 0;

                follow_tail(x, y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, &snake_index, s);
            } else {
                if (DEBUG)
                    printf("Can escape\n");

                action planned_action = calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);

                can_escape_value = false;
                next_tail_x = x;
                next_tail_y = y;

                path_length = 1;
                path_x[0] = next_x;
                path_y[0] = next_y;

                can_escape(mapxsize, mapysize, map, s, next_x, next_y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

                if (can_escape_value) {
                    if (DEBUG)
                        printf("Move allow to follow tail, playing %d\n", planned_action);

                    if (DEBUG)
                        printf("Next move : X:%d Y:%d\n", next_x, next_y);

                    return planned_action;//validate_action_or_random(x, y, last_action, planned_action, mapxsize, mapysize, map);
                } else {
                    if (DEBUG)
                        printf("Move don't allow to follow tail, following tail\n");

                    copy(mapysize, mapxsize, cost_empty, cost);
                    copy(mapysize, mapxsize, heuristic_empty, heuristic);
                    copy(mapysize, mapxsize, calculated_empty, calculated);
                    copy(mapysize, mapxsize, chosen_empty, chosen);

                    bool found_tail_path = false;
                    int snake_index = 0;

                    follow_tail(x, y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, &snake_index, s);

                    if (DEBUG)
                        printf("Next move : X:%d Y:%d\n", next_x, next_y);
                }
            }
        } else {
            if (DEBUG)
                printf("Can't go to apple !\n");

            displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

            can_escape_value = false;

            copy(mapysize, mapxsize, cost_empty, cost);
            copy(mapysize, mapxsize, heuristic_empty, heuristic);
            copy(mapysize, mapxsize, calculated_empty, calculated);
            copy(mapysize, mapxsize, chosen_empty, chosen);

            computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

            calculated[y][x] = 1;
            cost[y][x] = 1;
            heuristic[y][x] = 1;

            chosen[tail_y][tail_x] = 1;

            foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen, tail_x, tail_y, pathLink, &path_length);

            struct path_link *pathLink = malloc(sizeof(struct path_link));
            pathLink->x = apple_x;
            pathLink->y = apple_y;
            pathLink->next = NULL;
            int path_length = 0;

            int path_x[path_length];
            int path_y[path_length];

            calculate_path(pathLink, path_length, path_x, path_y);

            can_escape(mapxsize, mapysize, map, s, x, y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

            if (can_escape_value) {
                if (DEBUG)
                    printf("Can follow tail 1\n");

                //Reset
                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                bool found_tail_path = false;
                int snake_index = 0;

                follow_tail(x, y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, &snake_index, s);

                if (DEBUG)
                    printf("Next move : X:%d Y:%d\n", next_x, next_y);
            } else {
                if (DEBUG)
                    printf("Can't follow tail ! 1\n");

                displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

                find_farest(x, y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, pathLink, &path_length);

                if (DEBUG)
                    printf("Next move : X:%d Y:%d\n", next_x, next_y);
            }
        }

        if (next_x == x && next_y == y) {
            if (DEBUG)
                printf("No next move\n");

            can_escape_value = false;
            next_tail_x = x;
            next_tail_y = y;

            can_escape(mapxsize, mapysize, map, s, x, y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

            if (can_escape_value) {
                if (DEBUG)
                    printf("Can follow tail 2\n");

                //Reset
                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                bool found_tail_path = false;
                int snake_index = 0;

                follow_tail(x, y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, &snake_index, s);

                if (DEBUG)
                    printf("Can follow tail 3\n");
            } else {
                if (DEBUG)
                    printf("Can't follow tail 2\n");

                find_farest(x, y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, pathLink, &path_length);
            }
        }
    } else {
        copy(mapysize, mapxsize, cost_empty, cost);
        copy(mapysize, mapxsize, heuristic_empty, heuristic);
        copy(mapysize, mapxsize, calculated_empty, calculated);
        copy(mapysize, mapxsize, chosen_empty, chosen);

        bool found_tail_path = false;
        int snake_index = 0;

        follow_tail(x, y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, &snake_index, s);
    }

    if (DEBUG)
        printf("Next action : %d %d\n", next_x, next_y);

    action planned_action = calculate_direction(x, y, next_x, next_y, last_action, mapxsize, mapysize, map);
    return validate_action_or_random(x, y, last_action, planned_action, mapxsize, mapysize, map);
}

void calculate_path(
        struct path_link *pathLink,
        int path_length,
        int path_x[path_length],
        int path_y[path_length]
) {
    for (int i = 0; i < path_length; i++) {
        path_x[i] = 0;
        path_y[i] = 0;
    }

    int index = 0;
    struct path_link *currentPathLink = pathLink;
    while (index < path_length && currentPathLink->next != NULL) {
        path_x[index] = currentPathLink->x;
        path_y[index] = currentPathLink->y;

        index++;

        currentPathLink = currentPathLink->next;
    }

    currentPathLink = pathLink;
    while (currentPathLink->next != NULL) {
        struct path_link *next_ptr = currentPathLink->next;
        free(currentPathLink);
        currentPathLink = next_ptr;
    }
    free(currentPathLink);
}

void follow_tail(
        int x,
        int y,
        int tail_x,
        int tail_y,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        bool *dound_tail_path,
        int *snake_index,
        snake_list snake
) {
    bool path_found = false;

    int snake_length = 1;
    struct snake_link *currentSnakeLink = snake;
    while (currentSnakeLink->next != NULL) {
        snake_length++;
        currentSnakeLink = currentSnakeLink->next;
    }

    if (DEBUG)
        printf("Ft_SL : %d\n", snake_length);

    /*int index = 0;
    currentSnakeLink = snake;
    while (index < snake_length && currentSnakeLink->next != NULL) {
        index++;
        currentSnakeLink = currentSnakeLink->next;
    }*/

    path_found = false;
    computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    if (path_found) {
        if (DEBUG)
            printf("Found path to tail\n");

        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        chosen[tail_y][tail_x] = 1;

        struct path_link *pathLink = malloc(sizeof(struct path_link));
        pathLink->x = tail_x;
        pathLink->y = tail_y;
        pathLink->next = NULL;
        int path_length = 0;

        foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, next_x, next_y, chosen, tail_x, tail_y, pathLink, &path_length);
        free_all_path(pathLink);
    }
}

void initialize_map(
        int mapxsize,
        int mapysize,
        char **map,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *apple_x,
        int *apple_y,
        int *tail_x,
        int *tail_y
) {
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
                *apple_x = search_x;
                *apple_y = search_y;
            }

            if (map_char == SNAKE_TAIL) {
                *tail_x = search_x;
                *tail_y = search_y;
            }
        }
    }
}

void find_farest(
        int x,
        int y,
        int mapxsize,
        int mapysize,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        struct path_link *pathLink,
        int *path_length
) {
    int max_cost = 0;
    int max_x = 0;
    int max_y = 0;

    for (int search_x = 1; search_x < mapxsize - 1; search_x++) {
        for (int search_y = 1; search_y < mapysize - 1; search_y++) {
            int current_cost = cost[search_y][search_x];

            if (max_cost < current_cost) {
                max_cost = current_cost;
                max_x = search_x;
                max_y = search_y;
            }
        }
    }

    foundPath(max_x, max_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, next_x, next_y, chosen, max_x, max_y, pathLink, path_length);
}

void can_escape(
        int mapxsize,
        int mapysize,
        char **map,
        snake_list snake,
        int x,
        int y,
        int path_length,
        int path_x[path_length],
        int path_y[path_length],
        bool *can_escape,
        int *tail_path_x,
        int *tail_path_y
) {
    int snake_length = 1;
    struct snake_link *currentSnakeLink = snake;
    while (currentSnakeLink->next != NULL) {
        snake_length++;
        currentSnakeLink = currentSnakeLink->next;
    }

    if (DEBUG)
        printf("SL: %d\n", snake_length);

    int new_snake_length = snake_length - path_length;
    int min_length = path_length;
    if (snake_length < path_length) {
        min_length = snake_length;
    }

    if (DEBUG)
        printf("PL: %d\n", path_length);

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

        if (DEBUG)
            printf("Locking next path %d %d\n", current_x, current_y);

        if (current_x >= 0 && current_y >= 0 && current_x <= mapxsize - 1 && current_y <= mapysize - 1) {
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = i == 0 ? -1 : 1;
        }
    }

    int index = 0;
    struct snake_link *current_snake_link = snake;
    while (index < new_snake_length && current_snake_link->next != NULL) {
        int current_x = current_snake_link->x;
        int current_y = current_snake_link->y;

        if (current_snake_link->c == SNAKE_BODY) {
            if (DEBUG)
                printf("Locking snake path %d %d\n", current_x, current_y);

            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = 1;
        }

        if (current_snake_link->c == SNAKE_HEAD) {
            if (DEBUG)
                printf("Locking snake path %d %d\n", current_x, current_y);

            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = 1;
        }

        index++;
        current_snake_link = current_snake_link->next;
    }

    //calculated[y][x] = 1;

    if (DEBUG)
        printf("I: %d\n", index);

    int tail_x = current_snake_link->x;
    int tail_y = current_snake_link->y;

    if (DEBUG)
        printf("T: %d %d\n", tail_x, tail_y);

    bool path_found = false;
    computeCell(x, y, x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    int path_distance = distance(x, y, tail_x, tail_y);

    if (DEBUG)
        printf("Path found : %d (%d / %d)\n", path_found, path_length, path_distance);

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

        if (DEBUG)
            printf("ptl : %d\n", path_length_temp);

        foundPath(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, &next_x, &next_y, chosen,
                  tail_x, tail_y, pathLink, &path_length_temp);

        displayGrid(mapxsize, mapysize, heuristic, cost, chosen);

        free_all_path(pathLink);

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
        return;
    }

    if (cost[destinationy][destinationx] > 0) {
        *path_found = true;
        return;
    }

    int currentCost = previousCost;

    if (x > 0 && cost[y][x - 1] == 0) {
        cost[y][x - 1] = currentCost + 10;
        heuristic[y][x - 1] =
                currentCost + 10 +
                calculWeight(x - 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x - 1, y, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (y > 0 && cost[y - 1][x] == 0) {
        cost[y - 1][x] = currentCost + 10;
        heuristic[y - 1][x] =
                currentCost + 10 +
                calculWeight(x, y - 1, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x, y - 1, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (x < mapxsize - 1 && cost[y][x + 1] == 0) {
        cost[y][x + 1] = currentCost + 10;
        heuristic[y][x + 1] =
                currentCost + 10 +
                calculWeight(x + 1, y, destinationx, destinationy, mapxsize, mapysize, calculated, heuristic) +
                calculWeight(x + 1, y, origin_x, origin_y, mapxsize, mapysize, calculated, heuristic);
    }

    if (y < mapysize - 1 && cost[y + 1][x] == 0) {
        cost[y + 1][x] = currentCost + 10;
        heuristic[y + 1][x] =
                currentCost + 10 +
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
                if (currentWeight == lessweight) {
                    if (rand() % 2) {
                        lessweight = currentWeight;
                        lessx = testx;
                        lessy = testy;
                    }
                } else if (currentWeight < lessweight) {
                    lessweight = currentWeight;
                    lessx = testx;
                    lessy = testy;
                }
            }
        }
    }

    computeCell(lessx, lessy, sourcex, sourcey, destinationx, destinationy, origin_x, origin_y, currentCost + 10, mapxsize, mapysize, cost, heuristic, calculated, path_found);
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
        return;
    }

    int lessx = 0;
    int lessy = 0;
    int lessweight = 999999999;
    int currentWeight;

    if (x > 0 && chosen[y][x - 1] == 0) {
        currentWeight = heuristic_ptr2[y][x - 1];

        if (currentWeight > 0) {
            if (currentWeight < lessweight) {
                lessweight = currentWeight;
                lessx = x - 1;
                lessy = y;
            } else if (currentWeight == lessweight) {
                if (rand() % 2) {
                    lessweight = currentWeight;
                    lessx = x - 1;
                    lessy = y;
                }
            }
        }
    }

    if (x < mapxsize - 1 && chosen[y][x + 1] == 0) {
        currentWeight = heuristic_ptr2[y][x + 1];

        if (currentWeight > 0) {
            if (currentWeight < lessweight) {
                lessweight = currentWeight;
                lessx = x + 1;
                lessy = y;
            } else if (currentWeight == lessweight) {
                if (rand() % 2) {
                    lessweight = currentWeight;
                    lessx = x + 1;
                    lessy = y;
                }
            }
        }
    }

    if (y > 0 && chosen[y - 1][x] == 0) {
        currentWeight = heuristic_ptr2[y - 1][x];

        if (currentWeight > 0) {
            if (currentWeight < lessweight) {
                lessweight = currentWeight;
                lessx = x;
                lessy = y - 1;
            } else if (currentWeight == lessweight) {
                if (rand() % 2) {
                    lessweight = currentWeight;
                    lessx = x;
                    lessy = y - 1;
                }
            }
        }
    }

    if (y < mapysize - 1 && chosen[y + 1][x] == 0) {
        currentWeight = heuristic_ptr2[y + 1][x];

        if (currentWeight > 0) {
            if (currentWeight < lessweight) {
                lessweight = currentWeight;
                lessx = x;
                lessy = y + 1;
            } else if (currentWeight == lessweight) {
                if (rand() % 2) {
                    lessweight = currentWeight;
                    lessx = x;
                    lessy = y + 1;
                }
            }
        }
    }

    if (lessweight == 999999999) { //impossible
        if (last_x == x && last_y == y) {
            return;
        }

        foundPath(last_x, last_y, destination_x, destination_y, mapxsize, mapysize, heuristic_ptr2, calculated_ptr2, cost, next_x, next_y, chosen, last_x, last_y, currentPathLink, path_length);
        return;
    }

    *path_length = *(path_length) + 1;
    struct path_link *pathLink = malloc(sizeof(struct path_link));
    pathLink->x = lessx;
    pathLink->y = lessy;
    pathLink->next = NULL;
    (*currentPathLink).next = pathLink;

    chosen[lessy][lessx] = 1;

    if (lessx == destination_x && lessy == destination_y) {
        *next_x = x;
        *next_y = y;
        return;
    }

    foundPath(lessx, lessy, destination_x, destination_y, mapxsize, mapysize, heuristic_ptr2, calculated_ptr2, cost, next_x, next_y, chosen, x, y, pathLink, path_length);
}

action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y,
        action last_action,
        int mapxsize,
        int mapysize,
        char **map
) {
    action planned_action;

    if (x == next_x && y == next_y) {
        return last_action;
    }

    if (x == next_x) {
        if (y > next_y) {
            planned_action = NORTH;
        } else {
            planned_action = SOUTH;
        }
    } else if (x > next_x) {
        planned_action = WEST;
    } else {
        planned_action = EAST;
    }

    return planned_action;
}

action validate_action_or_random(
        int x,
        int y,
        action last_action,
        enum actions planned_action,
        int mapxsize,
        int mapysize,
        char **map
) {
    action action;
    bool ok = false;

    switch (planned_action) {
        case NORTH:
            if (y > 0
                && map[y - 1][x] != WALL
                && map[y - 1][x] != SNAKE_BODY
//                && map[y - 1][x] != SNAKE_TAIL
                && last_action != SOUTH)
                ok = true;
            break;
        case EAST:
            if (x < mapxsize - 1
                && map[y][x + 1] != WALL
                && map[y][x + 1] != SNAKE_BODY
//                && map[y][x + 1] != SNAKE_TAIL
                && last_action != WEST)
                ok = true;
            break;
        case SOUTH:
            if (y < mapysize - 1
                && map[y + 1][x] != WALL
                && map[y + 1][x] != SNAKE_BODY
//                && map[y + 1][x] != SNAKE_TAIL
                && last_action != NORTH)
                ok = true;
            break;
        case WEST:
            if (x > 0
                && map[y][x - 1] != WALL
                && map[y][x - 1] != SNAKE_BODY
//                && map[y][x - 1] != SNAKE_TAIL
                && last_action != EAST)
                ok = true;
            break;
        default:
            break;
    }

    if (ok) {
        action = planned_action;
    } else {
        int tested[] = {0, 0, 0, 0};

        do {
            action = rand() % 4;

            switch (action) {
                case NORTH:
                    if (y > 0 && map[y - 1][x] != WALL && map[y - 1][x] != SNAKE_BODY
                    //&& map[y - 1][x] != SNAKE_TAIL
                    && last_action != SOUTH) {
                        ok = true;
                    }
                    tested[0] = 1;

                    break;
                case EAST:
                    if (x < mapxsize - 1 && map[y][x + 1] != WALL && map[y][x + 1] != SNAKE_BODY
                    //&& map[y][x + 1] != SNAKE_TAIL
                    && last_action != WEST) {
                        ok = true;
                    }
                    tested[1] = 1;

                    break;
                case SOUTH:
                    if (y < mapysize - 1 && map[y + 1][x] != WALL && map[y + 1][x] != SNAKE_BODY
                    //&& map[y + 1][x] != SNAKE_TAIL
                    && last_action != NORTH) {
                        ok = true;
                    }
                    tested[2] = 1;

                    break;
                case WEST:
                    if (x > 0 && map[y][x - 1] != WALL && map[y][x - 1] != SNAKE_BODY
                    //&& map[y][x - 1] != SNAKE_TAIL
                    && last_action != EAST) {
                        ok = true;
                    }
                    tested[3] = 1;

                    break;
                default:
                    ok = true;
                    action = last_action;
                    break;
            }

            int i = 0;
            for (; i < 4; i++) {
                if (tested[i] == 0) {
                    i = 5;
                }
            }

            if (i == 4) {
                action = last_action;
                ok = true;
            }
        } while (!ok);
    }

    return action;
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

void copy(int mapysize, int mapxsize, int originalTable[mapysize][mapxsize], int copyTable[mapysize][mapxsize]) {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapxsize; x++) {
            copyTable[y][x] = originalTable[y][x];
        }
    }
}

void free_all_path(struct path_link *pathLink) {
    struct path_link *currentPathLink = pathLink;
    while (currentPathLink->next != NULL) {
        struct path_link *next_ptr = currentPathLink->next;
        free(currentPathLink);
        currentPathLink = next_ptr;
    }
    free(currentPathLink);
}