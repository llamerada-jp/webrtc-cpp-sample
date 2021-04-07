#!/bin/bash

set -eu

readonly ROOT_PATH=$(cd $(dirname $0) && pwd)

## Get OS environment parameters.
if [ "$(uname -s)" = "Darwin" ]; then
    # Mac OSX
    readonly ID="macos"
    readonly ARCH="x86_64"
    readonly IS_LINUX="false"

elif [ -e /etc/os-release ]; then
    . /etc/os-release
    readonly ARCH=`uname -p`
    readonly IS_LINUX="true"

else
    echo "Thank you for useing. But sorry, this platform is not supported yet."
    exit 1
fi

## Download libwebrtc (Compiled chromium WebRTC native APIs.)
readonly LOCAL_ENV_PATH=${ROOT_PATH}/local

mkdir -p ${LOCAL_ENV_PATH}/include
mkdir -p ${LOCAL_ENV_PATH}/src
cd ${LOCAL_ENV_PATH}/src

# Download and unarchive
if [ "${ID}" = "macos" ]; then
    readonly LIBWEBRTC_URL="https://github.com/llamerada-jp/libwebrtc/releases/download/m89/libwebrtc-89.0.4389.114-macos-amd64.zip"
    readonly LIBWEBRTC_ARCHIVE="$(basename ${LIBWEBRTC_URL})"
    if ! [ -e ${LIBWEBRTC_ARCHIVE} ]; then
        curl -OL ${LIBWEBRTC_URL}
        cd ${LOCAL_ENV_PATH}
        unzip -o src/${LIBWEBRTC_ARCHIVE}
    fi
else
    readonly LIBWEBRTC_URL="https://github.com/llamerada-jp/libwebrtc/releases/download/m89.2/libwebrtc-89.0.4389.114-linux-amd64.tar.gz"
    readonly LIBWEBRTC_ARCHIVE="$(basename ${LIBWEBRTC_URL})"
    if ! [ -e ${LIBWEBRTC_ARCHIVE} ]; then
      wget ${LIBWEBRTC_URL}
      cd ${LOCAL_ENV_PATH}
      tar zxf src/${LIBWEBRTC_ARCHIVE}
    fi
fi

## Build
# Change compiler to clang on linux
if [ "${IS_LINUX}" = "true" ]; then
    export CC=cc
    export CXX=c++
fi

readonly BUILD_PATH=${ROOT_PATH}/build
mkdir -p ${BUILD_PATH}

# cd ${ROOT_PATH}
# git submodule init
# git submodule update

cd ${BUILD_PATH}
cmake -DLIBWEBRTC_PATH=${LOCAL_ENV_PATH} ..
make
cp sample ${ROOT_PATH}
