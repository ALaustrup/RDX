#ifndef RDX_JOBVIEWMODEL_H
#define RDX_JOBVIEWMODEL_H

#include <QAbstractListModel>
#include <QObject>
#include <QString>
#include <QDateTime>
#include <vector>
#include <QVariant>
#include <QHash>
#include <QModelIndex>

enum class JobStatus {
    Queued,
    Running,
    Done,
    Failed
};

enum class JobOperation {
    Compress,
    Decompress
};

struct Job {
    QString item;
    JobOperation operation;
    JobStatus status;
    qint64 originalSize;
    qint64 compressedSize;
    double ratio;
    int schemaId;
    QString errorMessage;
    QDateTime startTime;
    QDateTime endTime;
};

class JobViewModel : public QAbstractListModel {
    Q_OBJECT
    
public:
    enum Roles {
        ItemRole = Qt::UserRole + 1,
        OperationRole,
        StatusRole,
        OriginalSizeRole,
        CompressedSizeRole,
        RatioRole,
        SchemaIdRole,
        ErrorMessageRole,
        StartTimeRole,
        EndTimeRole
    };
    
    explicit JobViewModel(QObject *parent = nullptr);
    
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QHash<int, QByteArray> roleNames() const override;
    
    Q_INVOKABLE void addJob(const QString& item, JobOperation operation);
    Q_INVOKABLE void updateJobStatus(int index, JobStatus status);
    Q_INVOKABLE void updateJobResult(int index, qint64 originalSize, qint64 compressedSize, int schemaId);
    Q_INVOKABLE void setJobError(int index, const QString& error);

private:
    std::vector<Job> jobs_;
};

#endif // RDX_JOBVIEWMODEL_H

