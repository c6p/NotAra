#include "pdfimageprovider.h"
#include "pdfview.h"

PDFImageProvider::PDFImageProvider() :
    QQuickImageProvider(QQuickImageProvider::Image),
    _pdfView(nullptr)
{}

void PDFImageProvider::setPDFView(PDFView *pdfView)
{
    _pdfView = pdfView;
}

QImage PDFImageProvider::requestImage(const QString &id,
        QSize *size, const QSize &requestedSize)
{
    Q_CHECK_PTR(_pdfView);
    QImage page = _pdfView->renderPage(id.toInt());
    *size = page.size();
    qDebug() << "PDFImageProvider::requestImage page" << id
        << "SIZE:" << *size << "px";
    return page;
}
