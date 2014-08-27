#ifndef PDFMODEL_H
#define PDFMODEL_H

#include <QtCore>

class PDFPage;

class PDFModel : public QAbstractListModel
{
    Q_OBJECT
public:
    enum PageRoles {
        SizeRole = Qt::UserRole + 1,
        NumberRole,
        SelectionRole,
        WidthRole,
        HeightRole
    };

    PDFModel(QObject *parent = 0);

    QList<PDFPage*> _pages;

    PDFPage* page(int number);
    void setPages(QList<PDFPage*> pages);
    int rowCount(const QModelIndex &parent) const;
    QVariant data(const QModelIndex &index, int role) const;
    QHash<int, QByteArray> roleNames() const;
};

#endif // PDFMODEL_H
