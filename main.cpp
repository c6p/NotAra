#include <QtWidgets>
#include "mainwindow.h"

int main(int argc, char * argv[])
{
    QApplication app(argc, argv);

    qmlRegisterType<PDFView>("MyTypes", 1, 0, "PDFView");
    //qmlRegisterType<PDFView>("MyTypes", 1, 0, "PDFPage");
    //qmlRegisterType<PDFView>("MyTypes", 1, 0, "QRectF");

    QCommandLineParser commandLineParser;
    commandLineParser.addPositionalArgument(QStringLiteral("url"),
        QStringLiteral("The url to be loaded in the browser window."));
    commandLineParser.process(app);
    QStringList positionalArguments = commandLineParser.positionalArguments();

    QUrl url;
    if (positionalArguments.size() > 1) {
        return -1;
    } else if (positionalArguments.size() == 1)
        url = QUrl::fromUserInput(positionalArguments.at(0));
    else
        url = QUrl("");

    //if (!url.isValid()) {
    //    return -1;
    //}

    MainWindow browser(url);
    browser.show();
    return app.exec();
}
