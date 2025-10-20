cmake_minimum_required(VERSION 3.16)

project(MobaController VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

# 用户指定的第三方库安装路径（如需调整请改这里）
# OpenCV 安装根目录（应包含 OpenCVConfig.cmake 于 C:/opencv/build）
set(OpenCV_DIR "C:/opencv/build")
# Tesseract 安装根目录（通常包含 lib/cmake/* 配置文件）
set(Tesseract_ROOT "C:/Program Files/Tesseract-OCR")

# 提示 CMake 在这些位置优先搜索
list(PREPEND CMAKE_PREFIX_PATH "${OpenCV_DIR}" "${Tesseract_ROOT}")

# 可选 OpenCV（用于截屏+模板匹配识别）
option(ENABLE_OPENCV "Enable OpenCV features" ON)
option(ENABLE_TESSERACT "Enable Tesseract OCR" ON)
if (ENABLE_OPENCV)
    find_package(OpenCV QUIET COMPONENTS core imgproc imgcodecs)
    if (OpenCV_FOUND)
        message(STATUS "OpenCV found: ${OpenCV_VERSION}")
        add_definitions(-DENABLE_OPENCV)
    else()
        message(WARNING "OpenCV not found; OCR features will be disabled")
    endif()
endif()

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
    # 低级鼠标钩子使用在 user32 中即可
endif()
if (ENABLE_OPENCV)
    find_package(OpenCV QUIET COMPONENTS core imgproc imgcodecs)
    if (OpenCV_FOUND)
        target_include_directories(MobaController PRIVATE ${OpenCV_INCLUDE_DIRS})
        target_link_libraries(MobaController PRIVATE ${OpenCV_LIBS})
        add_definitions(-DENABLE_OPENCV)
    else()
        message(WARNING "OpenCV not found; features that depend on it will be disabled")
    endif()
endif()

if (ENABLE_TESSERACT)
    # 优先从用户指定的根目录查找（CMAKE_PREFIX_PATH 已含 Tesseract_ROOT）
    find_package(Tesseract QUIET CONFIG)
    find_package(Leptonica QUIET CONFIG)
    if (Tesseract_FOUND AND Leptonica_FOUND)
        target_link_libraries(MobaController PRIVATE Tesseract::libtesseract Leptonica::leptonica)
        add_definitions(-DENABLE_TESSERACT)
    else()
        message(WARNING "Tesseract/Leptonica not found; OCR will be disabled")
    endif()
endif()
if (ENABLE_OPENCV AND OpenCV_FOUND)
    target_include_directories(MobaController PRIVATE ${OpenCV_INCLUDE_DIRS})
    target_link_libraries(MobaController PRIVATE ${OpenCV_LIBS})
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
