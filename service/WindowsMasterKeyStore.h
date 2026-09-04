#pragma once

#include <optional>
#include <QByteArray>

class WindowsMasterKeyStore
{
public:
    std::optional<QByteArray> load() const;
    std::optional<QByteArray> loadOrCreate();
    void clear(QByteArray& key) const;
private:
    QByteArray  generate() const;
    bool save(const QByteArray& key) const;
};
