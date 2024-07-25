set( TARGET_NAME EventSystem )

add_subdirectory( Events )

set( HEADERS
  Communicator.h
)

set( SOURCES
  Communicator.cpp
)

add_library( ${TARGET_NAME} ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Events )
