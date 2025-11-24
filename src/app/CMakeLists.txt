cmake_minimum_required(VERSION 3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# GUI application
add_executable(rdx_gui
    main.cpp
    viewmodels/JobViewModel.cpp
    viewmodels/LCMStatsViewModel.cpp
    controllers/CompressionController.cpp
    ui/qml.qrc
    ui/RdxMainWindow.qml
    ui/JobListView.qml
    ui/FileChooserView.qml
    ui/CorpusDashboardView.qml
    ui/SettingsView.qml
)

target_include_directories(rdx_gui PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}
)

# MSVC needs /Zc:__cplusplus for Qt
if(MSVC)
    target_compile_options(rdx_gui PRIVATE /Zc:__cplusplus)
endif()

target_link_libraries(rdx_gui PRIVATE
    rdx_core
    Qt6::Core
    Qt6::Quick
    Qt6::Qml
)

# Set QML import path
set_target_properties(rdx_gui PROPERTIES
    WIN32_EXECUTABLE TRUE
)

