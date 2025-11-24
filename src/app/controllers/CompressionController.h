#ifndef RDX_COMPRESSIONCONTROLLER_H
#define RDX_COMPRESSIONCONTROLLER_H

#include <QObject>
#include <QThread>
#include <QStringList>
#include <filesystem>
#include "lcm/LCMManager.h"
#include "schemas/SchemaRegistry.h"
#include "viewmodels/JobViewModel.h"

namespace rdx::core {
    class CompressionEngine;
    class DecompressionEngine;
}

class CompressionController : public QObject {
    Q_OBJECT
    
public:
    CompressionController(rdx::core::LCMManager& lcm,
                         rdx::core::SchemaRegistry& schemaRegistry,
                         JobViewModel& jobViewModel,
                         QObject* parent = nullptr);
    
    Q_INVOKABLE void compressFiles(const QStringList& filePaths, const QString& outputPath);
    Q_INVOKABLE void decompressArchive(const QString& archivePath, const QString& outputDir);

signals:
    void compressionProgress(int jobIndex, int percentage);
    void compressionComplete(int jobIndex);
    void compressionError(int jobIndex, const QString& error);

private slots:
    void onCompressionComplete(int jobIndex, qint64 originalSize, qint64 compressedSize, int schemaId);
    void onCompressionError(int jobIndex, const QString& error);

private:
    rdx::core::LCMManager& lcm_;
    rdx::core::SchemaRegistry& schemaRegistry_;
    JobViewModel& jobViewModel_;
    
    void compressFileAsync(const QString& filePath, const QString& archivePath, int jobIndex);
};

#endif // RDX_COMPRESSIONCONTROLLER_H

