cmake_minimum_required(VERSION 3.1)

set(CMAKE_CXX_STANDARD 11)

include_directories("${LIBWEBRTC_PATH}/include")
include_directories("${LIBWEBRTC_PATH}/include/third_party/abseil-cpp")

link_directories("${LIBWEBRTC_PATH}/lib")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # macOS
  file(READ ${LIBWEBRTC_PATH}/exports_libwebrtc.txt webrtc_libs)
  string(REGEX REPLACE "lib([^.]+).a[\r\n]*" "\\1;" webrtc_libs "${webrtc_libs}")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_MAC=1 -DWEBRTC_POSIX=1")
  find_library(CORE_FOUNDATION CoreFoundation)
  list(APPEND webrtc_libs ${CORE_FOUNDATION})
  find_library(FOUNDATION Foundation)
  list(APPEND webrtc_libs ${FOUNDATION})
  find_library(CORE_AUDIO CoreAudio)
  list(APPEND webrtc_libs ${CORE_AUDIO})
  find_library(AUDIO_TOOLBOX AudioToolbox)
  list(APPEND webrtc_libs ${AUDIO_TOOLBOX})
  find_library(CORE_GRAPHICS CoreGraphics)
  list(APPEND webrtc_libs ${CORE_GRAPHICS})

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux
  find_package(PkgConfig)

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")

  list(APPEND webrtc_libs "-Wl,--start-group")
  file(READ ${LIBWEBRTC_PATH}/exports_libwebrtc.txt webrtc_read_tmp)
  string(REGEX REPLACE "lib([^.]+).a[\r\n]*" "\\1;" webrtc_read_tmp "${webrtc_read_tmp}")
  list(APPEND webrtc_libs "${webrtc_read_tmp}")
  list(APPEND webrtc_libs "-Wl,--end-group")

  pkg_search_module(X11 REQUIRED x11)
  list(APPEND webrtc_libs ${X11_LIBRARIES})
  list(APPEND webrtc_libs "dl")
  list(APPEND webrtc_libs "pthread")

  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_LINUX=1 -DWEBRTC_POSIX=1")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

# Disable RTTI
set_source_files_properties(main.cpp PROPERTIES
  COMPILE_FLAGS " -fno-rtti "
  )

add_executable(sample
  main.cpp
  )

target_link_libraries(sample
  ${webrtc_libs}
  )
