#!/bin/bash
export CC=${DEVKITARM}/bin/arm-none-eabi-gcc
export CXX=${DEVKITARM}/bin/arm-none-eabi-g++

export CFLAGS="-Wall -O0 -mword-relocations -fomit-frame-pointer -ffunction-sections -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -I/home/stary/devkitPro/libctru/include -g -lctru"
export CXXFLAGS="-Wall -O0 -mword-relocations -fomit-frame-pointer -ffunction-sections -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft -I/home/stary2001/develop/3ds/application/include -I/home/stary/devkitPro/libctru/include -fno-rtti -fno-exceptions -std=gnu++11 -g"
export LDFLAGS="-L/home/stary/devkitPro/libctru/lib -specs=3dsx.specs -g -march=armv6k -mtune=mpcore -mfloat-abi=hard -mtp=soft"
# ./configure --with-3ds --disable-shared --enable-static --host=arm-none-eabi --disable-show-ips
# --enable-repeat-speedups --enable-handlers-chaining --enable-trace-linking