// compiler header files
#include <stdbool.h> // bool, true, false
#include <stdlib.h> // rand
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

char *binome = "Random"; // student names here

struct node_link {
    long cost;
    long x;
    long y;
    struct node_link *next;
};
typedef struct node_link *node_list;

node_list computeNode(char **map, int mapxsize, int mapysize, int x, int y, int source_x, int source_y, int destination_x, int destination_y, node_list nodeList);
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
    struct node_link first_link = {distance(0, 0, 8, 8), 0, 0, NULL};
    node_list nodeList = &first_link;
    nodeList = computeNode(map, mapxsize, mapysize, 1, 1, 1, 1, 8, 8, nodeList);

    node_list current_node = nodeList->next;

    if (current_node == NULL) {
        printf("A");
    }

    while (current_node != NULL) {
        if (current_node->next == NULL) {
            break;
        }

        current_node = current_node->next;
    }

    printf("OKKKK");

    while (true) {
        sleep(10);
    }

    return 0; // answer to the game engine
}

int distance(int source_x, int source_y, int destination_x, int destination_y) {
    return (source_x - destination_x) * (source_x - destination_x) + (source_y - destination_y) * (source_y - destination_y);
}

node_list computeNode(char **map, int mapxsize, int mapysize, int x, int y, int source_x, int source_y, int destination_x, int destination_y, node_list nodeList) {
    if (x < 1 || y < 1 || x >= mapxsize - 1 || y >= mapysize - 1) {
        printf("LES PROBLEMES !");
        return nodeList;
    }

    if (map[y][x] != PATH) {
        return nodeList;
    }

    if (x == destination_x && y == destination_y) {
        printf("DEST\n");
        return nodeList;
    }

    printf("Current X : %d | %d", x, y);

    node_list current_node = nodeList->next;

    printf("1\n");

    while (current_node != NULL) {
        if (current_node->x == x && current_node->y == y) {
            return nodeList;
        }

        if (current_node->next == NULL || &(current_node->next) == NULL) {
            break;
        }

        current_node = current_node->next;
    }

    printf("2\n");

    struct node_link nextLink = {distance(x, y, source_x, source_y) + distance(x, y, destination_x, destination_y), x, y, NULL};
    if (nextLink.cost < 0) {
        return nodeList;
    }

    printf("2.2\n");

    if (current_node == NULL) {
        nodeList->next = &nextLink;
    } else {
        current_node->next = &nextLink;
        current_node = current_node->next;
    }

    printf("3\n");

    if (x > 0) {
        node_list temp = computeNode(map, mapxsize, mapysize, x - 1, y, source_x, source_y, destination_x, destination_y, nodeList);
    }

    if (y > 0) {
        node_list temp = computeNode(map, mapxsize, mapysize, x, y - 1, source_x, source_y, destination_x, destination_y, nodeList);
    }

    if (x < mapxsize - 1) {
        node_list temp = computeNode(map, mapxsize, mapysize, x + 1, y, source_x, source_y, destination_x, destination_y, nodeList);
    }

    if (y < mapysize - 1) {
        node_list temp = computeNode(map, mapxsize, mapysize, x, y + 1, source_x, source_y, destination_x, destination_y, nodeList);
    }

    printf("4\n");

    current_node = nodeList->next;
    int less_cost = 0;
    node_list less_node;

    printf("5\n");

    while (current_node != NULL) {
        printf("6.0 %d\n", less_cost == 0);
        printf("6.0.1 %d\n", current_node->cost);
        printf("6.0.2 %d\n", current_node->cost < less_cost);

        if (less_cost == 0) {
            printf("6.1\n");
            less_cost = current_node->cost;
            printf("6.1.2\n");
            less_node = current_node;
        } else if (current_node->cost > 0 && current_node->cost < less_cost) {
            printf("6.2\n");
            less_cost = current_node->cost;
            less_node = current_node;
        }

        printf("6\n");
        current_node = current_node->next;
        printf("7\n");
    }

    printf("8\n");

    if (less_node != NULL) {
        return computeNode(map, mapxsize, mapysize, less_node->x, less_node->y, source_x, source_y, destination_x,
                           destination_y, nodeList);
    } else {
        return nodeList;
    }
}
