#include "controllers/CompressionController.h"
#include "compression/CompressionEngine.h"
#include "decompression/DecompressionEngine.h"
#include "container/RDXWriter.h"
#include "container/RDXReader.h"
#include "viewmodels/JobViewModel.h"
#include <QDebug>
#include <QFileInfo>
#include <QDir>
#include <QStringList>
#include <exception>

CompressionController::CompressionController(rdx::core::LCMManager& lcm,
                                             rdx::core::SchemaRegistry& schemaRegistry,
                                             JobViewModel& jobViewModel,
                                             QObject* parent)
    : QObject(parent)
    , lcm_(lcm)
    , schemaRegistry_(schemaRegistry)
    , jobViewModel_(jobViewModel) {
}

void CompressionController::compressFiles(const QStringList& filePaths, const QString& outputPath) {
    if (filePaths.isEmpty()) {
        return;
    }
    
    // Create archive
    std::filesystem::path archivePath = outputPath.toStdString();
    rdx::core::RDXWriter writer(archivePath);
    
    rdx::core::CompressionEngine engine(lcm_, schemaRegistry_);
    
    for (const QString& filePath : filePaths) {
        QFileInfo fileInfo(filePath);
        QString fileName = fileInfo.fileName();
        
        int jobIndex = jobViewModel_.rowCount();
        jobViewModel_.addJob(fileName, JobOperation::Compress);
        jobViewModel_.updateJobStatus(jobIndex, JobStatus::Running);
        
        try {
            std::filesystem::path inputPath = filePath.toStdString();
            writer.addFile(inputPath, engine, fileName.toStdString());
            
            auto entries = writer.getEntries();
            if (!entries.empty()) {
                const auto& entry = entries.back();
                jobViewModel_.updateJobResult(jobIndex,
                                            static_cast<qint64>(entry.originalSize),
                                            static_cast<qint64>(entry.compressedStructSize + entry.compressedResidualSize),
                                            entry.schemaId);
                jobViewModel_.updateJobStatus(jobIndex, JobStatus::Done);
            }
        } catch (const std::exception& e) {
            jobViewModel_.setJobError(jobIndex, QString::fromStdString(e.what()));
            jobViewModel_.updateJobStatus(jobIndex, JobStatus::Failed);
        }
    }
    
    writer.finalize();
}

void CompressionController::decompressArchive(const QString& archivePath, const QString& outputDir) {
    try {
        std::filesystem::path archive = archivePath.toStdString();
        rdx::core::RDXReader reader(archive);
        
        std::vector<rdx::core::RDXEntry> entries;
        reader.listEntries(entries);
        
        rdx::core::DecompressionEngine engine(lcm_, schemaRegistry_);
        
        QDir outputDirectory(outputDir);
        if (!outputDirectory.exists()) {
            outputDirectory.mkpath(".");
        }
        
        for (const auto& entry : entries) {
            QString fileName = QString::fromStdString(entry.fileName);
            int jobIndex = jobViewModel_.rowCount();
            jobViewModel_.addJob(fileName, JobOperation::Decompress);
            jobViewModel_.updateJobStatus(jobIndex, JobStatus::Running);
            
            try {
                std::filesystem::path outputPath = (outputDir + "/" + fileName).toStdString();
                reader.extractEntry(entry, outputPath, engine);
                
                jobViewModel_.updateJobResult(jobIndex,
                                            static_cast<qint64>(entry.originalSize),
                                            static_cast<qint64>(entry.compressedStructSize + entry.compressedResidualSize),
                                            entry.schemaId);
                jobViewModel_.updateJobStatus(jobIndex, JobStatus::Done);
            } catch (const std::exception& e) {
                jobViewModel_.setJobError(jobIndex, QString::fromStdString(e.what()));
            }
        }
    } catch (const std::exception& e) {
        qDebug() << "Decompression error:" << e.what();
    }
}

void CompressionController::onCompressionComplete(int jobIndex, qint64 originalSize, qint64 compressedSize, int schemaId) {
    jobViewModel_.updateJobResult(jobIndex, originalSize, compressedSize, schemaId);
    jobViewModel_.updateJobStatus(jobIndex, JobStatus::Done);
}

void CompressionController::onCompressionError(int jobIndex, const QString& error) {
    jobViewModel_.setJobError(jobIndex, error);
}

void CompressionController::compressFileAsync(const QString& filePath, const QString& archivePath, int jobIndex) {
    // Async implementation would go here
    // For now, synchronous implementation is used
}

