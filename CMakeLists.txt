cmake_minimum_required(VERSION 3.10)
project(BS_10_2024 C)

set(CMAKE_C_STANDARD 99)

add_executable(BS_10_2024 main.c
        main.h
        keyValStore.h
        keyValStore.c
        semaphore.c
)
