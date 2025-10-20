cmake_minimum_required(VERSION 3.16)

project(MobaController VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 自动查找当前目录下所有的 .h 文件，并放入 HEADERS 变量
file(GLOB HEADERS "*.h")

# 自动查找当前目录下所有的 .cpp 文件，并放入 SOURCES 变量
file(GLOB SOURCES "*.cpp")

# 自动查找当前目录下所有的 .ui 文件，并放入 FORMS 变量
file(GLOB FORMS "*.ui")

# 把所有找到的文件合并到 PROJECT_SOURCES 变量里
set(PROJECT_SOURCES ${HEADERS} ${SOURCES} ${FORMS})

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(MobaController
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
# Define target properties for Android with Qt 6 as:
#    set_property(TARGET MobaController APPEND PROPERTY QT_ANDROID_PACKAGE_SOURCE_DIR
#                 ${CMAKE_CURRENT_SOURCE_DIR}/android)
# For more information, see https://doc.qt.io/qt-6/qt-add-executable.html#target-creation
else()
    if(ANDROID)
        add_library(MobaController SHARED
            ${PROJECT_SOURCES}
        )
# Define properties for Android with Qt 5 after find_package() calls as:
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
    else()
        add_executable(MobaController
            ${PROJECT_SOURCES}
        )
    endif()
endif()

if(MSVC)
    target_compile_options(MobaController PRIVATE /utf-8)
endif()

target_link_libraries(MobaController PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
if (WIN32)
    target_link_libraries(MobaController PRIVATE user32)
endif()

# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.MobaController)
endif()
set_target_properties(MobaController PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS MobaController
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(MobaController)
endif()
