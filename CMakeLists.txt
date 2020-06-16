cmake_minimum_required(VERSION 3.1)
project(webrtc-cpp-sample)

set(CMAKE_CXX_STANDARD 14)

include_directories("${LIBWEBRTC_PATH}/include")
include_directories("${LIBWEBRTC_PATH}/include/third_party/abseil-cpp")

link_directories("${LIBWEBRTC_PATH}/lib")
list(APPEND webrtc_libs "webrtc")

if(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
  # macOS
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_MAC=1 -DWEBRTC_POSIX=1")
  find_library(FOUNDATION Foundation)
  list(APPEND webrtc_libs ${FOUNDATION})

elseif(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
  # Linux
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DWEBRTC_LINUX=1 -DWEBRTC_POSIX=1")

  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")
  # set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D_GLIBCXX_USE_CXX11_ABI=0")
  list(APPEND webrtc_libs "pthread")
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -W -Wall -Wno-unused-parameter")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g")

add_executable(sample
  main.cpp
  )

target_link_libraries(sample
  ${webrtc_libs}
  )
