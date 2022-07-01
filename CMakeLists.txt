cmake_minimum_required(VERSION 3.5)

project(ls LANGUAGES C)

add_executable(ls file_perms.h file_perms.c main.c)
