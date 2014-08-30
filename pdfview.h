#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QQuickItem>
#include <QQuickImageProvider>
#include <poppler-qt5.h>

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

    Q_PROPERTY(int firstHandlePage READ firstHandlePage)
    Q_PROPERTY(int secondHandlePage READ secondHandlePage)
    Q_PROPERTY(QPointF firstHandlePoint READ firstHandlePoint)
    Q_PROPERTY(QPointF secondHandlePoint READ secondHandlePoint)
public:
    explicit PDFView(QQuickItem *parent = 0);

    enum CursorMode {
        Cursor,
        Rectangle
    };

    Poppler::Document* docPDF;
    QUrl _source;
    int _currentPage;
    qreal _zoom;
    const QPoint _dpi;
    int _numPages;
    PDFPageModel *_pageModel;
    QSize _pageSize;
    CursorMode _cursorMode;

    std::pair<int, int> _handlePages, _rectPages;
    std::pair<QPointF, QPointF> _handlePoints, _rectPoints;

    QSize pageSize() const;
    QUrl source() const;
    int currentPage() const;
    qreal zoom() const;
    const QPoint dpi() const;
    int numPages() const;
    CursorMode cursorMode() const;

    int firstHandlePage() const;
    int secondHandlePage() const;
    QPointF firstHandlePoint() const;
    QPointF secondHandlePoint() const;

    Q_INVOKABLE void setRect(int page1, QPointF p1, int page2, QPointF p2);
    void _selectText();

    void _loadPDF();
    void _loadPages();
    QImage renderPage(int page) const;

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
};

#endif // PDFVIEW_H
