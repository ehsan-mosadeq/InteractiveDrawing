cmake_minimum_required(VERSION 3.14)
project(interactive_drawing LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 COMPONENTS Core)
find_package(Qt6 COMPONENTS Gui)
find_package(Qt6 COMPONENTS Widgets)

qt_add_executable(interactive_drawing
    main.cpp
    window.cpp window.h
    renderarea.cpp renderarea.h
    DrawablesScene.cpp DrawablesScene.h
    Drawables.h Drawables.cpp
    MovableActor.cpp MovableActor.h
    DrawableActor.cpp DrawableActor.h
    DrawablesInit.h
    DrawablesContextMenu.h
    SceneMapper.h
    TextActor.h
)
set_target_properties(interactive_drawing PROPERTIES
    WIN32_EXECUTABLE TRUE
    MACOSX_BUNDLE TRUE
)
target_link_libraries(interactive_drawing PUBLIC
    Qt::Core
    Qt::Gui
    Qt::Widgets
)

qt6_add_resources(interactive_drawing "interactive_drawing"
    PREFIX
        "/"
    FILES
       "images/brick.png"
)