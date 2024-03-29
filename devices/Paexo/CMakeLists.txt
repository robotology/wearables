# SPDX-FileCopyrightText: Fondazione Istituto Italiano di Tecnologia (IIT)
# SPDX-License-Identifier: BSD-3-Clause

# Flag to enable iFeelDriver in Paexo wearable device
option(ENABLE_PAEXO_USE_iFEELDriver "Flag that enables building Paexo wearable device with iFeelDriver" OFF)

if (ENABLE_PAEXO_USE_iFEELDriver)

    ## Find iFeelDriver
    find_package(iFeelDriver REQUIRED)

endif()

# Generate cmake config.file
configure_file (
  "${CMAKE_CURRENT_SOURCE_DIR}/conf/paexo-cmake-config.h.in"
  "${CMAKE_CURRENT_SOURCE_DIR}/include/paexo-cmake-config.h"
  @ONLY)

# Compile the plugin by default
yarp_prepare_plugin(paexo  TYPE wearable::devices::Paexo
                    INCLUDE include/Paexo.h
                    CATEGORY device
                    ADVANCED
                    DEFAULT ON
                    )

yarp_add_plugin(Paexo src/Paexo.cpp include/Paexo.h)

target_include_directories(Paexo PRIVATE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>)

target_link_libraries(Paexo
    PUBLIC
    YARP::YARP_dev
    YARP::YARP_init
    Wearable::IWear
    )

if (ENABLE_PAEXO_USE_iFEELDriver)
    target_link_libraries(Paexo PUBLIC iFeelDriver::iFeelDriver)
    message(STATUS "Paexo: Compiling with iFeelDriver")
endif()

yarp_install(TARGETS Paexo
             COMPONENT runtime
             LIBRARY DESTINATION ${YARP_DYNAMIC_PLUGINS_INSTALL_DIR}
             ARCHIVE DESTINATION ${YARP_STATIC_PLUGINS_INSTALL_DIR}
             YARP_INI DESTINATION ${YARP_PLUGIN_MANIFESTS_INSTALL_DIR}
             )
