#ifndef CLIP_H
#define CLIP_H

#include <QtWidgets>
#include <QJsonDocument>

class Clip
{
//    Q_OBJECT
//    Q_PROPERTY(QVariant data READ data NOTIFY dataChanged)
//    Q_PROPERTY(QString type READ type NOTIFY typeChanged)
//    Q_PROPERTY(QString rawText READ rawText WRITE setRawText NOTIFY rawTextChanged)

public:
    Clip(int id, QString type, QString data, int page1, int page2, QPointF p1, QPointF p2, QVariantList rects);
    Clip(int id, QString type, QString data, QJsonObject clipData);

    QVariant data();
    QString type();
    QString rawText();
    void setRawText(QString text);

//signals:
//    void typeChanged();
//    void dataChanged();
//    void rawTextChanged();

private:
    int _id;
    QString _type;
    QString _data;
    QJsonDocument _clipData;
    QString _rawText;
};

#endif // CLIP_H
