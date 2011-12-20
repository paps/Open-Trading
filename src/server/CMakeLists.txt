# The Open Trading Project - open-trading.org
#
# Copyright (c) 2011 Martin Tapia - martin.tapia@open-trading.org
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without
# modification, are permitted provided that the following conditions are
# met:
#
#    * Redistributions of source code must retain the above copyright
# notice, this list of conditions and the following disclaimer.
#
#    * Redistributions in binary form must reproduce the above
# copyright notice, this list of conditions and the following disclaimer
# in the documentation and/or other materials provided with the
# distribution.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
# "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
# LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
# A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
# OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
# SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
# LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
# DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
# THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
# (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
# OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
# flags

set(CMAKE_CXX_CUSTOM_FLAGS "-Wall -Wextra -std=c++0x")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${CMAKE_CXX_CUSTOM_FLAGS}")

# qt stuff
find_package(Qt4 COMPONENTS QtCore QtGui QtNetwork REQUIRED)
set(QT_USE_QTNETWORK TRUE)
file(GLOB qthpp "qt/*.hpp")
file(GLOB qtforms "ui/*.ui")
file(GLOB qtres "res/*.qrc")

# server
file(GLOB server_src "*.[ch]pp")
file(GLOB server_qt_src "qt/*.[ch]pp")

# logger
file(GLOB logger_src "../logger/*.[ch]pp")

# core
file(GLOB core_src "../core/*.[ch]pp")
file(GLOB core_strategy_src "../core/strategy/*.[ch]pp")
file(GLOB core_signal_src "../core/signal/*.[ch]pp")
file(GLOB core_actor_src "../core/actor/*.[ch]pp")
file(GLOB core_indicator_src "../core/indicator/*.[ch]pp")

# conf
file(GLOB conf_src "../conf/*.[ch]pp")

# lua
include(FindLua51)
include(${CMAKE_SOURCE_DIR}/cmake/FindLuabind.cmake)
file(GLOB lua_src "../lua/*.[ch]pp")

# packet
file(GLOB packet_src "../packet/*.[ch]pp")

# some tools
file(GLOB timetostring_src "../tools/TimeToString.[ch]pp")
file(GLOB tostring_src "../tools/ToString.hpp")

# sound
include(${CMAKE_SOURCE_DIR}/cmake/FindFMODEx.cmake)
if (FMODEX_FOUND)
    file(GLOB sound_src "../sound/*.[ch]pp")
    file(WRITE CMakeFmodEx.hpp "#ifndef __SERVER_CMAKEFMODEX__\n"
        "#define __SERVER_CMAKEFMODEX__\n\n"
        "#define CMAKE_FMODEX \"${FMODEX_LIBRARY}\"\n\n"
        "#endif\n")
else (FMODEX_FOUND)
    file(WRITE CMakeFmodEx.hpp "#ifndef __SERVER_CMAKEFMODEX__\n"
        "#define __SERVER_CMAKEFMODEX__\n\n"
        "#endif\n")
endif (FMODEX_FOUND)

# boost (header-only libraries, and thread)
find_package(Boost COMPONENTS thread REQUIRED)

include(${QT_USE_FILE})
include_directories(
    ${CMAKE_CURRENT_BINARY_DIR}
    ${CMAKE_CURRENT_SOURCE_DIR}
    ${LUA_INCLUDE_DIR}
    ${LUABIND_INCLUDE_DIR}
    ${Boost_INCLUDE_DIR}
)
if (FMODEX_FOUND)
    include_directories(${FMODEX_INCLUDE_DIR})
endif (FMODEX_FOUND)

qt4_wrap_cpp(src ${qthpp})
qt4_wrap_ui(src ${qtforms})
qt4_add_resources(src ${qtres})
add_definitions(${QT_DEFINITIONS})

list(APPEND src
    ${server_src}
    ${server_qt_src}
    ${logger_src}
    ${conf_src}
    ${lua_src}
    ${packet_src}
    ${core_src}
    ${core_strategy_src}
    ${core_signal_src}
    ${core_actor_src}
    ${core_indicator_src}
    ${timetostring_src}
    ${tostring_src}
)
if (FMODEX_FOUND)
    list(APPEND src ${sound_src})
endif (FMODEX_FOUND)

add_executable(server ${src} ${qtres})

target_link_libraries(server
    ${QT_LIBRARIES}
    ${LUA_LIBRARIES}
    ${LUABIND_LIBRARY}
    ${Boost_LIBRARIES}
)
if (FMODEX_FOUND)
    target_link_libraries(server ${FMODEX_LIBRARY})
endif (FMODEX_FOUND)
