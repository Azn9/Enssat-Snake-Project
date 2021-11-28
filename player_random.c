// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
#include <stdio.h> // printf

// program header file
#include "snake.h"

// global declarations
extern const char SNAKE_HEAD; // ascii used for snake's head
extern const char SNAKE_BODY; // ascii used for snake's body
extern const char SNAKE_TAIL; // ascii used for snake's tail
extern const char WALL; // ascii used for the walls
extern const char PATH; // ascii used for the paths
extern const char BONUS; // ascii used for the bonuses

extern bool DEBUG; // indicates whether the game runs in DEBUG mode

char *binome = "Random"; // student names here

// prototypes of the local functions/procedures
void printAction(action);

void printBoolean(bool);

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
    printf("Last action : %d\n", last_action);

    action a; // action to choose and return

    bool ok = false; // ok will be set to true as soon as a randomly selected action is valid

    do {
        a = rand() % 4; // ramdomly select an action: 0=NORTH, 1=EAST...

        if (DEBUG) { // print the randomly selected action, only in DEBUG mode
            printf("Candidate action is: ");
            printAction(a);
            printf("\n");
        }

        switch (a) { // check whether the randomly selected action is valid, i.e., if its preconditions are satisfied
            case NORTH:
                if (map[s->y - 1][s->x] != WALL
                    && map[s->y - 1][s->x] != SNAKE_BODY
                    && map[s->y - 1][s->x] != SNAKE_TAIL)
                    ok = true;
                break;
            case EAST:
                if (map[s->y][s->x + 1] != WALL
                    && map[s->y][s->x + 1] != SNAKE_BODY
                    && map[s->y][s->x + 1] != SNAKE_TAIL)
                    ok = true;
                break;
            case SOUTH:
                if (map[s->y + 1][s->x] != WALL
                    && map[s->y + 1][s->x] != SNAKE_BODY
                    && map[s->y + 1][s->x] != SNAKE_TAIL)
                    ok = true;
                break;
            case WEST:
                if (map[s->y][s->x - 1] != WALL
                    && map[s->y][s->x - 1] != SNAKE_BODY
                    && map[s->y][s->x - 1] != SNAKE_TAIL)
                    ok = true;
                break;
        }

        if (DEBUG) { // print whether the randomly selected action is valid, only in DEBUG mode
            printf("Is this candidate action valid? ");
            printBoolean(ok);
            printf("\n");
        }
    } while (!ok && (map[s->y - 1][s->x] == PATH
                     || map[s->y][s->x + 1] == PATH
                     || map[s->y + 1][s->x] == PATH
                     || map[s->y][s->x - 1] == PATH));

    return a; // answer to the game engine
}

/*
  printAction procedure:
  This procedure prints the input action name on screen.
 */
void printAction(action a) {
    switch (a) {
        case NORTH:
            printf("NORTH");
            break;
        case EAST:
            printf("EAST");
            break;
        case SOUTH:
            printf("SOUTH");
            break;
        case WEST:
            printf("WEST");
            break;
    }
}


/*
  printBoolean procedure:
  This procedure prints the input boolan value on screen.
 */
void printBoolean(bool b) {
    if (b) {
        printf("true");
    } else {
        printf("false");
    }
}

