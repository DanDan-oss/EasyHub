cmake_minimum_required(VERSION 3.16)

project(EasyHub VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick
    LinguistTools
)

qt_standard_project_setup(
    REQUIRES 6.8
    I18N_SOURCE_LANGUAGE en
    I18N_TRANSLATED_LANGUAGES zh_CN
)

qt_add_executable(appEasyHub
    main.cpp
    service/AuthService.cpp
    service/AuthService.h
)

qt_add_qml_module(appEasyHub
    URI EasyHub
    QML_FILES
        Main.qml
        pages/LoginPage.qml
        pages/MRListPage.qml
        pages/MRDetailPanel.qml
)

qt_add_translations(appEasyHub
    TS_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/i18n
    NO_GENERATE_PLURALS_TS_FILE
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appEasyHub PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appEasyHub
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appEasyHub
    PRIVATE Qt6::Quick
)

include(GNUInstallDirs)
install(TARGETS appEasyHub
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
