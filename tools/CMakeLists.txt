project(demo LANGUAGES CXX)

include(${CMAKE_SOURCE_DIR}/app/db/db.cmake)
include(${CMAKE_SOURCE_DIR}/app/view/view.cmake)
include(${CMAKE_SOURCE_DIR}/app/model/model.cmake)
include(${CMAKE_SOURCE_DIR}/app/utils/utils.cmake)

add_executable (demo-scan-task-bentch
    ${CMAKE_SOURCE_DIR}/tools/demo-scan-task-bentch.cpp
    ${MODEL_SRC} ${VIEW_SRC} ${UTILS_SRC} ${DB_SRC})
target_include_directories(demo-scan-task-bentch
        PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/app
        PUBLIC ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_libraries (demo-scan-task-bentch
    Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})

add_executable (demo-scan-result-bentch
        ${CMAKE_SOURCE_DIR}/tools/demo-scan-result-bentch.cpp
        ${MODEL_SRC} ${VIEW_SRC} ${UTILS_SRC} ${DB_SRC})
target_include_directories(demo-scan-result-bentch
        PRIVATE ${CMAKE_SOURCE_DIR} ${CMAKE_SOURCE_DIR}/app
        PUBLIC ${Qt5Gui_INCLUDE_DIRS} ${Qt5Widgets_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS} ${GIO_INCLUDE_DIRS})
target_link_libraries (demo-scan-result-bentch
        Qt5::Core Qt5::Network Qt5::Gui Qt5::Widgets ${SQLITE3_LIBRARIES} ${GIO_LIBRARIES})
