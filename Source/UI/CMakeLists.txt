set( TARGET_NAME UI )

set( HEADERS
  mainwindow.h
  NodeArea.h
  NodeAreaView.h
  Scene.h
  GraphicsItem.h
  GraphicsConnection.h
  GraphicsNode.h
)

set( SOURCES
  mainwindow.cpp
  NodeArea.cpp
  NodeAreaView.cpp
  Scene.cpp
  GraphicsItem.cpp
  GraphicsConnection.cpp
  GraphicsNode.cpp
)

add_library( ${TARGET_NAME} ${SOURCES} ${HEADERS})

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Shared )
