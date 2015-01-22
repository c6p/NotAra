#include "database.h"
//#include "clip.h"

Database* Database::_db = nullptr;

Database& Database::get()
{
    if (_db != nullptr)
        _db = new Database();
    return *_db;
}

QSqlError Database::init()
{
    if (!_sql.open())
         return _sql.lastError();

     QStringList tables = _sql.tables();
     if (tables.contains("resources", Qt::CaseInsensitive)
         && tables.contains("url", Qt::CaseInsensitive)
         && tables.contains("notes", Qt::CaseInsensitive)
         && tables.contains("clips", Qt::CaseInsensitive)
         && tables.contains("tags", Qt::CaseInsensitive)
         && tables.contains("note_tag", Qt::CaseInsensitive))
         return QSqlError();

     QSqlQuery q;
     if (!q.exec("CREATE TABLE resources(id INTEGER PRIMARY KEY,"
                 "path TEXT NOT NULL, title TEXT)"))
         return q.lastError();
// TODO different urls pointing to same file, file hash
//     if (!q.exec("CREATE TABLE resources(id INTEGER PRIMARY KEY,"
//                 "hash TEXT NOT NULL, path TEXT NOT NULL, title TEXT)"))
//         return q.lastError();
//     if (!q.exec("CREATE TABLE url(url TEXT PRIMARY_KEY NOT NULL,"
//                 "resource_id INTEGER REFERENCES resources(id)) NOT NULL ON DELETE CASCADE"))
//         return q.lastError();
     if (!q.exec("CREATE TABLE notes(id INTEGER PRIMARY KEY,"
                 "text_raw TEXT NOT NULL, text_rich TEXT NOT NULL,"
                 "created INTEGER NOT NULL, modified INTEGER NOT NULL)"))
         return q.lastError();
     if (!q.exec("CREATE TABLE datatype(id INTEGER PRIMARY KEY,"
                 "type TEXT)"))
         return q.lastError();
     if (!q.exec("CREATE TABLE clips(id INTEGER PRIMARY KEY NOT NULL AUTOINCREMENT,"
                 "resource_id INTEGER REFERENCES resources(id) NOT NULL ON DELETE CASCADE,"
                 "datatype INTEGER REFERENCES datatype(datatype_id) NOT NULL,"
                 "data TEXT, clipdata BLOB, created INTEGER NOT NULL,"
                  "modified INTEGER NOT NULL)"))
         return q.lastError();
     if (!q.exec("CREATE TABLE tags(id INTEGER PRIMARY KEY,"
                 "name TEXT NOT NULL)"))
         return q.lastError();
     if (!q.exec("CREATE TABLE note_tag("
                 "note_id INTEGER REFERENCES notes(id) NOT NULL ON DELETE CASCADE)"
                 "tag_id INTEGER REFERENCES tags(id) NOT NULL ON DELETE CASCADE,"))
         return q.lastError();

     // Insert clip datatypes
     if (!q.prepare("INSERT INTO datatype(id, type) values(?, ?)"))
         return q.lastError();
     _addDataType(q, 0, "plaintext");
     _addDataType(q, 1, "image");

     // Prepare statements
     Q["insertClip"] = "INSERT INTO clips(resources_id, datatype, data, clipdata, created, modified) values(?, ?, ?, ?, now, now)";
     Q["insertNote"] = "INSERT INTO notes(text_raw, text_rich, created, modified) values(?, ?, now, now)";
     Q["insertResource"] = "INSERT INTO resources(filename) values(?)";
     Q["insertTag"] = "INSERT INTO tags(name) values(?)";
     Q["insertNoteTag"] = "INSERT INTO note_tag(note_id, tag_id) values(?, ?)";
     Q["updateNote"] = "UPDATE notes text_raw=?, text_rich=?, modified=? WHERE id=?";
     Q["deleteClip"] = "DELETE FROM clips WHERE id=?";
     Q["deleteNote"] = "DELETE FROM notes WHERE id=?";
     Q["deleteTagId"] = "DELETE FROM tags WHERE id=?";
     Q["deleteTagName"] = "DELETE FROM tags WHERE name=?";
}

Database::Database()
{
    _sql = QSqlDatabase::addDatabase("QSQLITE");
    _sql.setDatabaseName(":memory:");

    init();
}

QVariant Database::_addDataType(QSqlQuery &q, int id, const QString &name)
{
    q.addBindValue(id);
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

QVariant Database::addClip(QSqlQuery &q, int resource, int datatype, const QString &data,
                      const QJsonDocument &clipdata)
{
    q.addBindValue(resource);
    q.addBindValue(datatype);
    q.addBindValue(data);
    q.addBindValue(clipdata.toBinaryData());
    q.exec();
    return q.lastInsertId();
}

QVariant Database::addNote(QSqlQuery &q, const QString &rawText, const QString &richText)
{
    q.addBindValue(rawText);
    q.addBindValue(richText);
    q.exec();
    return q.lastInsertId();
}

QVariant Database::addResource(QSqlQuery &q, const QString &path)
{
    q.addBindValue(path);
    q.exec();
    return q.lastInsertId();
}

QVariant Database::addTag(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
    return q.lastInsertId();
}

void Database::delClip(QSqlQuery &q, int id)
{
    q.addBindValue(id);
    q.exec();
}

void Database::delNote(QSqlQuery &q, int id)
{
    q.addBindValue(id);
    q.exec();
}

void Database::delTag(QSqlQuery &q, int id)
{
    q.addBindValue(id);
    q.exec();
}

void Database::delTagByName(QSqlQuery &q, const QString &name)
{
    q.addBindValue(name);
    q.exec();
}

int Database::getResource(const QString &name)
{
    QSqlQuery q;
    q.prepare("SELECT id FROM resources WHERE name = ?");
    q.addBindValue(name);
    q.exec();

    q.first();
    return q.value(0).toInt();
}

QList<Clip> Database::getClips(const QString &path)
{
    QSqlQuery q;
    int resource = getResource(path);
    if (resource < 0) {
        q.prepare(Q["insertResource"]);
        resource = _db->addResource(q, path).toInt();
    }
    q.prepare("SELECT c.id, c.type, c.data, c.clipdata, n.raw_text FROM clips AS c"
              " LEFT JOIN notes AS n ON c.id = n.id WHERE c.resource_id = ?");
    q.addBindValue(resource);
    q.exec();

    QList<Clip> clips;
    while (q.next()) {
        clips.push_back(Clip(q.value(0).toInt(), q.value(1).toString(),
                             q.value(2).toString(), q.value(3).toJsonObject()));
        clips.back().setRawText(q.value(4).toString());
    }
    return clips;
}

