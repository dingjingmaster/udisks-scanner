cmake_minimum_required(VERSION 3.5)
project(udisks-scanner LANGUAGES C CXX)

set(DEBUG ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(PROJECT_VERSION_MAJOR 0)
set(PROJECT_VERSION_MINOR 0)
set(PROJECT_VERSION_PATCH 0)

set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_definitions(
        -W -g
        -D QT_NO_KEYWORDS
        -D QT_MESSAGELOGCONTEXT

        -D PACKAGE_NAME=\\"${PROJECT_NAME}\\"
        -D G_LOG_DOMAIN=\\"${PROJECT_NAME}\\"
        -D INSTALL_PATH=\\"/usr/local/ultrasec/\\"
        -D TASK_DB_LOCKER=\\"/tmp/.est.db.lock\\"
        -D IPC_PF_SOCKET=\\"/tmp/ultrasec_pf.sock\\"
        -D PACKAGE_VERSION=\\"${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}.${PROJECT_VERSION_PATCH}\\")

find_package (PkgConfig)
find_package(Qt5  COMPONENTS Widgets Network Gui REQUIRED)

pkg_check_modules(SQLITE3 REQUIRED sqlite3)
pkg_check_modules(Xlsx REQUIRED Qt5Xlsx REQUIRED)
pkg_check_modules(GIO REQUIRED gio-2.0 gio-unix-2.0 libgtop-2.0)

cmake_host_system_information(RESULT OS QUERY OS_NAME)
cmake_host_system_information(RESULT RELEASE QUERY OS_RELEASE)
execute_process(COMMAND uname -m OUTPUT_VARIABLE OS_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
execute_process(COMMAND bash -c "qmake --version | grep -i 'qt version' | awk '{print $4}' | tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE CUR_QT_VERSION)
execute_process(COMMAND bash -c "cat /etc/os-release|grep -E '^NAME'|awk -F'=' '{print $2}'|sed 's/\"//g'|tr '\n' ' '|sed 's/ //g'" OUTPUT_VARIABLE DISTRIB)

MESSAGE("")
MESSAGE(STATUS "OS name         : '${OS}'")
MESSAGE(STATUS "OS arch         : '${OS_ARCH}'")
MESSAGE(STATUS "OS distribute   : '${DISTRIB}'")
MESSAGE(STATUS "OS release      : '${RELEASE}'")
MESSAGE(STATUS "Qt Version      : '${CUR_QT_VERSION}'")
MESSAGE("")


include_directories(${CMAKE_SOURCE_DIR})
include_directories(${CMAKE_SOURCE_DIR}/app)

add_subdirectory(app)
add_subdirectory(tools)
