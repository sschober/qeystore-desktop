#include "entry.h"
#include <QSqlQuery>
#include <QSqlResult>

Entry::Entry(QObject *parent) :
    QObject(parent)
{
}

Entry::Entry(QSqlQuery *q){
    if(q->isSelect() && q->isActive() && q->next()){
        m_url = q->value(0).toString();
        m_username = q->value(1).toString();
        m_password = q->value(2).toString();
    }
}

Entry* Entry::fromDb(QString url){
    QSqlQuery q;
    q.prepare("SELECT * from passwords where url=:url");
    q.bindValue(":url",url);
    if(q.exec()){
        return new Entry(&q);
    }
    return NULL;
}
