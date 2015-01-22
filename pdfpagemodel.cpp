#include "pdfpagemodel.h"
#include "pdfpage.h"

PDFPageModel::PDFPageModel(QObject *parent) : QAbstractListModel(parent)
{}

PDFPage* PDFPageModel::page(int number)
{
    return _pages[number];
}

void PDFPageModel::setPages(QList<PDFPage*> pages)
{
    int bottom = _pages.size() ? _pages.size()-1 : 0;
    beginRemoveRows(QModelIndex(), 0, bottom);
    endRemoveRows();

    _pages = pages;
    bottom = _pages.size() ? _pages.size()-1 : 0;
    beginInsertRows(QModelIndex(), 0, bottom);
    endInsertRows();

    qDebug() << "PDFPageModel::setPages " << 0 << "-" << bottom;
}

int PDFPageModel::rowCount(const QModelIndex &parent) const
{
    return _pages.size();
}

QVariant PDFPageModel::data(const QModelIndex &index, int role) const
{
    auto &p = _pages[index.row()];
    if (role == PageRoles::SizeRole)
        return QVariant(p->pageSize());
    else if (role == PageRoles::SelectionRole)
        return QVariant(p->selectedText());
    else if (role == PageRoles::RectRole)
        return QVariant(p->selectedRect());
    else if (role == PageRoles::HighlightRole)
        return QVariant(p->clippedTexts());
    else if (role == PageRoles::CropRole)
        return QVariant(p->clippedRects());
    else // PageRoles::NumberRole
        return QVariant(p->pageNumber());
}

QHash<int, QByteArray> PDFPageModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PageRoles::SizeRole] = "pageSize";
    roles[PageRoles::NumberRole] = "pageNumber";
    roles[PageRoles::SelectionRole] = "selection";
    roles[PageRoles::RectRole] = "selectionRect";
    roles[PageRoles::HighlightRole] = "highlights";
    roles[PageRoles::CropRole] = "clipRects";
    return roles;
}
