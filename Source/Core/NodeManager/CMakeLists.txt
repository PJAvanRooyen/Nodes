set( TARGET_NAME NodeManager )

set( HEADERS
  NodeManager.h
  NodeHandler.h
  
  ConnectionHandler.h
  
  Interactions.h
  InteractionHandler.h
)

set( SOURCES
  NodeManager.cpp
  NodeHandler.cpp
  
  ConnectionHandler.cpp
  
  InteractionHandler.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Shared)

