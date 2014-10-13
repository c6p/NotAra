#include <QtWidgets>
#include <QtCore>

#include <QQuickWindow>
#include <QQmlContext>
#include "pdfview.h"
#include "pdfpagemodel.h"
#include "pdfpage.h"


PDFView::PDFView(QQuickItem *parent) :
    QQuickItem(parent),
    _source(""), _currentPage(0),
    _zoom(1.0),
    _dpi(QApplication::desktop()->physicalDpiX(),
         QApplication::desktop()->physicalDpiY()),
    _numPages(0),
    _cursorMode(CursorMode::Cursor)
{
    //setFlags(QQuickItem::ItemClipsChildrenToShape | QQuickItem::ItemHasContents);
}

QUrl PDFView::source() const
{ return _source; }

int PDFView::currentPage() const
{ return _currentPage; }

qreal PDFView::zoom() const
{ return _zoom; }

int PDFView::numPages() const
{ return _numPages; }

const QPoint PDFView::dpi() const
{ return _dpi; }

QSize PDFView::pageSize() const
{ return _pageSize; }

PDFView::CursorMode PDFView::cursorMode() const
{ return _cursorMode; }

int PDFView::firstHandlePage() const
{ return _handlePages.first; }

QPointF PDFView::firstHandlePoint() const
{ return _handlePoints.first; }

int PDFView::secondHandlePage() const
{ return _handlePages.second; }

QPointF PDFView::secondHandlePoint() const
{ return _handlePoints.second; }


void PDFView::setCurrentPage(int currPage)
{
    if((currPage > (numPages() - 1)) || (currPage < 0)) {
        qDebug()<< "PDFView::setCurrentPage" << currPage
            << "\nMax page: " <<docPDF->numPages();
        return;
    }

    if(currPage != _currentPage) {
        _currentPage = currPage;
        emit currentPageChanged();
    } else {
        qDebug()<< "PDFView::setCurrentPage" << currPage << _currentPage;
        return;
    }
}

void PDFView::setZoom(qreal zoomPage)
{
    if(zoomPage != _zoom) {
        _zoom = std::max(0.25, std::min(qreal(4), zoomPage));
        emit zoomChanged();
    } else {
        qDebug()<< "PDFView::setCurrentPage" << zoomPage << _zoom;
        return;
    }
}

void PDFView::setCursorMode(CursorMode mode)
{
    if (_cursorMode != mode)
    {
        _cursorMode = mode;
        emit cursorModeChanged();
    }
}

void PDFView::setPageModel(PDFPageModel *model)
{
    _pageModel = model;
    int size =  _pageModel->rowCount(QModelIndex());
    int bottom = size ? size-1 : 0;
}

void PDFView::load(const QUrl &url)
{
    _source = url;
    _currentPage = 0;
    _loadPDF();
    emit sourceChanged();
}

void PDFView::setRect(int page1, QPointF p1, int page2, QPointF p2, bool asRect)
{
    page1 = std::max(page1, 0);
    page2 = std::max(page2, 0);
    // Clear selection
    for (int i=_rectPages.first; i<=_rectPages.second; i++)
    {
        _pageModel->page(i)->clearSelection();
    }
    emit _pageModel->dataChanged(_pageModel->index(_rectPages.first),
            _pageModel->index(_rectPages.second));

    // Make page1 <= page2
    _rectPages = std::minmax(page1, page2);
    // Swap if necessary, so p1 on topleft of p2
    if (page1 == page2)
    {
        if (p1.y() > p2.y())
            std::swap(p1, p2);
        else if (p1.y() == p2.y())
        {
            if (p1.x() > p2.x())
                std::swap(p1, p2);
        }
    }
    else if (page1 > page2)
        std::swap(p1, p2);
    _rectPoints = std::make_pair(p1, p2);
    qDebug()<< "PDFView::setRect" << _rectPages.first << _rectPoints.first
            << _rectPages.second << _rectPoints.second;

    _selectText(asRect);
}

void PDFView::_selectText(bool asRect)
{
    for (int i=_rectPages.first; i<=_rectPages.second; i++)
    {
        auto *p = _pageModel->page(i);
        auto size = p->pageSize();
        QPointF begin = i == _rectPages.first
                ? _rectPoints.first
                : QPointF(0, 0);
        QPointF end = i == _rectPages.second
                ? _rectPoints.second
                : QPointF(size.width(), size.height());
        p->selectMarked(begin, end, asRect);
    }

    bool noSelection = true;
    // Move first text handle
    for (int i=_rectPages.first; i<=_rectPages.second; i++)
    {
        auto sel = _pageModel->page(i)->selection();
        if (!sel.empty())
        {
            _handlePages.first = i;
            _handlePoints.first = sel.front().toRect().bottomLeft();
            noSelection = false;
            break;
        }
    }
    // No text selected, so no selection handle
    if (noSelection)
    {
        _handlePages.first = -1;
        _handlePages.second = -1;
        _handlePoints.first = QPoint();
        _handlePoints.second = QPoint();
    }
    else
    {
        // Move second text handle
        for (int i=_rectPages.second; i>=_rectPages.first; i--)
        {
            auto sel = _pageModel->page(i)->selection();
            if (!sel.empty())
            {
                _handlePages.second = i;
                _handlePoints.second = sel.back().toRect().bottomRight();
                break;
            }
        }
    }
    qDebug() << _rectPages.first << _rectPages.second;
    qDebug() << _handlePages.first << _handlePages.second;

    emit _pageModel->dataChanged(_pageModel->index(_rectPages.first),
            _pageModel->index(_rectPages.second));
}

void PDFView::_loadPDF()
{
    typedef Poppler::Document::RenderHint RenderHint;

    QString source = _source.toLocalFile();
    if(!QFile::exists(source)) {
        qDebug()<<"Not found file: "<<source;
        return;
    }

    docPDF = Poppler::Document::load(source);
    docPDF->setRenderHint(RenderHint::Antialiasing, true);
    docPDF->setRenderHint(RenderHint::TextAntialiasing, true);
    docPDF->setRenderHint(RenderHint::TextHinting, true);
    //docPDF->setRenderHint(RenderHint::TextSlightHinting, true);
    //docPDF->setRenderHint(RenderHint::ThinLineShape, true);
    _numPages = docPDF->numPages();
    _loadPages();
}

void PDFView::_loadPages()
{
    QList<PDFPage*> pages;
    int width=-1, height=-1;

    qDebug() << "PDFView::_loadPages BEGIN";
    for (int i=0; i<_numPages; i++)
    {
        auto page = new PDFPage(docPDF->page(i), i, _dpi);
        auto size = page->pageSize();
        if (width < size.width()) width = size.width();
        if (height < size.height()) height = size.height();
        pages.push_back(page);
    }
    qDebug() << "PDFView::_loadPages END";

    _pageSize = QSize(width, height);
    emit pageSizeChanged();
    qDebug() << "PDFView::_loadPages" << _pageSize;

    Q_CHECK_PTR(_pageModel);
    _pageModel->setPages(pages);
}

QImage PDFView::renderPage(int page) const
{
    QPoint res(_dpi * _zoom);
    qDebug() << "PDFView::renderPage" << page << "ZOOM:"
        << _zoom << "RES:" << res << "dpi";
    return _pageModel->page(page)->render(res.x(), res.y());
}



