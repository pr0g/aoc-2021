cmake_minimum_required(VERSION 3.15)
project(aoc-day19)

include(FetchContent)
FetchContent_Declare(
  as
  GIT_REPOSITORY https://github.com/pr0g/as.git
  GIT_TAG origin/main)
FetchContent_MakeAvailable(as)

add_executable(${PROJECT_NAME})
target_sources(${PROJECT_NAME} PRIVATE main.cpp)
target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)
target_compile_definitions(${PROJECT_NAME} PRIVATE -DAS_PRECISION_FLOAT
                                                   -DAS_ROW_MAJOR)
target_link_libraries(${PROJECT_NAME} PRIVATE as)
