cmake_minimum_required(VERSION 3.22.1)

project(string_processor 
VERSION 0.2.0 
LANGUAGES C
)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Debug)
add_executable(${PROJECT_NAME} main.c string_parse.c replace_word.c regexp.c error_handling.c)
configure_file(${PROJECT_NAME}Config.h.in ${PROJECT_NAME}Config.h)

list(APPEND MY_INCLUDE_DIRECTORIES ${PROJECT_SOURCE_DIR})
list(APPEND MY_INCLUDE_DIRECTORIES ${PROJECT_BUILD_DIR})
# target_compile_options(${PROJECT_NAME} PRIVATE -Werror)

# if (CMAKE_BUILD_TYPE STREQUAL Debug AND CMAKE_C_COMPILER_ID STREQUAL "GNU")
#   target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
#   target_compile_options(${PROJECT_NAME} PRIVATE -ASAN_OPTIONS=symbolize=1)
#   target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=address)
#   target_compile_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
#   target_link_options(${PROJECT_NAME} PRIVATE -fsanitize=undefined)
# endif()

target_include_directories(${PROJECT_NAME}
        PRIVATE ${MY_INCLUDE_DIRECTORIES}
)
