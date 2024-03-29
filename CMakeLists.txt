# Copyright: (C) 2016 iCub Facility - Istituto Italiano di Tecnologia
# Authors: Vadim Tikhanoff
# CopyPolicy: Released under the terms of the GNU GPL v2.0.

cmake_minimum_required(VERSION 3.5)
project(find-wally)

find_package(ICUBcontrib REQUIRED)
find_package(YARP 3.1.101 REQUIRED COMPONENTS OS sig cv)
find_package(OpenCV REQUIRED)
list(APPEND CMAKE_MODULE_PATH ${ICUBCONTRIB_MODULE_PATH})

include(ICUBcontribOptions)
include(ICUBcontribHelpers)

icubcontrib_set_default_prefix()

yarp_add_idl(IDL_GEN_FILES ${PROJECT_NAME}.thrift)

set(idl ${PROJECT_NAME}.thrift)
set(doc ${PROJECT_NAME}.xml)
source_group("IDL Files" FILES ${idl})
source_group("DOC Files" FILES ${doc})

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(${PROJECT_NAME} main.cpp ${doc} ${idl} ${IDL_GEN_FILES})
target_link_libraries(${PROJECT_NAME} ${YARP_LIBRARIES} ${OpenCV_LIBS})

install(TARGETS ${PROJECT_NAME} DESTINATION bin)

# generate ad-hoc project to perform "make uninstall"
icubcontrib_add_uninstall_target()

yarp_install(FILES ${doc} DESTINATION ${ICUBCONTRIB_MODULES_INSTALL_DIR})

add_subdirectory(app)
