import QtQuick 2.0

Item {
    // FIXME Reparent after page destruction-construction.
    id: dragHandle
    //property var page;
    property point pos;
    property real pageScale;
    property int page;
    onPageScaleChanged: setXY();
    function setXY() {
        x = pos.x * pageScale;
        y = pos.y * pageScale;
        console.log("setXY", x, y)
    }

    z: 10
    property alias color: rect.color
    signal pressed
    signal positionChanged
    signal released
    visible: true
    Rectangle {
        id: rect
        anchors.horizontalCenter: parent.horizontalCenter
        width: 8
        height: 20
        color: "blue"
        border.width: 1

        MouseArea {
            id: dragArea
            anchors.fill: parent
            drag.target: dragHandle
            onPressed: dragHandle.pressed()
            onPositionChanged: dragHandle.positionChanged()
            onReleased: dragHandle.released()
        }
    }
}
