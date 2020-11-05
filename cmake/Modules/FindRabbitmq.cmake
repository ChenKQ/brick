include(FindPackageHandleStandardArgs)

set(RABBITMQ_ROOT_DIR "" CACHE PATH "Folder contains rabbitmq-c library")

set(RABBITMQ_INCLUDE_SEARCH_PATHS
    /usr/include
    /usr/include/ffmpeg
    /usr/local/include
    /usr/local/include/ffmpeg
    ${RABBITMQ_ROOT_DIR}/include
    ${RABBITMQ_ROOT_DIR}/rabbitmq/include
    ${RABBITMQ_ROOT_DIR}/rabbitmq-c/include
)

set(RABBITMQ_LIB_SEARCH_PATHS
    /usr/lib
    /usr/local/lib
    ${RABBITMQ_ROOT_DIR}/lib
    ${RABBITMQ_ROOT_DIR}/rabbitmq/lib
    ${RABBITMQ_ROOT_DIR}/rabbitmq-c/lib
)

find_path(RABBITMQ_INCLUDE_DIR amqp.h PATHS ${RABBITMQ_INCLUDE_SEARCH_PATHS})

find_library(RABBITMQ_LIBRARY NAMES rabbitmq.4 PATHS ${RABBITMQ_LIB_SEARCH_PATHS})

find_package_handle_standard_args(  RABBITMQ
                                    DEFAULT_MSG
                                    RABBITMQ_INCLUDE_DIR
                                    RABBITMQ_LIBRARY
)

if(RABBITMQ_FOUND)
    set(RABBITMQ_INCLUDE_DIRS ${RABBITMQ_INCLUDE_DIR})
    set(RABBITMQ_LIBRARIES ${RABBITMQ_LIBRARY})
    mark_as_advanced(RABBITMQ_LIBRARIES RABBITMQ_INCLUDE_DIRS RABBITMQ_LIBRARY)
endif(RABBITMQ_FOUND)
