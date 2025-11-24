#ifndef RDX_LCMSTATSVIEWMODEL_H
#define RDX_LCMSTATSVIEWMODEL_H

#include <QObject>
#include <QString>
#include "lcm/LCMManager.h"

class LCMStatsViewModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(qint64 totalFiles READ totalFiles NOTIFY totalFilesChanged)
    Q_PROPERTY(qint64 totalCorpusSize READ totalCorpusSize NOTIFY totalCorpusSizeChanged)
    Q_PROPERTY(QString topSchemas READ topSchemas NOTIFY topSchemasChanged)
    Q_PROPERTY(QString topFileTypes READ topFileTypes NOTIFY topFileTypesChanged)
    
public:
    explicit LCMStatsViewModel(rdx::core::LCMManager& lcm, QObject *parent = nullptr);
    
    qint64 totalFiles() const { return totalFiles_; }
    qint64 totalCorpusSize() const { return totalCorpusSize_; }
    QString topSchemas() const { return topSchemas_; }
    QString topFileTypes() const { return topFileTypes_; }
    
    Q_INVOKABLE void refresh();

signals:
    void totalFilesChanged();
    void totalCorpusSizeChanged();
    void topSchemasChanged();
    void topFileTypesChanged();

private:
    rdx::core::LCMManager& lcm_;
    qint64 totalFiles_;
    qint64 totalCorpusSize_;
    QString topSchemas_;
    QString topFileTypes_;
};

#endif // RDX_LCMSTATSVIEWMODEL_H

