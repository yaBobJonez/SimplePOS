qt_add_executable(app_Terminal
    main.cpp
)
qt_add_qml_module(app_Terminal
    URI testTerminal
    VERSION 1.0
    QML_FILES
        Main.qml
    RESOURCES
        terminal.jpg
        card.png
)
set_target_properties(app_Terminal PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(app_Terminal
    PRIVATE Qt6::Quick
)
