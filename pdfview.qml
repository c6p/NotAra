import QtQuick 2.0
import MyTypes 1.0
import QtGraphicalEffects 1.0
import QtQuick.Controls 1.2

PDFView {
    id: pdfView

    Component {
        id: pageDelegate

        Rectangle {
            Component.onCompleted: console.log(pageNumber, pageSize, selection)
            id: page
          //          Text { text : pageNumber + pageSize }
            width: parent.width
            height: pageSize.height * pdfView.scale
            //      width: pageWidth;
      //      height: pageHeight;
           // height: pageImage.height
             color: "transparent";
             Image {
                 //width: parent.width
                 //height: parent.height
                 //width: pageSize.width// * pdfView.dpi.x * pdfView.zoom
                 //height: pageSize.height// * pdfView.dpi.y * pdfView.zoom
                 id: pageImage
                 fillMode: Image.PreserveAspectFit
                 asynchronous: true
                 source:  "image://pdfImage/" + pageNumber
                 //source: ""
                 sourceSize.width:  pdfView.fitWidth ? listView.width : pageSize.width* pdfView.scale
                 //onWidthChanged: { if (source==="") source = tmpSource; console.log(source); }
                 //property string tmpSource: "image://pdfImage/swap" + pageNumber
                 //onStatusChanged: if (status == Image.Ready) { var tmp=pageImage.source; pageImage.source=tmpSource; tmpSource=tmp; source=""; }
                 //onStatusChanged: if (status == Image.Ready) { var tmp=pageImage.source; pageImage.source=source; source=tmp; }
//                 onStatusChanged: if (status == Image.Ready) {
//                         var tmp;
//                         tmp=width; width=sourceSize.width; sourceSize.width=tmp;
//                         //swap(source, tmpSource);
//                         tmp=z; z=pageImage.z; pageImage.z=tmp;
//                         tmp=fillMode; fillMode=pageImage.fillMode; pageImage.fillMode=tmp;
//                         //swap(onWidthChanged, pageImage.onWidthChanged);
//                          //                       console.log("pageImage SWAP "+z);
//                 }
                 //function swap(a, b) { var tmp=a; a=b; b=tmp; }
                 z: 1;
                 Repeater {
                     model: selection
                     Rectangle {
                         x: model.modelData.x * pdfView.scale
                         y: model.modelData.y * pdfView.scale
                         width: model.modelData.width * pdfView.scale
                         height: model.modelData.height * pdfView.scale
                         color: "#403333cc"
                     }
                 }
             }
//            Image {
//                width: parent.width
//                //height: parent.height
//                //width: pageSize.width// * pdfView.dpi.x * pdfView.zoom
//                //height: pageSize.height// * pdfView.dpi.y * pdfView.zoom
//                id: pageImage
//                fillMode: Image.PreserveAspectCrop
//                source: "image://pdfImage/" + pageNumber
//                //property string tmpSource: ""
//                asynchronous: true
//                //property string tmpSource: ""
//                //sourceSize.width: parent.width;
//                //function swap() { var tmp=source; source=tmpSource; tmpSource=tmp; }
//                //onWidthChanged: { console.log(source); reload(); }
//                //onWidthChanged: { if (source==="") source = tmpSource; console.log(source); }
//                onStatusChanged: if (status == Image.Ready) {
//                        var tmp;
//                        tmp=width; width=sourceSize.width; sourceSize.width=tmp;
//                        //swap(source, tmpSource);
//                        tmp=z; z=pageSwapImage.z; pageSwapImage.z=tmp;
//                        tmp=fillMode; fillMode=pageSwapImage.fillMode; pageSwapImage.fillMode=tmp;
//                        //swap(onWidthChanged, pageImage.onWidthChanged);
//                         //                       console.log("pageImage SWAP "+z);
//                }
//               // function swap(a, b) { var tmp=a; a=b; b=tmp; }
//                z: 2;
//            }
        }
    }
    property bool fitWidth: false
    property var scale: zoom * dpi.x / 72
    //property var pageZoom: width / pageSize.width
    //zoom: pageZoom// * (1+Math.log(Math.abs(listView.verticalVelocity)+1))
    zoom: pdfView.fitWidth ? listView.width / pageSize.width : zoom
ScrollView {
            anchors.fill: parent
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
        function mouseDelegateCoord(mx, my) {
            var mouse = Qt.point(mx+contentX, my+contentY);
            var item = itemAt(mouse.x, mouse.y);
            var page = indexAt(mouse.x, mouse.y);
            console.log(mouse.x, mouse.y, indexAt(mouse.x, mouse.y), item);
            return [page, Qt.point( (mouse.x-item.x) / pdfView.scale, (mouse.y-item.y) / pdfView.scale)]; }

        MouseArea {
//preventStealing: true
            property var p;
             property point start : Qt.point(0,0)
anchors.fill: parent
onPressed: {             p = listView.mouseDelegateCoord(mouseX, mouseY);
    start.x = mouseX;
    start.y = mouseY; console.log(m) }
// onEntered:  { start.x = mouseX; start.y = mouseY; console.log(pageNumber, mouseX, mouseY) }
//        onExited: {
//            selectionRect.width= Math.min(width,mouseX)-start.x;
//            selectionRect.height= Math.max(height,mouseY)-start.y;}
onReleased: {
selectionRect.width = 0;
    selectionRect.height = 0;
//                             var newRect = Qt.createQmlObject(
//'import QtQuick 2.0; Rectangle {color: "#333333cc";x:' + start.x + ';y:' + start.y +';width:' + (mouseX-start.x) + ';height:' + (mouseY-start.y) + '}',
//                                         parent, "dynamicSnippet1");
    var m = listView.mouseDelegateCoord(mouseX, mouseY);
    console.log(m) }
Rectangle {
    id: selectionRect;
    color: "#103333cc";
}


onWheel: {
    if (wheel.modifiers & Qt.ControlModifier) {
                    pdfView.zoom += wheel.angleDelta.y * 0.0005 ;
        console.log(zoom)
    wheel.accepted = true
            }
    wheel.accepted = false
        }


onPositionChanged: {

    if (pressed) {
        selectionRect.x = start.x;
        selectionRect.y = start.y;
    var m = listView.mouseDelegateCoord(mouseX, mouseY);
        selectionRect.width= (mouseX-start.x);
        selectionRect.height= (mouseY-start.y);
        console.log(p, m)
        pdfView.selectText(p[0], p[1], m[0], m[1]);
    } }
}
    }
}




         //MouseArea {
         //    anchors.fill: parent
         //    onClicked: console.log(pdfModel, listView.count)
         //}

}
