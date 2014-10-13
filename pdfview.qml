import QtQuick 2.0
import MyTypes 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2
import QtQuick.Layouts 1.0

// TODO Center listView on mouse position upon zoom

PDFView {
    id: pdfView

    property var pages: new Object()
    function showHandles(show)
    {
        show = typeof show !== 'undefined' ? show : true;
        console.log("showHandles ("+show+") "+firstHandlePage+", "+secondHandlePage)
        var item;
        var firstPage = pages[firstHandlePage]
        var secondPage = pages[secondHandlePage]
        if (!show || !firstPage) {
            handleBegin.visible = false
        } else {
            handleBegin.parent = firstPage
            handleBegin.visible = true
        }
        handleBegin.page = firstHandlePage
        handleBegin.pos = firstHandlePoint
        handleBegin.setXY()
        if (!show || !secondPage) {
            handleEnd.visible = false
        } else {
            handleEnd.parent = secondPage
            handleEnd.visible = true
        }
        handleEnd.pos = secondHandlePoint
        handleEnd.page = secondHandlePage
        handleEnd.setXY()
    }

    Component {
        id: pageDelegate

        Rectangle {
            Component.onCompleted: {
                pdfView.pages[pageNumber] = this
                // TODO Broadcast item events
                console.log(pageNumber, pageSize, selection)
            }
            Component.onDestruction: {
                pdfView.pages[pageNumber] = null
            }
            anchors.horizontalCenter: parent.horizontalCenter
            property int index: pageNumber
            id: page
            width: pageSize.width * pdfView.pageScale
            height: pageSize.height * pdfView.pageScale
            color: "transparent";
            Image {
                id: pageImage
                fillMode: Image.PreserveAspectFit
                asynchronous: true
                source:  "image://pdfImage/" + pageNumber
                sourceSize.width:  pdfView.fitWidth ? listView.width : pageSize.width* pdfView.pageScale
                z: 1;
                Repeater {
                    model: selection
                    Rectangle {
                        x: model.modelData.x * pdfView.pageScale
                        y: model.modelData.y * pdfView.pageScale
                        width: model.modelData.width * pdfView.pageScale
                        height: model.modelData.height * pdfView.pageScale
                        color: "#403333cc"
                    }
                }
                Rectangle {
                    x: selectionRect.x * pdfView.pageScale
                    y: selectionRect.y * pdfView.pageScale
                    width: selectionRect.width * pdfView.pageScale
                    height: selectionRect.height * pdfView.pageScale
                    color: "transparent"
                    border.width: 1
                }
            }
        }
    }
    property bool fitWidth: false
    property real pageScale: zoom * dpi.x / 72
    zoom: pdfView.fitWidth ? listView.width / pageSize.width : zoom
    ScrollView {
        anchors.fill: parent
        MouseArea {
            property var p;
            anchors.fill: parent
            onPressed: {
                p = listView.pageCoordRel(mouseX, mouseY);
                selRect.x = mouseX;
                selRect.y = mouseY;
                pdfView.showHandles(false);
            }
            onReleased: {
                selRect.width = 0;
                selRect.height = 0;
                var m = listView.pageCoordRel(mouseX, mouseY);
                console.log("ON_RELEASED", m, p)
                var isRect = pdfView.cursorMode === PDFView.Rectangle;
                pdfView.setRect(p[0], p[1], m[0], m[1], isRect);
                pdfView.showHandles(!isRect);
                if (isRect && p[1] !== m[1])
                    pdfContext.popup()
            }
            onPositionChanged: {

                if (pressed) {
                    var m = listView.pageCoordRel(mouseX, mouseY);
                    selRect.width= (mouseX-selRect.x);
                    selRect.height= (mouseY-selRect.y);
                    console.log(p, m)
                    pdfView.setRect(p[0], p[1], m[0], m[1], pdfView.cursorMode === PDFView.Rectangle);
                }
            }
            Rectangle {
                id: selRect;
                color: "#103333cc";
            }
        }
        ListView {
            id: listView
            anchors.fill: parent
            model: pdfModel
            delegate: pageDelegate
            currentIndex: pdfView.currentPage
            focus: true
            //cacheBuffer: pdfView.pageSize.height
            spacing: 2
            maximumFlickVelocity: 1000
            function pageCoordRel(mx, my) {
                return pageCoord(mx+contentX, my+contentY);
            }

            function pageCoord(mx, my) {
                var item = itemAt(mx, my);
                var page = indexAt(mx, my);
                return [page, Qt.point( (mx-item.x) / pdfView.pageScale, (my-item.y) / pdfView.pageScale)];
            }

            MouseArea {
                anchors.fill: parent
                onWheel: {
                    if (wheel.modifiers & Qt.ControlModifier) {
                        wheel.accepted = true
                        pdfView.zoom += wheel.angleDelta.y * 0.0005 ;
                        console.log(zoom)
                    } else
                        wheel.accepted = false
                }
                onPressed: mouse.accepted = false;
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

    TextHandle {
        id: handleBegin
        pageScale: pdfView.pageScale
        parent : listView.contentItem;
        onPressed: {
            // Detach from page
            x = parent.x + x
            y = parent.y + y
            parent = listView.contentItem;
        }
        onPositionChanged: {
            var p = listView.pageCoord(x, y);
            pdfView.setRect(p[0], p[1], handleEnd.page, handleEnd.pos)
        }

        onReleased: {
            // Attach to page
            pdfView.showHandles()
        }
    }
    TextHandle {
        id: handleEnd
        pageScale: pdfView.pageScale
        parent : listView.contentItem;
        onPressed: {
            // Detach from page
            x = parent.x + x
            y = parent.y + y
            parent = listView.contentItem;
        }
        onPositionChanged: {
            var p = listView.pageCoord(x, y);
            pdfView.setRect(handleBegin.page, handleBegin.pos, p[0], p[1])
        }

        onReleased: {
            // Attach to page
            pdfView.showHandles()
        }
    }

}
