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
    QRectF selectionRect() const;
    QList<QVariant> highlights() const;
    QList<QVariant> clipRects() const;

    void selectText(QPointF begin, QPointF end);
    void selectRect(QPointF begin, QPointF end);
    void clearText();
    void clearRect();
    void clipText();
    void clipRect();

    Poppler::Page* _page;
    QSize _pageSize;
    int _pageNumber;

    bool _asRect = false;
    bool _listRequested = false;
    QList<Poppler::TextBox*> _textList;
    QList<QVariant> _selection;
    QList<QVariant> _highlight;
    QList<QVariant> _clip;
    QRectF _selectionRect;

};

#endif // PDFPAGE_H
