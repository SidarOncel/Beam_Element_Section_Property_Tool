cmake_minimum_required(VERSION 3.16)

project(BeamElementSectionPropertyTool
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_library(beam_section_core
    source/Core/PropertyCalculator.cpp
    source/geometry/GeometryUtils.cpp
    source/geometry/SectionBase.cpp
)

target_include_directories(beam_section_core
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

if(MSVC)
    target_compile_options(beam_section_core PRIVATE /W4)
else()
    target_compile_options(beam_section_core PRIVATE -Wall -Wextra -Wpedantic)
endif()

add_executable(beam_property_tests
    source/Tests/TestRunner.cpp
)

target_link_libraries(beam_property_tests
    PRIVATE
        beam_section_core
)

enable_testing()

add_test(
    NAME beam_property_tests
    COMMAND beam_property_tests
)

add_library(beam_section_gui_state
    source/GUI/PreviewShapeAdapter.cpp
    source/GUI/PreviewShapeAdapter.hpp
    source/GUI/SectionState.cpp
    source/GUI/SectionState.hpp
    source/GUI/StressPointDefaults.cpp
    source/GUI/StressPointDefaults.hpp
)

target_include_directories(beam_section_gui_state
    PUBLIC
        ${CMAKE_CURRENT_SOURCE_DIR}/source
)

target_link_libraries(beam_section_gui_state
    PUBLIC
        beam_section_core
)

find_package(Qt5 COMPONENTS Widgets QUIET)

if(Qt5Widgets_FOUND)
    add_executable(beam_section_gui
        source/GUI/FEMeshTab.cpp
        source/GUI/FEMeshTab.hpp
        source/GUI/GeneralTab.cpp
        source/GUI/GeneralTab.hpp
        source/GUI/main.cpp
        source/GUI/MainWindow.cpp
        source/GUI/MainWindow.hpp
        source/GUI/SectionPreviewWidget.cpp
        source/GUI/SectionPreviewWidget.hpp
        source/GUI/StressPointsTab.cpp
        source/GUI/StressPointsTab.hpp
    )

    target_link_libraries(beam_section_gui
        PRIVATE
            beam_section_gui_state
            Qt5::Widgets
    )
else()
    message(STATUS "Qt5 Widgets was not found; skipping the beam_section_gui target.")
endif()
