cmake_minimum_required(VERSION 3.15)
project(VMPAttach)

include(FetchContent)

SET(FETCHCONTENT_FULLY_DISCONNECTED on)

#FetchContent_Declare(
#    VTIL-Core
#    GIT_REPOSITORY https://github.com/vtil-project/VTIL-Core
#    GIT_SHALLOW    true
#    SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/VTIL-Core
#)
#FetchContent_MakeAvailable(VTIL-Core)

add_subdirectory(VTIL-Core)
add_subdirectory(VMPAttack)