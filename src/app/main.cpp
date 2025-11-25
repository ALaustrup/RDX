#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQmlContext>
#include <QDir>
#include <QtQml>
#include "viewmodels/JobViewModel.h"
#include "viewmodels/LCMStatsViewModel.h"
#include "controllers/CompressionController.h"
#include "lcm/LCMManager.h"
#include "schemas/SchemaRegistry.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    
    // Initialize LCM and Schema Registry
    auto lcm = rdx::core::LCMManager::createDefault();
    rdx::core::SchemaRegistry schemaRegistry(*lcm);
    
    // Create view models
    JobViewModel jobViewModel;
    LCMStatsViewModel lcmStatsViewModel(*lcm);
    
    // Create controller
    CompressionController compressionController(*lcm, schemaRegistry, jobViewModel);
    
    // Register QML types
    qmlRegisterType<JobViewModel>("RDX", 1, 0, "JobViewModel");
    qmlRegisterType<LCMStatsViewModel>("RDX", 1, 0, "LCMStatsViewModel");
    qmlRegisterType<CompressionController>("RDX", 1, 0, "CompressionController");
    
    // Setup QML engine
    QQmlApplicationEngine engine;
    
    // Expose objects to QML
    engine.rootContext()->setContextProperty("jobViewModel", &jobViewModel);
    engine.rootContext()->setContextProperty("lcmStatsViewModel", &lcmStatsViewModel);
    engine.rootContext()->setContextProperty("compressionController", &compressionController);
    
    const QUrl url(QStringLiteral("qrc:/RdxMainWindow.qml"));
    
    // Add error handling for QML loading
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
            qCritical() << "Failed to load QML file:" << objUrl;
            QCoreApplication::exit(-1);
        }
    }, Qt::QueuedConnection);
    
    // Connect to warnings and errors
    QObject::connect(&engine, &QQmlApplicationEngine::warnings, 
                     [](const QList<QQmlError> &warnings) {
        for (const auto &warning : warnings) {
            qWarning() << "QML Warning:" << warning.toString();
        }
    });
    
    engine.load(url);
    
    // Check for errors after loading
    if (engine.rootObjects().isEmpty()) {
        qCritical() << "Failed to load QML. Root objects are empty.";
        return -1;
    }
    
    return app.exec();
}

