#ifndef PDFVIEW_H
#define PDFVIEW_H

#include <QQuickItem>
#include <QQuickImageProvider>
#include <poppler-qt5.h>

class PDFModel;

class PDFView : public QQuickItem
{
    Q_OBJECT
    Q_PROPERTY(QSize pageSize READ pageSize NOTIFY pageSizeChanged)
    //Q_PROPERTY(QUrl source READ source WRITE load NOTIFY sourceChanged)
    Q_PROPERTY(int currentPage READ currentPage WRITE setCurrentPage NOTIFY currentPageChanged)
    Q_PROPERTY(qreal zoom READ zoom WRITE setZoom NOTIFY zoomChanged)
    Q_PROPERTY(QPointF dpi READ dpi CONSTANT)
    Q_PROPERTY(int numPages READ numPages)

public:
    explicit PDFView(QQuickItem *parent = 0);

    Poppler::Document* docPDF;
    QUrl _source;
    int _currentPage;
    qreal _zoom;
    const QPoint _dpi;
    int _numPages;
    PDFModel *_model;
    QSize _pageSize;

    QSize pageSize() const;
    QUrl source() const;
    int currentPage() const;
    qreal zoom() const;
    const QPoint dpi() const;
    int numPages() const;

    std::pair<int, int> _selectPage;
    Q_INVOKABLE void selectText(int page1, QPointF p1, int page2, QPointF p2);

    void _loadPDF();
    void _loadPages();
    QImage renderPage(int page) const;

    void setModel(PDFModel *model);

signals:
    void pageSizeChanged();
    void sourceChanged();
    void currentPageChanged();
    void zoomChanged();

public slots:
    void load(const QUrl& url);
    void setCurrentPage(int currPage);
    void setZoom(qreal zoomPage);
};

#endif // PDFVIEW_H
