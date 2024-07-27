set( TARGET_NAME InteractionModule )

set( HEADERS
  Interactions.h
  InteractionModule.h
  InteractionHandler.h
)

set( SOURCES
  InteractionModule.cpp
  InteractionHandler.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Shared)

