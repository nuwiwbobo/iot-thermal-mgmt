cmake_minimum_required(VERSION 3.5)
include($ENV{IDF_PATH}/tools/cmake/project.cmake)
set(PARTITION_TABLE_CSV_PATH "partitions.csv")
project(thermal-mgmt)