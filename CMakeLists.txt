cmake_minimum_required(VERSION 3.10)

project(TimeTrackerPractice LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

enable_testing()

add_library(timetracker_core STATIC
    src/ConsoleApp.cpp
    src/DateTimeUtils.cpp
    src/ReportService.cpp
    src/Storage.cpp
    src/Task.cpp
    src/TimeEntry.cpp
    src/TimeTrackerService.cpp
)

target_include_directories(timetracker_core PUBLIC include)

add_executable(timetracker src/main.cpp)
target_link_libraries(timetracker PRIVATE timetracker_core)

set(TEST_RUNTIME_DIR "${CMAKE_BINARY_DIR}/tests")

function(configure_test_target target_name)
    target_link_libraries(${target_name} PRIVATE timetracker_core)
    set_target_properties(${target_name} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY "${TEST_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG "${TEST_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE "${TEST_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${TEST_RUNTIME_DIR}"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${TEST_RUNTIME_DIR}"
    )
endfunction()

add_executable(timetracker_tests tests/unit_tests.cpp)
target_include_directories(timetracker_tests PRIVATE external/catch2)
configure_test_target(timetracker_tests)
add_test(NAME unit_tests COMMAND timetracker_tests)

add_executable(scenario_basic_workflow tests/scenarios/scenario_basic_workflow.cpp)
configure_test_target(scenario_basic_workflow)
add_test(NAME scenario_basic_workflow COMMAND scenario_basic_workflow)

add_executable(scenario_persistence tests/scenarios/scenario_persistence.cpp)
configure_test_target(scenario_persistence)
add_test(NAME scenario_persistence COMMAND scenario_persistence)

add_executable(scenario_console_workflow tests/scenarios/scenario_console_workflow.cpp)
configure_test_target(scenario_console_workflow)
add_test(NAME scenario_console_workflow COMMAND scenario_console_workflow)
