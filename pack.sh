#!/bin/bash

set -e

# shellcheck disable=SC2046
workDir=$(dirname $(realpath -- $0))
buildDir="$workDir/build-dir"

# 接下来工作：
#     1. 任务启动
#     2. 任务导出
#     3. 依赖动态库

# 编译
export PKG_CONFIG_PATH="/usr/local/lib/pkgconfig/"
cmake -S $workDir -B $buildDir -DCMAKE_PREFIX_PATH=/usr/local/lib/pkgconfig/

cd $buildDir && make -j8