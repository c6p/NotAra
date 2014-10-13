import QtQuick 2.0
import MyTypes 1.0

ListView {
    id: listView
    property real pageScale: 1;

    focus: true
    spacing: 2

    function pageCoordRel(mx, my) {
        return pageCoord(mx+contentX, my+contentY);
    }

    function pageCoord(mx, my) {
        var mid = width/2;
        var page = indexAt(mid, my);
        var item = itemAt(mx, my);
        if (!item) {
            item = itemAt(mid, my);
            if (mx > mid)
                mx = item.x + item.width;
            else
                mx = item.x;
        }
        return [page, Qt.point( (mx-item.x)/pageScale , (my-item.y)/pageScale )];
    }

    MouseArea {
        anchors.fill: parent
        onWheel: {
            if (wheel.modifiers & Qt.ControlModifier) {
                wheel.accepted = true
                listView.pageScale += wheel.angleDelta.y * 0.0005 ;
                console.log(listView.scale);
                //console.log(zoom)
            } else
                wheel.accepted = false
        }
        onPressed: mouse.accepted = false;
    }
}
