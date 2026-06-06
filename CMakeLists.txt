cmake_minimum_required(VERSION 3.16)

project(BeamElementSectionPropertyTool VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS ON)

set(CMAKE_PREFIX_PATH "C:/msys64/ucrt64")

add_library(beam_section_core
    source/Core/PropertyCalculator.cpp
    source/geometry/GeometryUtils.cpp
    source/geometry/SectionBase.cpp
)

target_include_directories(beam_section_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_compile_options(beam_section_core PRIVATE -Wall -Wextra -Wpedantic)

add_library(beam_section_gui_state
    source/GUI/PreviewShapeAdapter.cpp
    source/GUI/SectionState.cpp
    source/GUI/StressPointDefaults.cpp
    source/GUI/MeshGenerator.cpp
    source/GUI/MeshDisplayWidget.cpp
)

target_include_directories(beam_section_gui_state
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/source
)

# Qt6 include yolunu elle ekle (KESİN ÇÖZÜM)
target_include_directories(beam_section_gui_state
    PRIVATE
        "C:/msys64/ucrt64/include/qt6"
        "C:/msys64/ucrt64/include/qt6/QtCore"
        "C:/msys64/ucrt64/include/qt6/QtWidgets"
        "C:/msys64/ucrt64/include/qt6/QtGui"
)

target_link_libraries(beam_section_gui_state
    PUBLIC beam_section_core
)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(beam_section_gui
    source/GUI/FEMeshTab.cpp
    source/GUI/GeneralTab.cpp
    source/GUI/main.cpp
    source/GUI/MainWindow.cpp
    source/GUI/SectionPreviewWidget.cpp
    source/GUI/StressPointsTab.cpp
)

target_link_libraries(beam_section_gui
    PRIVATE
        beam_section_gui_state
        Qt6::Widgets
)