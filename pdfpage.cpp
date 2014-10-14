#include "pdfpage.h"

#include <QtCore>
#include <QtWidgets>

PDFPage::PDFPage(Poppler::Page *page, int index, QPoint dpi) :
    _page(page), _pageNumber(index),
    _pageSize(page->pageSize())
{
    qDebug() << "PDFPage" << _pageNumber << _page << _pageSize << page->pageSize();
}

PDFPage::~PDFPage()
{
    delete _page;
}

QImage PDFPage::render(int resX, int resY) const
{
    //QImage image = _page->renderToImage(resX, resY);
    QImage image = _page->renderToImage();
    qDebug() << "PDFPage::render" << _pageNumber << "at" << resX << resY
        << "dpi" << image.size() << "px";
    return image;
}

int PDFPage::pageNumber() const
{ return _pageNumber; }

QSize PDFPage::pageSize() const
{ return _pageSize; }

QList<QVariant> PDFPage::selection() const
{ return _selection; }

QRectF PDFPage::selectionRect() const
{ return _selectionRect; }

QList<QVariant> PDFPage::highlights() const
{ return _highlight; }

QList<QVariant> PDFPage::clipRects() const
{ return _clip; }

void PDFPage::clearText()
{
    _selection.clear();
}

void PDFPage::clearRect()
{
    _selectionRect.setCoords(0,0,0,0);
}

void PDFPage::clipRect()
{
    if (!_selectionRect.isValid())
        return;
    _clip.push_back(_selectionRect);
    qDebug() << _clip;
}

void PDFPage::clipText()
{
    if (_selection.isEmpty())
        return;
    _highlight.push_back(_selection);
    qDebug() << _highlight;
}


void PDFPage::selectRect(QPointF begin, QPointF end)
{
    _selectionRect.setTopLeft(begin);
    _selectionRect.setBottomRight(end);
    qDebug()<< "PDFPage::selectRect" <<  _selectionRect;
}

void PDFPage::selectText(QPointF begin, QPointF end)
{
    if (!_listRequested) {
        _textList = _page->textList();
        _listRequested = true;
    }
    QList<QVariant> rects;
    bool mark=false, nomark=false;
    QPointF topLeft, bottomRight;
    for (auto it=_textList.begin(), e=_textList.end(); it!=e; it++)
    {
        QRectF word = (*it)->boundingBox();
        // Find first
        if (!mark) {
            mark = word.top() > begin.y() || (word.bottom() > begin.y() && word.top() < begin.y() && word.right() > begin.x());
            // Begin selection
            if (mark) {
                topLeft = word.topLeft();
                bottomRight = word.bottomRight();
            }
        }
        // Continue selection
        else {
            nomark = word.top() > end.y() || (word.bottom() > end.y() && word.top() < end.y() && word.right() > end.x());
            if (word.bottom() > end.y() && word.top() < end.y())
                qDebug() << "END" << word.left() << end.x() << word.right();
            // New rectangle, newline or another column or else
            if (word.bottom() > bottomRight.y() || word.top() < topLeft.y()) {
                rects.push_back(QRectF(topLeft, bottomRight));
                topLeft = word.topLeft();
                bottomRight = word.bottomRight();
            } else {
                bottomRight = word.bottomRight();
            }
        }
        // Selection end
        if (nomark) {
            // Insert last rectangle
            if (! (word.top() > end.y()) )  // If not beyond endpoint
                rects.push_back(QRectF(topLeft, bottomRight));
            break;
        }
    }
    _selection = rects;
    qDebug()<< "PDFPage::selectText" <<  _selection;
}
