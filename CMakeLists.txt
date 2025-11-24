cmake_minimum_required(VERSION 3.20)
project(RDX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_GUI "Build Qt6 GUI application" ON)

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml)

# Find SQLite3 - try multiple methods
find_package(SQLite3 QUIET)
if(NOT SQLite3_FOUND)
    # Try manual search
    find_library(SQLite3_LIBRARY
        NAMES sqlite3 sqlite
        PATHS
            ${CMAKE_SOURCE_DIR}/extern/sqlite/lib
            ${CMAKE_SOURCE_DIR}/extern/sqlite
            "C:/Program Files/SQLite/lib"
            "C:/Program Files (x86)/SQLite/lib"
    )
    find_path(SQLite3_INCLUDE_DIR
        NAMES sqlite3.h
        PATHS
            ${CMAKE_SOURCE_DIR}/extern/sqlite/include
            ${CMAKE_SOURCE_DIR}/extern/sqlite
            "C:/Program Files/SQLite/include"
            "C:/Program Files (x86)/SQLite/include"
    )
    
    if(SQLite3_LIBRARY AND SQLite3_INCLUDE_DIR)
        set(SQLite3_FOUND TRUE)
        add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLite3 PROPERTIES
            IMPORTED_LOCATION "${SQLite3_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}"
        )
    endif()
endif()

if(NOT SQLite3_FOUND)
    message(WARNING "SQLite3 not found. Attempting to use bundled or system library...")
    # Try to find sqlite3 as a system library (Windows may have it in system paths)
    find_library(SQLite3_LIBRARY NAMES sqlite3 sqlite PATHS)
    find_path(SQLite3_INCLUDE_DIR NAMES sqlite3.h PATHS)
    
    if(SQLite3_LIBRARY AND SQLite3_INCLUDE_DIR)
        set(SQLite3_FOUND TRUE)
        add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
        set_target_properties(SQLite::SQLite3 PROPERTIES
            IMPORTED_LOCATION "${SQLite3_LIBRARY}"
            INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}"
        )
        message(STATUS "Found SQLite3: ${SQLite3_LIBRARY}")
    else()
        message(FATAL_ERROR "SQLite3 not found. Please install SQLite3:\n"
            "  Windows: Download from https://www.sqlite.org/download.html or use vcpkg: vcpkg install sqlite3\n"
            "  Linux: sudo apt-get install libsqlite3-dev\n"
            "  Or place SQLite3 in extern/sqlite/ (see extern/README.md)")
    endif()
endif()

# Find zstd (try system first, fallback to extern)
find_library(ZSTD_LIBRARY
    NAMES zstd libzstd
    PATHS
        ${CMAKE_SOURCE_DIR}/extern/zstd/lib
        /usr/lib
        /usr/local/lib
)
find_path(ZSTD_INCLUDE_DIR
    NAMES zstd.h
    PATHS
        ${CMAKE_SOURCE_DIR}/extern/zstd/include
        /usr/include
        /usr/local/include
)

if(NOT ZSTD_LIBRARY OR NOT ZSTD_INCLUDE_DIR)
    message(WARNING "zstd not found. You may need to install it or place it in extern/zstd/")
endif()

# Add subdirectories
add_subdirectory(src/core)

if(BUILD_GUI)
    add_subdirectory(src/app)
endif()

if(BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

