#include "clip.h"

Clip::Clip(int id, QString type, QString data, int page1, int page2, QPointF p1, QPointF p2, QVariantList rects)
    : _id(id), _type(type), _data(data)
{
    QJsonObject obj;
    obj.insert("type", _type);
    obj.insert("page1", page1);
    obj.insert("page2", page2);
    obj.insert("p1x", p1.x());
    obj.insert("p1y", p1.y());
    obj.insert("p2x", p2.x());
    obj.insert("p2y", p2.y());
    QJsonArray rectArray;
    for (auto r=rects.begin(), re=rects.end(); r!=re; r++)
    {
        QRectF rect = r->toRectF();
        QJsonObject rectObj;
        rectObj.insert("x", rect.x());
        rectObj.insert("y", rect.y());
        rectObj.insert("w", rect.width());
        rectObj.insert("h", rect.height());
        rectArray.append(rectObj);
    }
    obj.insert("rects", rectArray);
    _clipData.setObject(obj);
}

Clip::Clip(int id, QString type, QString data, QJsonObject obj)
    : _id(id), _type(type), _data(data)
{
    _clipData.setObject(obj);
}

QVariant Clip::data()
{ return _data; }

QString Clip::type()
{ return _type; }

QString Clip::rawText()
{ return _rawText; }

void Clip::setRawText(QString text)
{
    _rawText = text;
}
