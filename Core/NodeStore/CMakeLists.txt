set( TARGET_NAME NodeStore )

set( HEADERS
  NodeStore.h
  NodeManager.h
  ConnectionHandler.h
)

set( SOURCES
  NodeStore.cpp
  NodeManager.cpp
  ConnectionHandler.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Shared)

