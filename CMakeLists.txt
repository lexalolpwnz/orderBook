cmake_minimum_required(VERSION 3.14)
project(orderBook)

set(CMAKE_CXX_STANDARD 14)

include_directories(third_party)

SET(THIRD_PARTY_SRCS
        third_party/nlohmann/json.hpp
        )

SET(PROGRAM_SRCS
        main.cpp
        ${THIRD_PARTY_SRCS}
        )

add_executable(${CMAKE_PROJECT_NAME} ${PROGRAM_SRCS})
TARGET_LINK_LIBRARIES(${CMAKE_PROJECT_NAME} pthread)