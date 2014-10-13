#include <QtWidgets>
#include <QQmlContext>
#include "mainwindow.h"
#include "pdfpagemodel.h"
#include "pdfimageprovider.h"

MainWindow::MainWindow(const QUrl& url)
{
    _splitter = new QSplitter(this);
    _locationEdit = new QLineEdit(this);
    _locationEdit->setSizePolicy(QSizePolicy::Expanding, _locationEdit->sizePolicy().verticalPolicy());
    connect(_locationEdit, SIGNAL(returnPressed()), SLOT(changeURL()));

    QToolBar *toolBar = addToolBar(tr("Navigation"));
    toolBar->addWidget(_locationEdit);

    PDFPageModel *pdfPageModel = new PDFPageModel;
    //pages.push_back(new PDFPage(nullptr, 0));
    //pages.push_back(new PDFPage(nullptr, 1));
    //pages.push_back(new PDFPage(nullptr, 2));

    _pdfQuickView = new QQuickView;
    // Image provider instantiated before QML
    PDFImageProvider *pdfImageProvider =  new PDFImageProvider;
    _pdfQuickView->setColor(palette().color(QPalette::Normal, QPalette::Window));
    _pdfQuickView->engine()->addImageProvider(QLatin1String("pdfImage"), pdfImageProvider);
    _pdfQuickView->engine()->rootContext()->setContextProperty("pdfModel", pdfPageModel);
    _pdfQuickView->setResizeMode(QQuickView::SizeRootObjectToView);
    _pdfQuickView->setSource(QUrl("qrc:/PDF.qml"));
    //_pdfQuickView->setSource(QUrl("qrc:/pdfview.qml"));
    //_pdfQuickView->setSource(QUrl("qrc:/test.qml"));
    qDebug() << _pdfQuickView->errors();

    // Provide images for PDFView
    QObject *object = _pdfQuickView->rootObject();
    _pdfView = qobject_cast<PDFView*>(object);
    _pdfView->setPageModel(pdfPageModel);
    pdfImageProvider->setPDFView(_pdfView);

    // Add widgets to window
    QWidget *pdfContainer = QWidget::createWindowContainer(_pdfQuickView);
    setCentralWidget(_splitter);
    _splitter->addWidget(pdfContainer);
    //_splitter->addWidget(new QTextEdit);
    //setUnifiedTitleAndToolBarOnMac(true);
}


void MainWindow::_displayURL(QUrl const &url)
{
    _locationEdit->setText(url.toString());
}

void MainWindow::changeURL()
{
    QUrl url = QUrl::fromUserInput(_locationEdit->text());
    qDebug() << "MainWindow::changeURL:" << url.toString();
    _displayURL(url);
    _loadURL(url);
}

void MainWindow::_loadURL(QUrl const &url)
{
    _pdfView->load(url);
    qDebug() << _pdfQuickView->errors();
    //_pdfView->setFocus();
}




