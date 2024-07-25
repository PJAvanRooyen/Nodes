set( TARGET_NAME Events )

set( HEADERS
  AbstractEvent.h
  EvNode.h
)

set( SOURCES
  AbstractEvent.cpp
)

add_library( ${TARGET_NAME} ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core )
