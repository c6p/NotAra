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
                selRect.x = mouseX;
                selRect.y = mouseY;
                console.log(mouseX, mouseY);
                p = pdfDoc.pageCoordRel(mouseX, mouseY);
                //pdfView.showHandles(false);
            }

            onReleased: {
                selRect.width = 0;
                selRect.height = 0;
                var m = pdfDoc.pageCoordRel(mouseX, mouseY);
                var isRect = pdfView.cursorMode === PDFView.Rectangle;
                    pdfView.setRect(p[0], p[1], m[0], m[1], isRect);
                    //pdfView.showHandles(!isRect);
                    if (isRect && p[1] !== m[1])
                        pdfContext.popup()
            }

            onPositionChanged: {
                if (pressed) {
                    selRect.width= (mouseX-selRect.x);
                    selRect.height= (mouseY-selRect.y);
                    var m = pdfDoc.pageCoordRel(mouseX, mouseY);
                        pdfView.setRect(p[0], p[1], m[0], m[1], pdfView.cursorMode === PDFView.Rectangle);
                }
            }

            Rectangle {
                id: selRect;
                color: "#103333cc";
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

            ToolBar {
                RowLayout {
                    ToolButton {
                        iconSource: "images/cursor.svg"
                        onClicked: pdfView.cursorMode = PDFView.Cursor
                    }
                    ToolButton {
                        iconSource: "images/rectangle_stroked.svg"
                        onClicked: pdfView.cursorMode = PDFView.Rectangle
                    }
                }
            }

            Menu {
                id: pdfContext
                title: "Edit"

                MenuItem {
                    text: "Select"
                }
                MenuItem {
                    text: "Crop"
                    iconSource: "images/crop.svg"
                }
            }
        }
    }

}
