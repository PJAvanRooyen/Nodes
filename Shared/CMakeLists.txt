set( TARGET_NAME Shared )

add_subdirectory( EventSystem )
add_subdirectory( Node )

add_library( ${TARGET_NAME} ${OBJ_FILES} EventSystem)
