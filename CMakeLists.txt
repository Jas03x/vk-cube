
cmake_minimum_required(VERSION 3.10)

project(vk-cube)

file(GLOB sources source/*.c)

message("${sources}")

add_executable(vk-cube.exe ${sources})
