cmake_minimum_required(VERSION 2.8)
project(musiclights)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(PkgConfig REQUIRED)

pkg_check_modules(sndfile REQUIRED sndfile)
pkg_check_modules(ftdi REQUIRED libftdi1)

list(APPEND CMAKE_CXX_FLAGS "-std=c++11")

# "core" library -- log engine, dispatcher (core), abstract source and sink
add_library(core STATIC core.cpp sink.cpp source.cpp log.cpp options.cpp)
target_link_libraries(core pthread)

# "terminal" library -- POSIX terminal-based source and sink
add_library(terminal STATIC terminal.cpp sinks/terminal.cpp sources/terminal.cpp)

# "ftdi" library -- hardware sink using FTDI USB interface IC in bit-bang mode via libftdi1
add_library(ftdi STATIC sinks/ftdi_bitbang.cpp)
target_link_libraries(ftdi ${ftdi_LIBRARIES})

# "frequency" library -- source helper for doing periodic work
add_library(frequency STATIC sources/frequency.cpp)

# "soundfile" library -- sound file source
add_library(soundfile STATIC sources/soundfile.cpp)
target_link_libraries(soundfile frequency ${sndfile_LIBRARIES})

add_executable(musiclights main.cpp)
target_link_libraries(musiclights terminal ftdi soundfile core)