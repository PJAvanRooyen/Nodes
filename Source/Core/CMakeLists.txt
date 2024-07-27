set( TARGET_NAME Core )

add_subdirectory( NodeManager )

add_library( ${TARGET_NAME} )

target_link_libraries( ${TARGET_NAME} )
