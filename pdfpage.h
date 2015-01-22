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
    QList<QVariant> selectedText() const;
    QRectF selectedRect() const;
    QList<QVariant> clippedTexts() const;
    QList<QVariant> clippedRects() const;

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
    QList<QVariant> _selectedText;
    QRectF _selectedRect;
    QList<QVariant> _clippedTexts;
    QList<QVariant> _clippedRects;

};

#endif // PDFPAGE_H
