#!/bin/bash

# Copyright (C) 2025 Langning Chen
# 
# This file is part of miniapp.
# 
# miniapp is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
# 
# miniapp is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with miniapp.  If not, see <https://www.gnu.org/licenses/>.

mkdir -p ui/libs
mkdir -p dist

# Use the first available toolchain
toolchain=$(find jsapi/toolchains -mindepth 1 -maxdepth 1 -type d | head -n 1)

if [ -z "$toolchain" ]; then
    echo "Error: No toolchain found in jsapi/toolchains/"
    exit 1
fi

echo "Using toolchain: $toolchain"

export CROSS_TOOLCHAIN_PREFIX=$(find $(pwd)/$toolchain/bin -name "*buildroot*gcc" | head -n 1 | sed 's/gcc$//')

if [ -z "$CROSS_TOOLCHAIN_PREFIX" ]; then
    echo "Error: No suitable gcc compiler found in toolchain"
    exit 1
fi

echo "Using cross compiler prefix: $CROSS_TOOLCHAIN_PREFIX"

cmake -S jsapi -B jsapi/build
make -C jsapi/build -j $(nproc)
cp jsapi/build/libjsapi_langningchen.so ui/libs

pnpm -C ui package
cp $(find ui -name "800*.amr") dist/miniapp-$(basename $CROSS_TOOLCHAIN_PREFIX | sed 's/-$//').amr
