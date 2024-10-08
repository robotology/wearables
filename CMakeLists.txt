## SPDX-FileCopyrightText: Fondazione Istituto Italiano di Tecnologia (IIT)
## SPDX-License-Identifier: BSD-3-Clause


cmake_minimum_required(VERSION 3.16)

set(PROJECT_VERSION "1.10.0")

set (WEARABLES_PROJECT_NAME Wearables)

project(${WEARABLES_PROJECT_NAME} VERSION ${PROJECT_VERSION}
                  LANGUAGES CXX C)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(GNUInstallDirs)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Export all symbols in Windows
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS ON)

# Add a postfix to Windows libraries compiled in debug
if(MSVC)
    set(CMAKE_DEBUG_POSTFIX "d")
endif()

# Libraries type
if(MSVC)
    option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" OFF)
else()
    option(BUILD_SHARED_LIBS "Build libraries as shared as opposed to static" ON)
endif()

# Control where binaries and libraries are placed in the build folder.
# This simplifies tests running in Windows.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR}")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR}")

# Enable RPATH support for installed binaries and libraries
include(AddInstallRPATHSupport)
add_install_rpath_support(LIB_DIRS "${CMAKE_INSTALL_FULL_LIBDIR}"       # Libraries
                          BIN_DIRS "${CMAKE_INSTALL_FULL_BINDIR}"       # Binaries
                                   "${CMAKE_INSTALL_FULL_LIBDIR}/yarp"  # Plugins
                          INSTALL_NAME_DIR "${CMAKE_INSTALL_FULL_LIBDIR}"
                          USE_LINK_PATH)

include(InstallBasicPackageFiles)

#Function to automatize the process of creating python bindings
include(AddWearablesPythonModule)


# Tweak linker flags in Linux
if(UNIX AND NOT APPLE)
    get_filename_component(LINKER_BIN ${CMAKE_LINKER} NAME)
    if(${LINKER_BIN} STREQUAL "ld")
        set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -Wl,--unresolved-symbols=report-all")
    endif()
endif()

if(WIN32)
  #Add compile definition to suppress warning related to header <experimental/filesystem>
  add_compile_definitions(_SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING)
endif()

# Flag to enable Paexo wearable device
option(WEARABLES_COMPILE_PYTHON_BINDINGS "Flag that enables building the bindings" OFF)

# Flag to enable XSensSuit wearable device
find_package(XsensXME QUIET)
option(ENABLE_XsensSuit "Flag that enables building XsensSuit wearable device" ${XsensXME_FOUND})
if(ENABLE_XsensSuit)
  add_subdirectory(XSensMVN)
endif()

# Flag to enable Paexo wearable device
option(ENABLE_Paexo "Flag that enables building Paexo wearable device" OFF)

# Flag to enable SenseGlove wearable device
option(ENABLE_HapticGlove "Flag that enables building Haptic Sense Glove wearable device" OFF)

# Flag to enable IWearFrameVisualizer module
find_package(iDynTree QUIET)
option(ENABLE_FrameVisualizer "Flag that enables building IWearFrameVisualizer module" ${iDynTree_FOUND})

# Flag to enable IWearLogger device
find_package(robometry QUIET)
option(ENABLE_Logger "Flag that enables building Wearable Logger device" ${robometry_FOUND})

# Flag to enable ICub wearable device
option(ENABLE_ICub "Flag that enables building iCub wearable device" ${iDynTree_FOUND})

add_subdirectory(interfaces)
add_subdirectory(impl)

# Prepare YARP wrapper and devices
find_package(YARP 3.2 REQUIRED)
set(YARP_FORCE_DYNAMIC_PLUGINS ON)
yarp_configure_plugins_installation(wearable)

set(CMAKE_POSITION_INDEPENDENT_CODE ON)

add_subdirectory(msgs)
add_subdirectory(devices)
add_subdirectory(wrappers)
add_subdirectory(app)
add_subdirectory(modules)
add_subdirectory(bindings)


