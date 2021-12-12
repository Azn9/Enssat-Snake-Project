/*
 * Naming, formatting and code style based on https://users.ece.cmu.edu/~eno/coding/CCodingStandard.html
 */


// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand, abs
#include <unistd.h>

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char BONUS; // ascii used for the bonuses

char *binome = "Azid Mouad & Joly Axel"; // student names here


struct path_link {
    int x;
    int y;
    struct path_link *next;
};

void calculate_cell_cost(
        int x,
        int y,
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

bool action_is_valid(
        int x,
        int y,
        action last_action,
        action planned_action,
        int mapxsize,
        int mapysize,
        char **map
);

void search_path(
        int x,
        int y,
        int destination_x,
        int destination_y,
        int mapxsize,
        int mapysize,
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int last_x,
        int last_y,
        struct path_link *current_path_link,
        int *path_length
);

action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y,
        action last_action
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
        action planned_action,
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
        const int path_x[path_length],
        const int path_y[path_length],
        bool *can_escape,
        int *tail_path_x,
        int *tail_path_y
);

void copy(
        int mapysize,
        int mapxsize,
        int original_matrix[mapysize][mapxsize],
        int new_matrix[mapysize][mapxsize]
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
        int *bonus_x,
        int *bonus_y,
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
        bool *found_tail_path,
        snake_list snake
);

void calculate_path(
        struct path_link *path_link,
        int path_length,
        int path_x[path_length],
        int path_y[path_length]
);

