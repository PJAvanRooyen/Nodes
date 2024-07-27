set( TARGET_NAME Source )

add_subdirectory( Shared )
add_subdirectory( Core )
add_subdirectory( UI )

add_library( ${TARGET_NAME} )

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core )
