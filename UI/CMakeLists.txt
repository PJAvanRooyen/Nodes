set( TARGET_NAME UI )

set( HEADERS
  mainwindow.h
  CentralWidget.h
  CentralWidgetView.h
  Scene.h
  GraphicsItem.h
  GraphicsConnection.h
  GraphicsNode.h
)

set( SOURCES
  mainwindow.cpp
  CentralWidget.cpp
  CentralWidgetView.cpp
  Scene.cpp
  GraphicsItem.cpp
  GraphicsConnection.cpp
  GraphicsNode.cpp
)

add_library( ${TARGET_NAME} STATIC ${SOURCES} ${HEADERS})

target_link_libraries( ${TARGET_NAME} Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Widgets Shared )
