set( TARGET_NAME Shared )

add_subdirectory( EventSystem )
add_subdirectory( Node )

set( HEADERS
  StaticTypedefs.h
)

add_library( ${TARGET_NAME} )

target_include_directories( ${TARGET_NAME} PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}
)
