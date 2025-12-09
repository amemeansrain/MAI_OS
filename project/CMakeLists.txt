cmake_minimum_required(VERSION 3.15)
project(DAG_Scheduler)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Библиотека JSON (можно скачать автоматически)
include(FetchContent)
FetchContent_Declare(
    nlohmann_json
    GIT_REPOSITORY https://github.com/nlohmann/json.git
    GIT_TAG v3.11.2
)
FetchContent_MakeAvailable(nlohmann_json)

# Основная программа
add_executable(dag_scheduler
    src/main.cpp
    src/dag.cpp
    src/job.cpp
    src/parser_json.cpp
    src/scheduler.cpp
    src/semaphore.cpp
    src/utils.cpp
)

target_include_directories(dag_scheduler PRIVATE src)
target_link_libraries(dag_scheduler PRIVATE nlohmann_json::nlohmann_json pthread)

# Тесты
enable_testing()
add_executable(tests
    tests/test_dag.cpp
    tests/test_semaphore.cpp
    src/dag.cpp
    src/job.cpp
    src/parser_json.cpp
    src/scheduler.cpp
    src/semaphore.cpp
    src/utils.cpp
)
target_include_directories(tests PRIVATE src)
target_link_libraries(tests PRIVATE nlohmann_json::nlohmann_json pthread gtest_main)
add_test(NAME DAGTests COMMAND tests)