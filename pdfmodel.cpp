#include "pdfmodel.h"
#include "pdfpage.h"

PDFModel::PDFModel(QObject *parent) : QAbstractListModel(parent)
{}

PDFPage* PDFModel::page(int number)
{
    return _pages[number];
}

void PDFModel::setPages(QList<PDFPage*> pages)
{
    int bottom = _pages.size() ? _pages.size()-1 : 0;
    beginRemoveRows(QModelIndex(), 0, bottom);
    endRemoveRows();

    _pages = pages;
    bottom = _pages.size() ? _pages.size()-1 : 0;
    beginInsertRows(QModelIndex(), 0, bottom);
    endInsertRows();

    qDebug() << "PDFModel::setPages " << 0 << "-" << bottom;
}

int PDFModel::rowCount(const QModelIndex &parent) const
{
    return _pages.size();
}

QVariant PDFModel::data(const QModelIndex &index, int role) const
{
    auto &p = _pages[index.row()];
    if (role == PageRoles::SizeRole)
        return QVariant(p->pageSize());
    else if (role == PageRoles::SelectionRole)
        return QVariant(p->selection());
    else // PageRoles::NumberRole
        return QVariant(p->pageNumber());
}

QHash<int, QByteArray> PDFModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PageRoles::SizeRole] = "pageSize";
    roles[PageRoles::NumberRole] = "pageNumber";
    roles[PageRoles::SelectionRole] = "selection";
    return roles;
}
