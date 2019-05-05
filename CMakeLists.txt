cmake_minimum_required(VERSION 3.10)
project(projet_rsa_dedonato_joly C)

####################################
# Find JSON-C
####################################
find_path(JSONC_INCLUDE_DIR
        NAMES json.h
        PATH_SUFFIXES include json-c
        )

find_library(JSONC_LIBRARY
        NAMES json-c libjson-c
        PATH_SUFFIXES lib
        )

####################################
# Find SQLITE 3
####################################
find_path(SQLITE3_INCLUDE_DIR
        NAMES sqlite3.h
        PATH_SUFFIXES include
        )

find_library(SQLITE3_LIBRARY
        NAMES sqlite3
        PATH_SUFFIXES lib
        )


include_directories(${SQLITE3_INCLUDE_DIR})
include_directories(${JSONC_INCLUDE_DIR})
include_directories(src/client)
include_directories(src/server)

file(GLOB SERVER_FILES src/server/*.c src/server/*.h)

add_executable(server
    ${SERVER_FILES}
)

target_link_libraries(server ${JSONC_LIBRARY} ${SQLITE3_LIBRARY})
target_compile_options(server PRIVATE -Werror -Wall -Wextra -Wno-deprecated-declarations)

file(GLOB CLIENT_FILES src/client/*.c src/client/*.h)

add_executable(client
    ${CLIENT_FILES}
)

target_link_libraries(client ${JSONC_LIBRARY})
target_compile_options(client PRIVATE -Werror -Wall -Wextra -Wno-deprecated-declarations)