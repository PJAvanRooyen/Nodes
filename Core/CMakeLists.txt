set( TARGET_NAME Core )

add_subdirectory( NodeStore )
add_subdirectory( InteractionModule )

add_library( ${TARGET_NAME} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Shared)
