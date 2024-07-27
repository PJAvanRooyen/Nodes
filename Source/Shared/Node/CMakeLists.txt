set( TARGET_NAME Node )

set( HEADERS
  Node.h
)

set( SOURCES
  Node.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS} )

target_include_directories( ${TARGET_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core )
