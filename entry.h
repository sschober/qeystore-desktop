#ifndef ENTRY_H
#define ENTRY_H

#include <QObject>
#include <QSqlQuery>

class Entry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString url READ url WRITE setUrl NOTIFY urlChanged)
    Q_PROPERTY(QString username READ username WRITE setUsername NOTIFY usernameChanged)
    Q_PROPERTY(QString password READ password WRITE setPassword NOTIFY passwordChanged)

public:
    explicit Entry(QObject *parent = 0);
    explicit Entry(QSqlQuery *q);

    QString url() const { return m_url; }
    void    setUrl(QString url){ m_url = url; }

    QString username() const { return m_username; }
    void    setUsername(QString username ) { m_username = username; }

    QString password() const { return m_password; }
    void    setPassword(QString password) { m_password = password; }

    static Entry*    fromDb(QString url);
    static void     toDb(Entry *e);

signals:
    void urlChanged(QString);
    void usernameChanged(QString);
    void passwordChanged(QString);

public slots:

private:
    QString m_url;
    QString m_username;
    QString m_password;
};

#endif // ENTRY_H
