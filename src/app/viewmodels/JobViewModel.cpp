#include "viewmodels/JobViewModel.h"
#include <QDebug>
#include <QModelIndex>
#include <QDateTime>

JobViewModel::JobViewModel(QObject *parent)
    : QAbstractListModel(parent) {
}

int JobViewModel::rowCount(const QModelIndex &parent) const {
    Q_UNUSED(parent)
    return static_cast<int>(jobs_.size());
}

QVariant JobViewModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid() || index.row() >= static_cast<int>(jobs_.size())) {
        return QVariant();
    }
    
    const Job& job = jobs_[index.row()];
    
    switch (role) {
        case ItemRole:
            return job.item;
        case OperationRole:
            return static_cast<int>(job.operation);
        case StatusRole:
            return static_cast<int>(job.status);
        case OriginalSizeRole:
            return job.originalSize;
        case CompressedSizeRole:
            return job.compressedSize;
        case RatioRole:
            return job.ratio;
        case SchemaIdRole:
            return job.schemaId;
        case ErrorMessageRole:
            return job.errorMessage;
        case StartTimeRole:
            return job.startTime;
        case EndTimeRole:
            return job.endTime;
        default:
            return QVariant();
    }
}

QHash<int, QByteArray> JobViewModel::roleNames() const {
    QHash<int, QByteArray> roles;
    roles[ItemRole] = "item";
    roles[OperationRole] = "operation";
    roles[StatusRole] = "status";
    roles[OriginalSizeRole] = "originalSize";
    roles[CompressedSizeRole] = "compressedSize";
    roles[RatioRole] = "ratio";
    roles[SchemaIdRole] = "schemaId";
    roles[ErrorMessageRole] = "errorMessage";
    roles[StartTimeRole] = "startTime";
    roles[EndTimeRole] = "endTime";
    return roles;
}

void JobViewModel::addJob(const QString& item, JobOperation operation) {
    beginInsertRows(QModelIndex(), static_cast<int>(jobs_.size()), static_cast<int>(jobs_.size()));
    
    Job job;
    job.item = item;
    job.operation = operation;
    job.status = JobStatus::Queued;
    job.originalSize = 0;
    job.compressedSize = 0;
    job.ratio = 0.0;
    job.schemaId = -1;
    job.startTime = QDateTime::currentDateTime();
    
    jobs_.push_back(job);
    
    endInsertRows();
}

void JobViewModel::updateJobStatus(int index, JobStatus status) {
    if (index < 0 || index >= static_cast<int>(jobs_.size())) {
        return;
    }
    
    jobs_[index].status = status;
    if (status == JobStatus::Done || status == JobStatus::Failed) {
        jobs_[index].endTime = QDateTime::currentDateTime();
    }
    
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {StatusRole, EndTimeRole});
}

void JobViewModel::updateJobResult(int index, qint64 originalSize, qint64 compressedSize, int schemaId) {
    if (index < 0 || index >= static_cast<int>(jobs_.size())) {
        return;
    }
    
    jobs_[index].originalSize = originalSize;
    jobs_[index].compressedSize = compressedSize;
    jobs_[index].schemaId = schemaId;
    
    if (originalSize > 0) {
        jobs_[index].ratio = static_cast<double>(compressedSize) / static_cast<double>(originalSize);
    }
    
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {OriginalSizeRole, CompressedSizeRole, RatioRole, SchemaIdRole});
}

void JobViewModel::setJobError(int index, const QString& error) {
    if (index < 0 || index >= static_cast<int>(jobs_.size())) {
        return;
    }
    
    jobs_[index].errorMessage = error;
    jobs_[index].status = JobStatus::Failed;
    jobs_[index].endTime = QDateTime::currentDateTime();
    
    QModelIndex modelIndex = createIndex(index, 0);
    emit dataChanged(modelIndex, modelIndex, {StatusRole, ErrorMessageRole, EndTimeRole});
}

