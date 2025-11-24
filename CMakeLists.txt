cmake_minimum_required(VERSION 3.20)
project(RDX VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Build options
option(BUILD_TESTS "Build test suite" ON)
option(BUILD_GUI "Build Qt6 GUI application" ON)

# Find dependencies
find_package(Qt6 REQUIRED COMPONENTS Core Quick Qml QuickControls2)

# Find SQLite3 - try vcpkg's unofficial-sqlite3 first, then standard find
# vcpkg provides it as unofficial-sqlite3, set the path explicitly if using vcpkg
if(CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_TOOLCHAIN_FILE}")
    set(unofficial-sqlite3_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../vcpkg/packages/sqlite3_x64-windows/share/unofficial-sqlite3")
    # Try to find it in vcpkg's installed directory
    find_path(VCPKG_INSTALLED_DIR 
        NAMES packages/sqlite3_x64-windows/share/unofficial-sqlite3
        PATHS
            "C:/vcpkg"
            "${CMAKE_SOURCE_DIR}/../vcpkg"
        NO_DEFAULT_PATH
    )
    if(VCPKG_INSTALLED_DIR)
        set(unofficial-sqlite3_DIR "${VCPKG_INSTALLED_DIR}/packages/sqlite3_x64-windows/share/unofficial-sqlite3")
    endif()
endif()

find_package(unofficial-sqlite3 CONFIG QUIET)
if(TARGET unofficial::sqlite3::sqlite3)
    set(SQLite3_FOUND TRUE)
    # unofficial-sqlite3 provides unofficial::sqlite3::sqlite3 target
    # Create alias for consistency
    if(NOT TARGET SQLite::SQLite3)
        add_library(SQLite::SQLite3 ALIAS unofficial::sqlite3::sqlite3)
    endif()
    message(STATUS "Found SQLite3 via unofficial-sqlite3")
else()
    find_package(SQLite3 QUIET)
    if(NOT SQLite3_FOUND)
        find_package(SQLite3 CONFIG QUIET)
    endif()
    if(SQLite3_FOUND)
        message(STATUS "Found SQLite3 via standard find_package")
    endif()
endif()

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
        if(NOT TARGET SQLite::SQLite3)
            add_library(SQLite::SQLite3 UNKNOWN IMPORTED)
            set_target_properties(SQLite::SQLite3 PROPERTIES
                IMPORTED_LOCATION "${SQLite3_LIBRARY}"
                INTERFACE_INCLUDE_DIRECTORIES "${SQLite3_INCLUDE_DIR}"
            )
        endif()
    endif()
endif()

if(NOT SQLite3_FOUND)
    message(FATAL_ERROR "SQLite3 not found. Please install SQLite3:\n"
        "  Windows: Use vcpkg: vcpkg install sqlite3:x64-windows\n"
        "  Linux: sudo apt-get install libsqlite3-dev\n"
        "  Or place SQLite3 in extern/sqlite/ (see extern/README.md)")
endif()

message(STATUS "Found SQLite3: ${SQLite3_LIBRARY}")

# Find zstd - try vcpkg config first, then standard find
find_package(zstd CONFIG QUIET)
if(NOT zstd_FOUND)
    # Try to find zstd in vcpkg if using vcpkg
    if(CMAKE_TOOLCHAIN_FILE AND EXISTS "${CMAKE_TOOLCHAIN_FILE}")
        find_path(VCPKG_INSTALLED_DIR 
            NAMES packages/zstd_x64-windows/include/zstd.h
            PATHS
                "C:/vcpkg"
                "${CMAKE_SOURCE_DIR}/../vcpkg"
            NO_DEFAULT_PATH
        )
        if(VCPKG_INSTALLED_DIR)
            set(ZSTD_INCLUDE_DIR "${VCPKG_INSTALLED_DIR}/packages/zstd_x64-windows/include")
            find_library(ZSTD_LIBRARY
                NAMES zstd
                PATHS
                    "${VCPKG_INSTALLED_DIR}/packages/zstd_x64-windows/lib"
                NO_DEFAULT_PATH
            )
            if(ZSTD_LIBRARY AND ZSTD_INCLUDE_DIR)
                if(NOT TARGET zstd::libzstd)
                    add_library(zstd::libzstd UNKNOWN IMPORTED)
                    set_target_properties(zstd::libzstd PROPERTIES
                        IMPORTED_LOCATION "${ZSTD_LIBRARY}"
                        INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_INCLUDE_DIR}"
                    )
                endif()
                set(zstd_FOUND TRUE)
            endif()
        endif()
    endif()
    
    # Fallback to standard find
    if(NOT zstd_FOUND)
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
        
        if(ZSTD_LIBRARY AND ZSTD_INCLUDE_DIR)
            if(NOT TARGET zstd::libzstd)
                add_library(zstd::libzstd UNKNOWN IMPORTED)
                set_target_properties(zstd::libzstd PROPERTIES
                    IMPORTED_LOCATION "${ZSTD_LIBRARY}"
                    INTERFACE_INCLUDE_DIRECTORIES "${ZSTD_INCLUDE_DIR}"
                )
            endif()
            set(zstd_FOUND TRUE)
        endif()
    endif()
endif()

if(zstd_FOUND)
    message(STATUS "Found zstd")
else()
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

