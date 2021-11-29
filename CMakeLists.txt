cmake_minimum_required(VERSION 3.16)
project(Snake C)

set(CMAKE_C_STANDARD 99)
add_executable(Snake snake.o player_astar.c)