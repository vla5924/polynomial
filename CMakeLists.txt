cmake_minimum_required(VERSION 3.0.0)
project(polynomial)

include_directories(include)

add_library(polynomial STATIC
    src/precompiled.cpp
    src/list.cpp
    src/listexception.cpp
    src/listiterator.cpp
    src/monomial.cpp
    src/polynomial.cpp
)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

target_link_libraries(polynomial)

add_subdirectory(sample)
