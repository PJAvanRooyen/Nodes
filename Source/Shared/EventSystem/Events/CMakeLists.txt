set( TARGET_NAME Events )

set( HEADERS
  AbstractEvent.h
  EvNode.h
  EvConnection.h
  EvInteraction.h
)

set( SOURCES
  AbstractEvent.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Shared)
