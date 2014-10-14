import QtQuick 2.0
import QtQuick.Controls 1.2
import MyTypes 1.0
import QtGraphicalEffects 1.0
import QtQuick.Layouts 1.0

PDFView {
    id: pdfView

    function attachPage(page) {

    }

    function detachPage(page) {

    }

    ScrollView {
        anchors.fill: parent

        MouseArea {
            property var p;
            anchors.fill: parent

            onPressed: {
                p = pdfDoc.pageCoordRel(mouseX, mouseY);
                //pdfView.showHandles(false);
            }

            onReleased: {
                var isRect = pdfView.cursorMode === PDFView.Rectangle;
                var m = pdfDoc.pageCoordRel(mouseX, mouseY);
                pdfView.setRect(p[0], p[1], m[0], m[1], isRect);
                    //pdfView.showHandles(!isRect);
            }

            onPositionChanged: {
                if (pressed) {
                    var isRect = pdfView.cursorMode === PDFView.Rectangle;
                    var m = pdfDoc.pageCoordRel(mouseX, mouseY);
                    pdfView.setRect(p[0], p[1], m[0], m[1], isRect);
                }
            }
        }

        PDFDoc {
            id: pdfDoc;
            anchors.fill: parent;
            currentIndex: pdfView.currentPage
            model: pdfModel
            delegate: PDFPage {
                pdfView: pdfView;
                scale: pdfDoc.pageScale;
            }

        }
    }

}
