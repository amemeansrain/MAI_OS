cmake_minimum_required(VERSION 3.10)
project(lab1)

set(CMAKE_CXX_STANDARD 17)

# Для использования shared memory (librt)
find_library(RT_LIBRARY rt)

# Добавление директории с заголовочными файлами
include_directories(funcs)

# Сборка библиотеки из funcs
add_library(funcs STATIC funcs/funcs.cpp)

# Компиляция исполняемых файлов
add_executable(main main.cpp)
add_executable(child1 child1.cpp)
add_executable(child2 child2.cpp)

# Связывание с библиотекой funcs и librt для shared memory
target_link_libraries(main funcs ${RT_LIBRARY})
target_link_libraries(child1 funcs ${RT_LIBRARY})
target_link_libraries(child2 funcs ${RT_LIBRARY})