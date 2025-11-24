#include "viewmodels/LCMStatsViewModel.h"
#include <sstream>
#include <string>

LCMStatsViewModel::LCMStatsViewModel(rdx::core::LCMManager& lcm, QObject *parent)
    : QObject(parent)
    , lcm_(lcm)
    , totalFiles_(0)
    , totalCorpusSize_(0) {
    refresh();
}

void LCMStatsViewModel::refresh() {
    totalFiles_ = static_cast<qint64>(lcm_.getTotalFilesTracked());
    emit totalFilesChanged();
    
    totalCorpusSize_ = static_cast<qint64>(lcm_.getTotalCorpusSize());
    emit totalCorpusSizeChanged();
    
    // Get top schemas
    auto topSchemas = lcm_.getTopSchemasByUsage(5);
    std::ostringstream oss;
    for (const auto& [schemaId, count] : topSchemas) {
        oss << "Schema " << schemaId << ": " << count << " uses\n";
    }
    topSchemas_ = QString::fromStdString(oss.str());
    emit topSchemasChanged();
    
    // Get top file types
    auto topFileTypes = lcm_.getTopFileTypes(5);
    oss.str("");
    oss.clear();
    for (const auto& [fileTypeId, count] : topFileTypes) {
        std::string name = lcm_.getFileTypeName(fileTypeId);
        oss << name << ": " << count << " files\n";
    }
    topFileTypes_ = QString::fromStdString(oss.str());
    emit topFileTypesChanged();
}

