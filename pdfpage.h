#ifndef PDFPAGE_H
#define PDFPAGE_H

#include <poppler-qt5.h>

class PDFPage
{
public:
    explicit PDFPage(Poppler::Page* page, int index, QPoint dpi);
    ~PDFPage();
    QImage render(int resX, int resY) const;

    int pageNumber() const;
    QSize pageSize() const;
    QList<QVariant> selection() const;

    void selectMarked(QPointF begin, QPointF end, bool asRect=false);
    void clearSelection();

    Poppler::Page* _page;
    QSize _pageSize;
    int _pageNumber;

    bool _listRequested = false;
    QList<Poppler::TextBox*> _textList;
    QList<QVariant> _selection;

};

#endif // PDFPAGE_H
