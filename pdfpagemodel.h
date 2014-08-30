#ifndef PDFPAGEMODEL_H
#define PDFPAGEMODEL_H

#include <QtCore>

class PDFPage;

class PDFPageModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PageRoles {
        SizeRole = Qt::UserRole + 1,
        NumberRole,
        SelectionRole,
        HandleRole
    };

    PDFPageModel(QObject *parent = 0);

    QList<PDFPage*> _pages;

    PDFPage* page(int number);
    void setPages(QList<PDFPage*> pages);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // PDFPAGEMODEL_H
