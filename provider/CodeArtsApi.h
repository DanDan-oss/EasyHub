#pragma once
#include <QString>
#include <QUrl>


namespace CodeArtsApi {

    // ====================== IAM ===========================
    inline QUrl regionalTokenUrl(const QString& region)
    {
        return QUrl(QString("https://iam.%1.myhuaweicloud.com/v3/auth/tokens").arg(region.trimmed()));
    }
    inline QUrl globalTokenUrl()
    {
        return  QUrl("https://iam.myhuaweicloud.com/v3/auth/tokens");
    }

    // ============= Merge Request =====================
    inline QUrl mergeRequestListUrl(const QString& region)
    {
        return QUrl(QString("https://codehub-ext.%1.myhuaweicloud.com/v4/merge-requests").arg(region.trimmed()));
    }
    inline QUrl mergeRequestDetailUrl(const QString& region, const QString& repositoryId, int iid)
    {
        return QUrl(QString("https://codehub-ext.%1.myhuaweicloud.com/v4/repositories/%2/merge-requests/%3").arg(region.trimmed()).arg(repositoryId).arg(iid));
    }
}
