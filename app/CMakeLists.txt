project(app LANGUAGES CXX)

include(ui/ui.cmake)
include(db/db.cmake)
include(view/view.cmake)
include(model/model.cmake)
include(utils/utils.cmake)
include(common/common.cmake)
include(../3thrd/3thrd.cmake)

file(GLOB UDISKS_SCANNER_SRC
        )

add_executable(udisks-scanner main.cpp res.qrc
        ${SINGLEAPP_GUI_SRC} ${COMMON_SRC} ${UI_SRC} ${UDISKS_SCANNER_SRC}
        ${VIEW_SRC} ${MODEL_SRC} ${DB_SRC} ${UTILS_SRC}
        )
target_link_libraries(udisks-scanner -lX11 -lsqlite3
        Qt5::Widgets Qt5::Network Qt5::X11Extras
        ${GIO_LIBRARIES})
target_include_directories(udisks-scanner
        PRIVATE ${CMAKE_SOURCE_DIR}/app
        PUBLIC
        ${Qt5Core_INCLUDE_DIRS}
        ${Qt5Widget_INCLUDE_DIRS} ${Qt5X11Extras_INCLUDE_DIRS}
        ${GIO_INCLUDE_DIRS}
        )

