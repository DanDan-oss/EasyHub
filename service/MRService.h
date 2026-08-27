#pragma once

#include <QObject>
#include "../model/MRListModel.h"

class MRService : public QObject
{
    Q_OBJECT
    Q_PROPERTY(MRListModel* model READ model CONSTANT)

public:
    explicit MRService(QObject* parent = nullptr);
    MRListModel* model();
    Q_INVOKABLE void loadTestData();
private:
    MRListModel m_model;
};
