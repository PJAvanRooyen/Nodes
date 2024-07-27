set( TARGET_NAME EventSystem )

add_subdirectory( Events )

set( HEADERS
  Communicator.h
)

set( SOURCES
  Communicator.cpp
)

set( OBJ_FILES
    $<TARGET_OBJECTS:Events>
    )

add_library( ${TARGET_NAME} ${OBJ_FILES} ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Events)
