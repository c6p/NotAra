import QtQuick 2.0

Item {
    id: base
    property alias txt_visible: txt.visible
    property alias img_visible: img.visible

    state: type == "text" ? "txt" : "img"

    Text {
        id: txt
    }

    Image {
        id: img
    }

    states: [
        State {
            name: "txt"
            PropertyChanges {
                target: base
                txt_visible: true
                img_visible: false
            }
        },
        State {
            name: "img"
            PropertyChanges {
                target: base
                txt_visible: false
                img_visible: true
            }
        }
    ]
}