void free_all_path(struct path_link *path_link);

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
    int cost_empty[mapysize][mapxsize]; //a matrix representing the map with the default cost for each cell
    int heuristic_empty[mapysize][mapxsize]; //a matrix representing the map with the default heuristic for each cell
    int calculated_empty[mapysize][mapxsize]; //a matrix representing the map with the default calculated value for each cell
    int chosen_empty[mapysize][mapxsize]; //a matrix representing the map with the default chosen value for each cell

    int cost[mapysize][mapxsize]; //a matrix representing the map with the cost for each cell
    int heuristic[mapysize][mapxsize]; //a matrix representing the map with the heuristic for each cell
    int calculated[mapysize][mapxsize]; //a matrix representing the map with the calculated value for each cell
    int chosen[mapysize][mapxsize]; //a matrix representing the map with the chosen value for each cell

    // Bonus coordinates
    int bonus_x = 0;
    int bonus_y = 0;

    // Snake's tail coordinates
    int tail_x = 0;
    int tail_y = 0;

    // Snake's head coordinates
    int head_x = s->x;
    int head_y = s->y;

    // Coordinates of the next planned move cell
    int next_x = head_x;
    int next_y = head_y;

    // A temporary variable representing if a path is found or not
    bool path_found = false;

    // A temporary variable representing the path's (if a path is found) first element of a linked list
    struct path_link *path_link = malloc(sizeof(struct path_link));
    path_link->x = bonus_x;
    path_link->y = bonus_y;
    path_link->next = NULL;

    // A temporary variable representing the path's (if a path is found) length
    int path_length = 0;

    // Initialize the cost, heuristic, calculated and chosen matrixes with values from the current map
    initialize_map(mapxsize, mapysize, map, cost, heuristic, calculated, chosen, &bonus_x, &bonus_y, &tail_x, &tail_y);

    // Save the default cost, heuristic, calculated and chosen values
    copy(mapysize, mapxsize, cost, cost_empty);
    copy(mapysize, mapxsize, heuristic, heuristic_empty);
    copy(mapysize, mapxsize, calculated, calculated_empty);
    copy(mapysize, mapxsize, chosen, chosen_empty);

    // Calculate the cost of the cells recursively, starting from the snake's head and aiming to reach the bonus
    calculate_cell_cost(head_x, head_y, bonus_x, bonus_y, head_x, head_y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    // If during calculation of the costs we reached the bonus
    if (path_found) {
        // Define the head values
        calculated[head_y][head_x] = 1;
        cost[head_y][head_x] = 1;
        heuristic[head_y][head_x] = 1;

        // Make the bonus part of the path
        chosen[bonus_y][bonus_x] = 1;

        // Search for the fastest path between the snake's head and the bonus
        search_path(bonus_x, bonus_y, head_x, head_y, mapxsize, mapysize, heuristic, calculated, cost, chosen, &next_x, &next_y, bonus_x, bonus_y, path_link, &path_length);

        // Arrays representing the coordinates of each path cell
        int path_x[path_length];
        int path_y[path_length];

        // Transform the two arrays path_x and path_y to a linked list starting with path_link
        calculate_path(path_link, path_length, path_x, path_y);

        // A temporary value representing if the snake will be able to reach its tail after he moved to the bonus
        bool can_escape_value = false;

        // Coordinates of the next move to do to reach the snake's tail
        int next_tail_x = head_x;
        int next_tail_y = head_y;

        // Calculate if the snake will be able to reach its tail after he moved to the bonus
        can_escape(mapxsize, mapysize, map, s, bonus_x, bonus_y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

        // If we found a path to the bonus
        if (!(next_x == head_x && next_y == head_y)) {
            // If the snake can't go to his tail after moved to the bonus
            if (!can_escape_value) {
                // Reset the cost, heuristic, calculated and chosen matrixes to their defalut values
                copy(mapysize, mapxsize, cost_empty, cost);
                copy(mapysize, mapxsize, heuristic_empty, heuristic);
                copy(mapysize, mapxsize, calculated_empty, calculated);
                copy(mapysize, mapxsize, chosen_empty, chosen);

                // A temporary value representing if a path to the current snake's tail exist
                bool found_tail_path = false;

                // Calculate the path to follow the current snake's tail
                follow_tail(head_x, head_y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, s);
            } else {
                //If we can reach the bonus and then reach our tail after moving, calculate the action to make based on the next move cell's coordinates
                action planned_action = calculate_direction(head_x, head_y, next_x, next_y, last_action);

                // Reset everything to their default values
                can_escape_value = false;
                next_tail_x = head_x;
                next_tail_y = head_y;

                path_length = 1;
                path_x[0] = next_x;
                path_y[0] = next_y;

                // Check if the move we are doing allows the snake to follow his tail
                can_escape(mapxsize, mapysize, map, s, next_x, next_y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

                // If after doing the planned_action action the snake can reach his tail, allows the movement
                if (can_escape_value) {
                    return planned_action;
                } else {
                    // If we can't follow the tail after doing the planned_action action, follow the current snake's tail
                    copy(mapysize, mapxsize, cost_empty, cost);
                    copy(mapysize, mapxsize, heuristic_empty, heuristic);
                    copy(mapysize, mapxsize, calculated_empty, calculated);
                    copy(mapysize, mapxsize, chosen_empty, chosen);

                    bool found_tail_path = false;

                    follow_tail(head_x, head_y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, s);
                }
            }
        } else { // We can't reach the current bonus
            can_escape_value = false;

            // Reset the cost, heuristic, calculated and chosen matrixes to their defalut values
            copy(mapysize, mapxsize, cost_empty, cost);
            copy(mapysize, mapxsize, heuristic_empty, heuristic);
            copy(mapysize, mapxsize, calculated_empty, calculated);
            copy(mapysize, mapxsize, chosen_empty, chosen);

            bool found_tail_path = false;

            // Follow the current snake's tail
            follow_tail(head_x, head_y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, s);
        }

        // If we found a move to do
        if (next_x == head_x && next_y == head_y) {
            can_escape_value = false;
            next_tail_x = head_x;
            next_tail_y = head_y;

            // Check if the snake can reach his tail after moving
            can_escape(mapxsize, mapysize, map, s, head_x, head_y, path_length, path_x, path_y, &can_escape_value, &next_tail_x, &next_tail_y);

            if (can_escape_value) { // If the snake can reach his tail after moving, then follow it
                next_x = next_tail_x;
                next_y = next_tail_y;
            } else { // If we can't reach our tail, we search for the farest cell, and we follow it
                find_farest(head_x, head_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, path_link, &path_length);
            }
        }
    } else { // If during calculation of the costs we didn't reach the bonus
        // Reset the cost, heuristic, calculated and chosen matrixes to their defalut values
        copy(mapysize, mapxsize, cost_empty, cost);
        copy(mapysize, mapxsize, heuristic_empty, heuristic);
        copy(mapysize, mapxsize, calculated_empty, calculated);
        copy(mapysize, mapxsize, chosen_empty, chosen);

        bool found_tail_path = false;

        // Follow our tail
        follow_tail(head_x, head_y, tail_x, tail_y, mapxsize, mapysize, cost, heuristic, calculated, chosen, &next_x, &next_y, &found_tail_path, s);
    }

    // Calculate the action to make based on the next move cell's coordinates
    action planned_action = calculate_direction(head_x, head_y, next_x, next_y, last_action);

    // Validate the action to make sure it will not kill us, and if it's the case, search for another move
    return validate_action_or_random(head_x, head_y, last_action, planned_action, mapxsize, mapysize, map);
}

/*
    calculate_path procedure:
    This procedure transforms a linkedlist using path_link as first element to two arrays of cell coordinates
 */
void calculate_path(
        struct path_link *path_link,
        int path_length,
        int path_x[path_length],
        int path_y[path_length]
) {
    // Initialize the two arrays with zeros
    for (int i = 0; i < path_length; i++) {
        path_x[i] = 0;
        path_y[i] = 0;
    }

    int index = 0;
    struct path_link *current_path_link = path_link;

    // Browse the linked list and append the values to the arrays
    while (index < path_length && current_path_link->next != NULL) {
        path_x[index] = current_path_link->x;
        path_y[index] = current_path_link->y;

        index++;

        current_path_link = current_path_link->next;
    }

    current_path_link = path_link;

    // Free the linked list
    while (current_path_link->next != NULL) {
        struct path_link *next_ptr = current_path_link->next;
        free(current_path_link);
        current_path_link = next_ptr;
    }
    free(current_path_link);
}

/*
    follow_tail procedure:
    This procedure transforms a linkedlist using path_link as first element to two arrays of cell coordinates
 */
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
        bool *found_tail_path,
        snake_list snake
) {
    // Represents if a path to the current tail is found
    bool path_found = false;

    // Calculate the current snake's length
    int snake_length = 1;
    struct snake_link *currents_snake_link = snake;
    while (currents_snake_link->next != NULL) {
        snake_length++;
        currents_snake_link = currents_snake_link->next;
    }

    // Calculate the cost of the cells recursively, starting from the snake's head and aiming to reach its tail
    calculate_cell_cost(x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    // If a path to the tail is found
    if (path_found) {
        // Set the head default values
        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        // Make the tail part of the path
        chosen[tail_y][tail_x] = 1;

        // Initialise the path linked list
        struct path_link *path_link = malloc(sizeof(struct path_link));
        path_link->x = tail_x;
        path_link->y = tail_y;
        path_link->next = NULL;
        int path_length = 0;

        // Trace the path to the tail
        search_path(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, chosen, next_x, next_y, tail_x, tail_y, path_link, &path_length);

        // Free the linked list as we don't need it
        free_all_path(path_link);
    }
}

/*
    initialize_map procedure:
    This procedure browse the whole map, searching for the bonus, the snake's tail and the impassable cells
 */
void initialize_map(
        int mapxsize,
        int mapysize,
        char **map,
        int cost[mapysize][mapxsize],
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *bonus_x,
        int *bonus_y,
        int *tail_x,
        int *tail_y
) {
    for (int search_y = 0; search_y < mapysize; search_y++) {
        for (int search_x = 0; search_x < mapxsize; search_x++) {
            // Set the default values
            cost[search_y][search_x] = 0;
            heuristic[search_y][search_x] = 0;
            calculated[search_y][search_x] = 0;
            chosen[search_y][search_x] = 0;

            char map_char = map[search_y][search_x];

            if (map_char == SNAKE_BODY || map_char == SNAKE_HEAD ||
                map_char == WALL) {

                // We can't cross this cell
                cost[search_y][search_x] = -1;
                heuristic[search_y][search_x] = -1;
                calculated[search_y][search_x] = 1;
            }

            if (map_char == SNAKE_HEAD) {
                // Due to how the pathfinding algorith works, we need this specific value for the head
                calculated[search_y][search_x] = -1;
            }

            // We found the bonus
            if (map_char == BONUS) {
                *bonus_x = search_x;
                *bonus_y = search_y;
            }

            // We found the snake's tail
            if (map_char == SNAKE_TAIL) {
                *tail_x = search_x;
                *tail_y = search_y;
            }
        }
    }
}

/*
    find_farest procedure:
    This procedure searching for the farest cell accessible from the snake's head and set the next_x and next_y values to the coordinates of the first path cell
 */
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
    // With our pathfinding algorithm, the cell with the more cost is the farest cell from the snake's head.
    int max_cost = 0;
    int max_x = 0;
    int max_y = 0;

    // Browse the cost matrix to find the max cost value
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

    // Trace the path between the current snake's head and the found coordinates
    search_path(max_x, max_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, chosen, next_x, next_y, max_x, max_y, pathLink, path_length);
}

/*
    can_escape procedure
    This procedure check if there is a path between the current position and the snake's tail. If this path exists, the values tail_path_x and tail_path_y will be set to the coordinates of the first cell of this path to follow
 */
void can_escape(
        int mapxsize,
        int mapysize,
        char **map,
        snake_list snake,
        int x,
        int y,
        int path_length,
        const int path_x[path_length],
        const int path_y[path_length],
        bool *can_escape,
        int *tail_path_x,
        int *tail_path_y
) {
    // Calculate the snake's length
    int snake_length = 1;
    struct snake_link *current_snake_link = snake;
    while (current_snake_link->next != NULL) {
        snake_length++;
        current_snake_link = current_snake_link->next;
    }

    // Check if the snake fit in the path or if it will "overflow"
    int new_snake_length = snake_length - path_length;
    int min_length = path_length;
    if (snake_length < path_length) {
        min_length = snake_length;
    }

    // Temporary arrays working as the ones on the snake method
    int cost[mapysize][mapxsize];
    int heuristic[mapysize][mapxsize];
    int calculated[mapysize][mapxsize];
    int chosen[mapysize][mapxsize];

    // Browse the map, searching for any wall (so this algorithm can be used on non-rectangular maps
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

    // If the min_length is the path_length, it means that the snake will fit in the path
    for (int i = 0; i < min_length; i++) {
        int current_x = path_x[i];
        int current_y = path_y[i];

        // Prevent any issue if the current_x or current_y value is incorrect
        if (current_x >= 0 && current_y >= 0 && current_x <= mapxsize - 1 && current_y <= mapysize - 1) {
            // Make the cell impassable
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = i == 0 ? -1 : 1;
        }
    }

    // If the min_length is the path_length, it means that the snake will overflow the path, so we need to calculate the new coordinates of the rest of his body
    int index = 0;
    current_snake_link = snake;

    // Browse the snake list until we reach its end or exceed the count of pieces overflowing
    while (index < new_snake_length && current_snake_link->next != NULL) {
        int current_x = current_snake_link->x;
        int current_y = current_snake_link->y;

        if (current_snake_link->c == SNAKE_BODY) {
            // Set this position as impassable
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = 1;
        }

        if (current_snake_link->c == SNAKE_HEAD) {
            // Set this position as impassable
            cost[current_y][current_x] = -1;
            heuristic[current_y][current_x] = -1;
            calculated[current_y][current_x] = 1;
        }

        index++;
        current_snake_link = current_snake_link->next;
    }

    // The new snake tail coordinates (after moving to the bonus)
    int tail_x = current_snake_link->x;
    int tail_y = current_snake_link->y;

    // Represents if a path is found from the new head to the new tail
    bool path_found = false;

    // Calculate the cost of the cells recursively, starting from the new snake's head and aiming to reach the new snake's tail
    calculate_cell_cost(x, y, tail_x, tail_y, x, y, 0, mapxsize, mapysize, cost, heuristic, calculated, &path_found);

    // Initialize the next position cell to the current one
    int next_x = x;
    int next_y = y;

    // If a path is found from the new head to the new tail
    if (path_found) {
        // Initialze all needed values for the pathfinding
        calculated[y][x] = 1;
        cost[y][x] = 1;
        heuristic[y][x] = 1;

        chosen[tail_y][tail_x] = 1;

        struct path_link *path_link = malloc(sizeof(struct path_link));
        path_link->x = tail_x;
        path_link->y = tail_y;
        path_link->next = NULL;
        int path_length_temp = 0;

        // Search for the smallest path between the tail and the head
        search_path(tail_x, tail_y, x, y, mapxsize, mapysize, heuristic, calculated, cost, chosen, &next_x, &next_y, tail_x, tail_y, path_link, &path_length_temp);

        // Free the path linked list, we don't need it
        free_all_path(path_link);

        // We found a path, set the next cell coordinates to the ones of the first cell of the path
        *can_escape = true;
        *tail_path_x = next_x;
        *tail_path_y = next_y;
    }
}

/*
    calculate_cell_cost procedure
    This procedure calculate
 */
void calculate_cell_cost(
        int x,
        int y,
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
    // If the requested coordinates are out of bounds, don't do anything
    if (x < 0 || y < 0 || x > mapxsize - 1 || y > mapysize - 1) {
        return;
    }

    // If the requested cell's cost is already calculated, don't do anything
    if (calculated[y][x] > 0) {
        return;
    }

    // If the destination cell"s cost is already calculated, don't do anything as we already have a shorter path
    if (cost[destinationy][destinationx] > 0) {
        *path_found = true;
        return;
    }

    int currentCost = previousCost;

    // If the cell at the left of the current one exists and is not calculated, we calculate its cost and heuristic
    if (x > 0 && cost[y][x - 1] == 0) {
        cost[y][x - 1] = currentCost + 10;
        heuristic[y][x - 1] =
                currentCost + 10 +
                distance(x - 1, y, destinationx, destinationy) +
                distance(x - 1, y, origin_x, origin_y);
    }

    // If the cell at the top of the current one exists and is not calculated, we calculate its cost and heuristic
    if (y > 0 && cost[y - 1][x] == 0) {
        cost[y - 1][x] = currentCost + 10;
        heuristic[y - 1][x] =
                currentCost + 10 +
                distance(x, y - 1, destinationx, destinationy) +
                distance(x, y - 1, origin_x, origin_y);
    }

    // If the cell at the right of the current one exists and is not calculated, we calculate its cost and heuristic
    if (x < mapxsize - 1 && cost[y][x + 1] == 0) {
        cost[y][x + 1] = currentCost + 10;
        heuristic[y][x + 1] =
                currentCost + 10 +
                distance(x + 1, y, destinationx, destinationy) +
                distance(x + 1, y, origin_x, origin_y);
    }

    // If the cell at the bottom of the current one exists and is not calculated, we calculate its cost and heuristic
    if (y < mapysize - 1 && cost[y + 1][x] == 0) {
        cost[y + 1][x] = currentCost + 10;
        heuristic[y + 1][x] =
                currentCost + 10 +
                distance(x, y + 1, destinationx, destinationy) +
                distance(x, y + 1, origin_x, origin_y);
    }

    // We mark the current cell as calculated
    calculated[y][x] = 1;

    int less_x = 0;
    int less_y = 0;
    int less_heuristic = 999999999;

    // We browse the whole map searching for the cell with the smallest heuristic
    for (int testy = 0; testy < mapysize; testy++) {
        for (int testx = 0; testx < mapxsize; testx++) {
            if (calculated[testy][testx] != 0) {  // If the cell is already calculated, we skip it
                continue;
            }

            int current_heuristic = heuristic[testy][testx];

            if (current_heuristic > 0) { // The heuristic of an impassable cell is -1, we only want positive ones
                if (current_heuristic == less_heuristic) { // Sometimes, two cells have the same heuristic, so we added a small random to chose between the two
                    if (rand() % 2) {
                        less_heuristic = current_heuristic;
                        less_x = testx;
                        less_y = testy;
                    }
                } else if (current_heuristic < less_heuristic) {
                    less_heuristic = current_heuristic;
                    less_x = testx;
                    less_y = testy;
                }
            }
        }
    }

    if (less_x == 0 && less_y == 0) { // We can't found the smallest value, this should never happen
        return;
    }

    // Once we found the cell with the less heuristic, we call recursively the calculate_cell_cost method to calculate its neighbour cells costs
    calculate_cell_cost(less_x, less_y, destinationx, destinationy, origin_x, origin_y, currentCost + 10, mapxsize, mapysize, cost, heuristic, calculated, path_found);
}

/*
    search_path procedure
    This procedure uses the previously calculated costs to find the fastest path from x,y to destination_x,destination_y
 */
void search_path(
        int x,
        int y,
        int destination_x,
        int destination_y,
        int mapxsize,
        int mapysize,
        int heuristic[mapysize][mapxsize],
        int calculated[mapysize][mapxsize],
        int cost[mapysize][mapxsize],
        int chosen[mapysize][mapxsize],
        int *next_x,
        int *next_y,
        int last_x,
        int last_y,
        struct path_link *current_path_link,
        int *path_length
) {
    // If the current coordinates are the ones of the destination, we don't need to go further away
    if (x == destination_x && y == destination_y) {
        return;
    }

    // We search for the smallest cost on our neighbour cells
    int less_x = 0;
    int less_y = 0;
    int less_heuristic = 999999999;
    int current_heuristic;

    // If the cell to the left exists and is not already part of the path
    if (x > 0 && chosen[y][x - 1] == 0) {
        current_heuristic = heuristic[y][x - 1];

        // If the current cell has a positive heuristic (so the cell has been calculated and is traversable)
        if (current_heuristic > 0) {
            if (current_heuristic < less_heuristic) {
                less_heuristic = current_heuristic;
                less_x = x - 1;
                less_y = y;
            }
        }
    }

    // If the cell to the right exists and is not already part of the path
    if (x < mapxsize - 1 && chosen[y][x + 1] == 0) {
        current_heuristic = heuristic[y][x + 1];

        // If the current cell has a positive heuristic (so the cell has been calculated and is traversable)
        if (current_heuristic > 0) {
            if (current_heuristic < less_heuristic) {
                less_heuristic = current_heuristic;
                less_x = x + 1;
                less_y = y;
            } else if (current_heuristic == less_heuristic) { // If the mallest heuristic is equals to the current heuristic, adds hazard to the choice
                if (rand() % 2) {
                    less_heuristic = current_heuristic;
                    less_x = x + 1;
                    less_y = y;
                }
            }
        }
    }

    // If the cell to the top exists and is not already part of the path
    if (y > 0 && chosen[y - 1][x] == 0) {
        current_heuristic = heuristic[y - 1][x];

        // If the current cell has a positive heuristic (so the cell has been calculated and is traversable)
        if (current_heuristic > 0) {
            if (current_heuristic < less_heuristic) {
                less_heuristic = current_heuristic;
                less_x = x;
                less_y = y - 1;
            } else if (current_heuristic == less_heuristic) { // If the mallest heuristic is equals to the current heuristic, adds hazard to the choice
                if (rand() % 2) {
                    less_heuristic = current_heuristic;
                    less_x = x;
                    less_y = y - 1;
                }
            }
        }
    }

    // If the cell to the bottom exists and is not already part of the path
    if (y < mapysize - 1 && chosen[y + 1][x] == 0) {
        current_heuristic = heuristic[y + 1][x];

        // If the current cell has a positive heuristic (so the cell has been calculated and is traversable)
        if (current_heuristic > 0) {
            if (current_heuristic < less_heuristic) {
                less_heuristic = current_heuristic;
                less_x = x;
                less_y = y + 1;
            } else if (current_heuristic == less_heuristic) { // If the mallest heuristic is equals to the current heuristic, adds hazard to the choice
                if (rand() % 2) {
                    less_heuristic = current_heuristic;
                    less_x = x;
                    less_y = y + 1;
                }
            }
        }
    }

    // If the default value has not changed, it means that every cell around is or already part of the path, or impassable or not calculated
    if (less_heuristic == 999999999) {
        // If the previous cell is the same as the current cell, don't do anything
        if (last_x == x && last_y == y) {
            return;
        }

        // Else, go back to the previous cell and calculate it again
        search_path(last_x, last_y, destination_x, destination_y, mapxsize, mapysize, heuristic, calculated, cost, chosen, next_x, next_y, last_x, last_y, current_path_link, path_length);
        return;
    }

    // Adds the found cell to the path

    // Increase the path length
    *path_length = *(path_length) + 1;

    // Adds a new element to the pah linked list
    struct path_link *pathLink = malloc(sizeof(struct path_link));
    pathLink->x = less_x;
    pathLink->y = less_y;
    pathLink->next = NULL;
    (*current_path_link).next = pathLink;

    // Mark the cell as part of the path
    chosen[less_y][less_x] = 1;

    // If the found cell is the destination one, mark the current coordinates as the ones to follow (Because we're doing the path backwards, so the destination is the snake's head)
    if (less_x == destination_x && less_y == destination_y) {
        *next_x = x;
        *next_y = y;
        return;
    }

    // If the found cell is not the destination, continue following the path recursively
    search_path(less_x, less_y, destination_x, destination_y, mapxsize, mapysize, heuristic, calculated, cost, chosen, next_x, next_y, x, y, pathLink, path_length);
}

/*
    calculate_direction function
    This function returns an action calculated from the current coordinates and the ones we want to go to. It also returns the last action if there is no move
 */
action calculate_direction(
        int x,
        int y,
        int next_x,
        int next_y,
        action last_action
) {
    action planned_action;

    // If we don't move, return the last action done
    if (x == next_x && y == next_y) {
        return last_action;
    }

    // If the two cells are on the same column
    if (x == next_x) {
        // If the next cell is on top of the actual
        if (y > next_y) {
            planned_action = NORTH;
        } else { // If the next cell is at the bottom of the actual
            planned_action = SOUTH;
        }
    } else if (x > next_x) { // If the two cells are on the same line
        // If the next cell is at the right of the actual
        planned_action = WEST;
    } else {
        // If the next cell is at the left of the actual
        planned_action = EAST;
    }

    return planned_action;
}

/*
    validate_action_or_random function
    This function checks if the planned action don't kill the snake, and returns a random one (not killing the snake) if it's the case
 */
action validate_action_or_random(
        int x,
        int y,
        action last_action,
        action planned_action,
        int mapxsize,
        int mapysize,
        char **map
) {
    // Represents if the action is valid or not
    bool valid = action_is_valid(x, y, last_action, planned_action, mapxsize, mapysize, map);

    if (valid) {
        return planned_action;
    } else {
        // Adds a list to ensure we don't test multiple times actions, se we don't get stuck on an infinite loop
        int tested[] = {0, 0, 0, 0};

        do {
            // Generate a random action
            action action = rand() % 4;

            // Check if the action is valid or not
            valid = action_is_valid(x, y, last_action, action, mapxsize, mapysize, map);

            // If the action is valid, return it
            if (valid) {
                return action;
            }

            // Mark the action as tested
            tested[action] = 1;

            // Check if there is an action not tested yet
            int i = 0;
            for (; i < 4; i++) {
                if (tested[i] == 0) {
                    i = 5;
                }
            }

            // If all the actions have been tested, we can't do anything, snake is stuck, rip
            if (i == 4) {
                return last_action;
            }
        } while (!valid);
    }

    return last_action;
}

/*
    action_is_valid function
    This functions returns a boolean set to true if the planned_action is allowed and don't kill the snake
 */
bool action_is_valid(
        int x,
        int y,
        action last_action,
        action planned_action,
        int mapxsize,
        int mapysize,
        char **map
) {
    // Represents if the planned_action is valid
    bool ok = false;

    switch (planned_action) {
        case NORTH:
            if (y > 0 // Check if we can go to the top without going out of bounds
                && map[y - 1][x] != WALL // Check if the next cell isn't a wall
                && map[y - 1][x] != SNAKE_BODY // Check if the next cell isn't a part of the snake's body
                && last_action != SOUTH) // We can't go reverse
                ok = true;
            break;
        case EAST:
            if (x < mapxsize - 1 // Check if we can go to the right without going out of bounds
                && map[y][x + 1] != WALL // Check if the next cell isn't a wall
                && map[y][x + 1] != SNAKE_BODY // Check if the next cell isn't a part of the snake's body
                && last_action != WEST) // We can't go reverse
                ok = true;
            break;
        case SOUTH:
            if (y < mapysize - 1 // Check if we can go to the bottom without going out of bounds
                && map[y + 1][x] != WALL // Check if the next cell isn't a wall
                && map[y + 1][x] != SNAKE_BODY // Check if the next cell isn't a part of the snake's body
                && last_action != NORTH) // We can't go reverse
                ok = true;
            break;
        case WEST:
            if (x > 0 // Check if we can go to the left without going out of bounds
                && map[y][x - 1] != WALL // Check if the next cell isn't a wall
                && map[y][x - 1] != SNAKE_BODY // Check if the next cell isn't a part of the snake's body
                && last_action != EAST) // We can't go reverse
                ok = true;
            break;
        default:
            break;
    }

    return ok;
}

/*
    distance function
    This function returns an integer representing the Manhattan's distance (https://fr.wikipedia.org/wiki/Distance_de_Manhattan)
 */
int distance(int source_x, int source_y, int destination_x, int destination_y) {
    return abs(source_x - destination_x) + abs(source_y - destination_y);
}

/*
    copy procedure
    This procedure copies all elements from a matrix to another one
 */
void copy(int mapysize, int mapxsize, int original_matrix[mapysize][mapxsize], int new_matrix[mapysize][mapxsize]) {
    for (int y = 0; y < mapysize; y++) {
        for (int x = 0; x < mapxsize; x++) {
            new_matrix[y][x] = original_matrix[y][x];
        }
    }
}

/*
    free_all_path procedure
    This procedure frees the memory used by each element of the provided linked list
 */
void free_all_path(struct path_link *path_link) {
    struct path_link *current_path_link = path_link;
    while (current_path_link->next != NULL) {
        struct path_link *next_ptr = current_path_link->next;
        free(current_path_link);
        current_path_link = next_ptr;
    }
    free(current_path_link);
}
