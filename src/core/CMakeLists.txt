cmake_minimum_required(VERSION 3.20)

# Core library
add_library(rdx_core STATIC
    lcm/LCMManager.cpp
    schemas/SchemaDefinition.cpp
    schemas/SchemaRegistry.cpp
    schemas/parsers/PE32Parser.cpp
    schemas/parsers/JSONParser.cpp
    schemas/parsers/LogParser.cpp
    schemas/parsers/CSVParser.cpp
    schemas/parsers/KVConfigParser.cpp
    schemas/parsers/ChunkedBinaryParser.cpp
    schemas/parsers/UnstructuredBinaryParser.cpp
    detectors/FileTypeDetector.cpp
    compression/CompressionEngine.cpp
    decompression/DecompressionEngine.cpp
    container/RDXWriter.cpp
    container/RDXReader.cpp
    util/ByteBuffer.cpp
    util/HashUtils.cpp
    util/TimeUtils.cpp
)

target_include_directories(rdx_core PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${CMAKE_CURRENT_BINARY_DIR}
)

# Add zstd include directory if using vcpkg
if(TARGET zstd::libzstd)
    get_target_property(ZSTD_INCLUDE_DIR zstd::libzstd INTERFACE_INCLUDE_DIRECTORIES)
    if(ZSTD_INCLUDE_DIR)
        target_include_directories(rdx_core PUBLIC ${ZSTD_INCLUDE_DIR})
    endif()
elseif(ZSTD_INCLUDE_DIR)
    target_include_directories(rdx_core PUBLIC ${ZSTD_INCLUDE_DIR})
endif()

# Link libraries - use targets if available, otherwise fall back to variables
if(TARGET SQLite::SQLite3)
    target_link_libraries(rdx_core PUBLIC SQLite::SQLite3)
elseif(SQLite3_LIBRARY)
    target_link_libraries(rdx_core PUBLIC ${SQLite3_LIBRARY})
    target_include_directories(rdx_core PUBLIC ${SQLite3_INCLUDE_DIR})
endif()

if(TARGET zstd::libzstd)
    target_link_libraries(rdx_core PUBLIC zstd::libzstd)
elseif(ZSTD_LIBRARY)
    target_link_libraries(rdx_core PUBLIC ${ZSTD_LIBRARY})
    target_include_directories(rdx_core PUBLIC ${ZSTD_INCLUDE_DIR})
endif()

# Export header for symbol visibility
configure_file(
    ${CMAKE_CURRENT_SOURCE_DIR}/rdx_core_export.h.in
    ${CMAKE_CURRENT_BINARY_DIR}/rdx_core_export.h
    @ONLY
)

