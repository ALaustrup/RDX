cmake_minimum_required(VERSION 3.20)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# QML resources
qt_add_resources(app_resources
    PREFIX "/"
    FILES
        ui/RdxMainWindow.qml
        ui/JobListView.qml
        ui/FileChooserView.qml
        ui/CorpusDashboardView.qml
        ui/SettingsView.qml
)

# GUI application
add_executable(rdx_gui
    main.cpp
    viewmodels/JobViewModel.cpp
    viewmodels/LCMStatsViewModel.cpp
    controllers/CompressionController.cpp
    ${app_resources}
)

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

