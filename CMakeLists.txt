cmake_minimum_required(VERSION 3.10)
project(BS_10_2024 C)

set(CMAKE_C_STANDARD 99)

add_executable(BS_10_2024 main.c
        main.h
        keyValStore.h
        keyValStore.c
        clientHandler.c
        clientHandler.h
        semaphore.c
        semaphore.h
        msgQueue.c
        msgQueue.h
        sub.c
        sub.h
        cmdType.h
)
