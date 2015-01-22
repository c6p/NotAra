#include <QtWidgets>
#include <QtCore>

#include <QQuickWindow>
#include <QQmlContext>
#include "pdfview.h"
#include "pdfpagemodel.h"
#include "pdfpage.h"
#include "database.h"
#include "clip.h"

PDFView::PDFView(QQuickItem *parent) :
    QQuickItem(parent),
    _source(""), _currentPage(0),
    _zoom(1.0),
    _dpi(QApplication::desktop()->physicalDpiX(),
         QApplication::desktop()->physicalDpiY()),
    _numPages(0),
    _cursorMode(CursorMode::Cursor)
{
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

QJsonDocument PDFView::clipData() const
{
    return _clipData;
}

void PDFView::setCurrentPage(int currPage)
{
    if((currPage > (numPages() - 1)) || (currPage < 0)) {
        return;
    }

    if(currPage != _currentPage) {
        _currentPage = currPage;
        emit currentPageChanged();
    } else {
        return;
    }
}

void PDFView::setZoom(qreal zoomPage)
{
    if(zoomPage != _zoom) {
        _zoom = std::max(0.25, std::min(qreal(4), zoomPage));
        emit zoomChanged();
    } else {
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

void PDFView::rectToggled(bool rect)
{
    if (rect)
        setCursorMode(CursorMode::Rectangle);
    else
        setCursorMode(CursorMode::Cursor);
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
    for (int i=_orderedPages.first; i<=_orderedPages.second; i++)
    {
        _pageModel->page(i)->clearText();
    }
    emit _pageModel->dataChanged(_pageModel->index(_orderedPages.first),
            _pageModel->index(_orderedPages.second));

    _pageModel->page(_clickPages.first)->clearRect();
    emit _pageModel->dataChanged(_pageModel->index(_clickPages.first),
            _pageModel->index(_clickPages.first));

    _clickPages.first = page1;
    _clickPages.second = page2;
    _clickPoints.first = p1;
    _clickPoints.second = p2;
    // Make page1 <= page2
    _orderedPages = std::minmax(page1, page2);
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
        if (p1.x() > p2.x()) {
            int tmp = p1.x();
            p1.setX(p2.x());
            p2.setX(tmp);
        }
    }
    else if (page1 > page2)
        std::swap(p1, p2);
    _orderedPoints = std::make_pair(p1, p2);
    qDebug()<< "PDFView::setRect" << _orderedPages.first << _orderedPoints.first
            << _orderedPages.second << _orderedPoints.second;

    if (asRect)
        _selectRect();
    else
        _selectText();
}

//QString PDFView::clipboardText() {}
//QImage PDFView::clipboardImage() {}
//
void PDFView::clipRect() {
    //if (asRect) {
        auto *p = _pageModel->page(_clickPages.first);
        p->clipRect();
    auto index = _pageModel->index(_clickPages.first);
    emit _pageModel->dataChanged(index, index);
    //} else {
        for (int i=_orderedPages.first; i<=_orderedPages.second; i++)
        {
            auto *p = _pageModel->page(i);
            p->clipText();
        }
    emit _pageModel->dataChanged(_pageModel->index(_orderedPages.first),
            _pageModel->index(_orderedPages.second));
    //}
    // emit clipped(data);
}

void PDFView::_selectRect()
{
    auto *p = _pageModel->page(_clickPages.first);
    auto size = p->pageSize();
    if (_clickPages.first == _clickPages.second)
        p->selectRect(_orderedPoints.first, _orderedPoints.second);
    else {
        qreal y1, y2;
        if (_clickPages.first < _clickPages.second) {
            y1 = _clickPoints.first.y();
            y2 = size.height();
        } else {
            y1 = 0.f;
            y2 = _clickPoints.first.y();
        }
        auto x = std::minmax(_clickPoints.first.x(), _clickPoints.second.x());
        qreal x1 = std::max((qreal)0.f, x.first);
        qreal x2 = std::min((qreal)size.width(), x.second);
        p->selectRect(QPointF(x1,y1), QPointF(x2,y2));
    }
    auto index = _pageModel->index(_clickPages.first);
    emit _pageModel->dataChanged(index, index);
}

void PDFView::_selectText()
{
    for (int i=_orderedPages.first; i<=_orderedPages.second; i++)
    {
        auto *p = _pageModel->page(i);
        auto size = p->pageSize();
        QPointF begin = i == _orderedPages.first
                ? _orderedPoints.first
                : QPointF(0, 0);
        QPointF end = i == _orderedPages.second
                ? _orderedPoints.second
                : QPointF(size.width(), size.height());
        p->selectText(begin, end);
    }

    emit _pageModel->dataChanged(_pageModel->index(_orderedPages.first),
            _pageModel->index(_orderedPages.second));
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

    for (int i=0; i<_numPages; i++)
    {
        auto page = new PDFPage(docPDF->page(i), i, _dpi);
        auto size = page->pageSize();
        if (width < size.width()) width = size.width();
        if (height < size.height()) height = size.height();
        pages.push_back(page);
    }

    _pageSize = QSize(width, height);
    emit pageSizeChanged();

    Q_CHECK_PTR(_pageModel);
    _pageModel->setPages(pages);
}
/*
void PDFView::_loadClips(const QString &path)
{
    auto &db = Database::get();
    auto clips = db.getClips(path);

    for (auto it=clips.begin(), e=clips.end(); it!=e; it++)
    {
        auto o = it->clipData().object();
        if (it->type() == SelectionMode::Rectangle)
        {
            auto *p = _pageModel->page(it->page1());
            p->addClipRect(it->id(), o.rect.toRect());
        }
        else
        {
            auto l = o.rect.toList();
            for (int i=it->page1; i<=it->page2; it++)
            {
                auto *p = _pageModel->page(i);
                p->addClipText(it->id(), l[i-it->page1]);
            }
        }
    }
}
*/
QImage PDFView::renderPage(int page) const
{
    QPoint res(_dpi * _zoom);
    qDebug() << "PDFView::renderPage" << page << "ZOOM:"
        << _zoom << "RES:" << res << "dpi";
    return _pageModel->page(page)->render(res.x(), res.y());
}



