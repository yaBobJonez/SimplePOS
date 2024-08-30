cmake_minimum_required(VERSION 3.19)

project(testQtQuick VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 6.5 REQUIRED COMPONENTS Qml Quick PrintSupport)
qt_standard_project_setup()
qt_policy(SET QTP0001 NEW)

qt_add_executable(SimplePOS
    main.cpp
    src/CartItem.cpp
    src/CartItemModel.cpp
    src/ReceiptPrinter.cpp
    src/Server.cpp
    include/CartItem.h
    include/CartItemModel.h
    include/ReceiptPrinter.h
    include/Server.h
)
qt_add_qml_module(SimplePOS
    URI app_SimplePOS
    VERSION 1.0
    QML_FILES
        ui/Main.qml
        ui/Checkout.qml
        ui/CheckoutSummary.qml
        ui/ItemsDatabase.qml
        ui/MerchantInfoDialog.qml
        ui/TaxConfigDialog.qml
)
target_include_directories(SimplePOS PRIVATE include)
set_target_properties(SimplePOS PROPERTIES
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)
target_link_libraries(SimplePOS
    PRIVATE Qt6::Quick Qt6::PrintSupport
)
qt_generate_deploy_qml_app_script(
    TARGET SimplePOS
    OUTPUT_SCRIPT pos_deploy_script
)
install(SCRIPT ${pos_deploy_script})

add_subdirectory(Scanner)
add_subdirectory(Terminal)
