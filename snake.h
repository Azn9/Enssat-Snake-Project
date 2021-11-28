/*
  Enumeration for the different actions that the player may choose
 */
enum actions {NORTH, EAST, SOUTH, WEST};
typedef enum actions action;

struct snake_link { // models a piece of Snake
    char c; // SNAKE_HEAD, SNAKE_BODY, or SNAKE_TAIL char
    int x; // x position for this piece of Snake within the map
    int y; // y position for this piece of Snake within the map
    struct snake_link * next; // pointer to the next piece of Snake
};
typedef struct snake_link * snake_list;

action snake(char * *, int, int, snake_list, action);

