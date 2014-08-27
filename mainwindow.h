#include <QtWidgets>
#include <QQuickView>
#include "pdfview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QUrl const& url);

public slots:
    void changeURL();

private:
    QSplitter *_splitter;
    PDFView *_pdfView;
    QQuickView *_pdfQuickView;
    QLineEdit *_locationEdit;

    void _displayURL(QUrl const &url);
    void _loadURL(QUrl const &url);

};
