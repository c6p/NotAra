#include <QtWidgets>
#include <QtCore>

#include <QQuickWindow>
#include <QQmlContext>
#include "pdfview.h"
#include "pdfmodel.h"
#include "pdfpage.h"


PDFView::PDFView(QQuickItem *parent) :
    QQuickItem(parent),
    _source(""), _currentPage(0),
    _zoom(1.0),
    _dpi(QApplication::desktop()->physicalDpiX(), QApplication::desktop()->physicalDpiY()),
    _numPages(0)
{
    setFlags(QQuickItem::ItemClipsChildrenToShape | QQuickItem::ItemHasContents);
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

void PDFView::setModel(PDFModel *model)
{
    _model = model;
    int size =  _model->rowCount(QModelIndex());
    int bottom = size ? size-1 : 0;
}

void PDFView::load(const QUrl &url)
{
    _source = url;
    _currentPage = 0;
    _loadPDF();
    emit sourceChanged();
}

void PDFView::selectText(int page1, QPointF p1, int page2, QPointF p2)
{
    for (int i=_selectPage.first; i<=_selectPage.second; i++)
    {
        _model->page(i)->clearSelection();
    }
    emit _model->dataChanged(_model->index(_selectPage.first),
            _model->index(_selectPage.second));
    _selectPage= std::minmax(page1, page2);

    if (page1 == page2) {
        if (p1.y() > p2.y())
            std::swap(p1, p2);
        else if (p1.y() == p2.y()) {
            if (p1.x() > p2.x())
                std::swap(p1, p2);
        }
    } else if (page1 > page2)
        std::swap(p1, p2);

    qDebug()<< "PDFView::selectText" << _selectPage.first << p1 << _selectPage.second << p2;
    for (int i=_selectPage.first; i<=_selectPage.second; i++)
    {
        auto *p = _model->page(i);
        auto size = p->pageSize();
        QPointF begin = i == _selectPage.first ? p1 : QPointF(0, 0);
        QPointF end = i == _selectPage.second ? p2 : QPointF(size.width(), size.height());
        p->selectMarked(begin, end);
    }
    emit _model->dataChanged(_model->index(_selectPage.first),
            _model->index(_selectPage.second));
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

    Q_CHECK_PTR(_model);
    _model->setPages(pages);
}

QImage PDFView::renderPage(int page) const
{
    QPoint res(_dpi * _zoom);
    qDebug() << "PDFView::renderPage" << page << "ZOOM:"
        << _zoom << "RES:" << res << "dpi";
    return _model->page(page)->render(res.x(), res.y());
}



