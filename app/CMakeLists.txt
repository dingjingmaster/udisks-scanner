project(app LANGUAGES CXX)

include(ui/ui.cmake)
include(db/db.cmake)
include(ipc/ipc.cmake)
include(view/view.cmake)
include(model/model.cmake)
include(utils/utils.cmake)
include(common/common.cmake)
include(../3thrd/3thrd.cmake)

add_executable(udisks-scanner main.cpp res.qrc
        ${SINGLEAPP_GUI_SRC} ${COMMON_SRC} ${UI_SRC} ${UDISKS_SCANNER_SRC}
        ${VIEW_SRC} ${MODEL_SRC} ${DB_SRC} ${UTILS_SRC} ${IPC_SRC}
        )
#target_link_options(udisks-scanner
#        PUBLIC -static-libgcc -static-libstdc++
#        )
target_link_libraries(udisks-scanner
        -Wl,-rpath=./lib/
        -Wl,-rpath=../lib/
        -lX11
        ${GIO_LIBRARIES} ${SQLITE3_LIBRARIES}
        Qt5::Widgets Qt5::Network Qt5X11Extras
        )
target_include_directories(udisks-scanner
        PRIVATE ${CMAKE_SOURCE_DIR}/app
        PUBLIC
        ${Qt5Widget_INCLUDE_DIRS}
        ${GIO_INCLUDE_DIRS} ${SQLITE3_INCLUDE_DIRS}
        )

