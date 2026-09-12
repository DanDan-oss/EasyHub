#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QTranslator>
#include <QQuickStyle>
#include <QTimer>

#include "service/AuthService.h"
#include "service/MRService.h"
#include "service/AccountManager.h"
#include "model/ProviderListModel.h"
#include "provider/CodeArtsMRProvider.h"
#include "provider/GitLabMRProvider.h"
#include "provider/CodeArtsAuthProvider.h"
#include "service/CredentialCipher.h"
#include "service/WindowsMasterKeyStore.h"


int main(int argc, char *argv[])
{
    QQuickStyle::setStyle("Basic");
    QGuiApplication app(argc, argv);

    // 加载中文翻译
    QTranslator translator;
    if(translator.load(":/i18n/EasyHub_zh_CN.qm"))
    {
        app.installTranslator(&translator);
        qDebug() << "Chinese translator load Success.";
    }else
    {
        qDebug() << "Chinese translator load Failed.";
    }

    QQmlApplicationEngine engine;
    CredentialStore credentialStore;
    CredentialCache credentialCache;
    AccountManager accountManager;
    AuthService authService(&accountManager, &credentialStore, &credentialCache);
    MRService mrService(&accountManager);
    CodeArtsMRProvider codeArtsProvider(&credentialStore, &accountManager);
    ProviderListModel providerListModel(&accountManager);
    CodeArtsAuthProvider codeArtsAuthProvider;
    //GitLabMRProvider gitLabProvider;

    const bool registered = authService.registerProvider(&codeArtsAuthProvider);

    qDebug() << "CodeArts auth provider registered: " << registered;
    mrService.registerProvider(&codeArtsProvider);
    //mrService.registerProvider(&gitLabProvider);
    engine.rootContext()->setContextProperty("authService", &authService);
    engine.rootContext()->setContextProperty("providerModel", &providerListModel);
    engine.rootContext()->setContextProperty("mrService", &mrService);

    QObject::connect(
        &engine,
        &QQmlApplicationEngine::objectCreationFailed,
        &app,
        []() { QCoreApplication::exit(-1); },
        Qt::QueuedConnection);
    engine.loadFromModule("EasyHub", "Main");
    QTimer::singleShot(0, &authService, &AuthService::restoreSessions);

    return app.exec();
}
