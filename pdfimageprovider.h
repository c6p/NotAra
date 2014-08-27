#ifndef PDFIMAGEPROVIDER_H
#define PDFIMAGEPROVIDER_H

#include <QQuickImageProvider>

class PDFView;

class PDFImageProvider : public QQuickImageProvider
{
public:
    explicit PDFImageProvider();

    PDFView *_pdfView;

    void setPDFView(PDFView *pdfView);
    QImage requestImage(const QString &id, QSize *size, const QSize &requestedSize);

};

#endif // PDFIMAGEPROVIDER_H
