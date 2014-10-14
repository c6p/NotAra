#include <QtWidgets>
#include <QQuickView>
#include "pdfview.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(int placeholder);

public slots:
    void changeURL();

private:
    QSplitter *_splitter;
    PDFView *_pdfView;
    QQuickView *_pdfQuickView;
    QLineEdit *_locationEdit;
    QAction *_actionRect;
    QAction *_actionHi;

    void _displayURL(QUrl const &url);
    void _loadURL(QUrl const &url);

};
