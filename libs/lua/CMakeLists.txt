aux_source_directory(. SRC_FILES)

project(lualib)

file(GLOB HEADERS "${PROJECT_SOURCE_DIR}/*.h")

source_group("Headers" FILES ${HEADERS})

add_library(
    lualib
    STATIC
    ${SRC_FILES}
    ${HEADERS}
)

target_link_libraries(
    lualib
    m # link math
)
