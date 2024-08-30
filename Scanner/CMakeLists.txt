qt_add_executable(app_Scanner
    main.cpp
)
qt_add_qml_module(app_Scanner
    URI testScanner
    VERSION 1.0
    QML_FILES
        Main.qml
    RESOURCES
        scanner.png
)
set_target_properties(app_Scanner PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(app_Scanner
    PRIVATE Qt6::Quick
)
