#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QQuickItem>
#include <QQuickImageProvider>
#include <poppler-qt5.h>
#include <QtSql>

class PDFPageModel;


class PDFView : public QQuickItem
{
    Q_OBJECT
    Q_ENUMS(CursorMode)
    Q_PROPERTY(QSize pageSize READ pageSize NOTIFY pageSizeChanged)
    //Q_PROPERTY(QUrl source READ source WRITE load NOTIFY sourceChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QPointF dpi READ dpi CONSTANT)
    Q_PROPERTY(int numPages READ numPages)
    Q_PROPERTY(CursorMode cursorMode READ cursorMode WRITE setCursorMode NOTIFY cursorModeChanged)

public:
    explicit PDFView(QQuickItem *parent = 0);

    enum CursorMode {
        Cursor,
        Rectangle
    };

    Poppler::Document* docPDF;
    PDFPageModel *_pageModel;
    QUrl _source;
    CursorMode _cursorMode;
    QJsonDocument _clipData;
    int _currentPage;
    qreal _zoom;
    const QPoint _dpi;
    int _numPages;
    QSize _pageSize;

    std::pair<int, int> _orderedPages, _clickPages;
    std::pair<QPointF, QPointF> _orderedPoints, _clickPoints;

    QSize pageSize() const;
    QUrl source() const;
    int currentPage() const;
    qreal zoom() const;
    const QPoint dpi() const;
    int numPages() const;
    CursorMode cursorMode() const;
    QJsonDocument clipData() const;

    Q_INVOKABLE void setRect(int page1, QPointF p1, int page2, QPointF p2, bool asRect);
    void _selectText();
    void _selectRect();

    void _loadPDF();
    void _loadPages();
    //void _loadClips();
    QImage renderPage(int page) const;
    QImage renderRect(int page, QRectF rect) const;

    void setPageModel(PDFPageModel *model);

signals:
    void pageSizeChanged();
    void sourceChanged();
    void currentPageChanged();
    void zoomChanged();
    void cursorModeChanged();

public slots:
    void load(const QUrl& url);
    void setCurrentPage(int currPage);
    void setZoom(qreal zoomPage);
    void setCursorMode(CursorMode mode);
    void rectToggled(bool rect);
    void clipRect();
};

#endif // PDFVIEW_H
