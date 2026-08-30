cmake_minimum_required(VERSION 3.16)

project(EasyHub VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Quick
    QuickControls2
    LinguistTools
    Network
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
    service/MRService.cpp
    service/MRService.h
    service/AccountManager.cpp
    service/AccountManager.h
    service/CredentialStore.cpp
    service/CredentialStore.h

    model/MergeRequest.h
    model/MRListModel.cpp
    model/MRListModel.h
    model/ProviderAccount.h
    model/ProviderType.h
    model/ProviderListModel.cpp
    model/ProviderListModel.h

    provider/IMRProvider.h
    provider/CodeArtsMRProvider.cpp
    provider/CodeArtsMRProvider.h
    provider/GitLabMRProvider.cpp
    provider/GitLabMRProvider.h
    provider/IAuthProvider.h
    provider/CodeArtsAuthProvider.cpp
    provider/CodeArtsAuthProvider.h
)

set_source_files_properties(
    pages/AppTheme.qml
    PROPERTIES
        QT_QML_SINGLETON_TYPE TRUE
)

qt_add_qml_module(appEasyHub
    URI EasyHub
    QML_FILES
        Main.qml
        pages/AppTheme.qml
        pages/LoginPage.qml
        pages/MRListPage.qml

        pages/login/CodeArtsLoginForm.qml
        pages/login/GitLabLoginForm.qml

        pages/MRList/MRDetailPanel.qml
        pages/MRList/ProviderSidebar.qml
    RESOURCES
        resources/icons/codearts.svg
        resources/icons/gitlab.svg
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
    PRIVATE
        Qt6::Quick
        Qt6::QuickControls2
        Qt6::Network

)

include(GNUInstallDirs)
install(TARGETS appEasyHub
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
