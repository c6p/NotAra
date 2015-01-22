#ifndef DATABASE_H
#define DATABASE_H

#include <QtSql>
#include "clip.h"

class Database
{
private:
    static Database* _db;
    QSqlDatabase _sql;

    Database();
    QSqlError init();
    QVariant _addDataType(QSqlQuery &q, int id, const QString &name);

public:
    static Database& get();
    QMap<QString, QString> Q;

    QVariant addClip(QSqlQuery &q, int resource, int datatype, const QString &data, const QJsonDocument &clipdata);
    QVariant addNote(QSqlQuery &q, const QString &rawText, const QString &richText);
    QVariant addResource(QSqlQuery &q, const QString &path);
    QVariant addTag(QSqlQuery &q, const QString &name);
    void delClip(QSqlQuery &q, int id);
    void delNote(QSqlQuery &q, int id);
    void delTag(QSqlQuery &q, int id);
    void delTagByName(QSqlQuery &q, const QString &name);

    int getResource(const QString &name);
    QList<Clip> getClips(const QString &path);

};

#endif // DATABASE_H
