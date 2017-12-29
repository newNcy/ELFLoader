
project(load)
cmake_minimum_required(VERSION 2.8)
include_directories(include)
aux_source_directory(src DIR_SRCS)

ADD_EXECUTABLE(load ${DIR_SRCS})
