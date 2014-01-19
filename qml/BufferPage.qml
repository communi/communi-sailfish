/*
  Copyright (C) 2013-2014 Robin Burchell <robin+git@viroteck.net>
  Copyright (C) 2013-2014 J-P Nurmi <jpnurmi@gmail.com>

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Jolla Ltd nor the
      names of its contributors may be used to endorse or promote products
      derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
  ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
  WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR
  ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
  (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
  ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

import QtQuick 2.1
import Communi 3.1
import Sailfish.Silica 1.0
import "misc"

Page {
    id: page

    property IrcBuffer buffer
    property alias contentItem: column
    readonly property alias contentHeight: view.height
    readonly property var storage: MessageStorage.get(buffer)

    anchors.fill: parent

    Binding {
        target: storage
        property: "visible"
        value: Qt.application.active && (page.status === PageStatus.Active || page.status === PageStatus.Activating)
    }

    Column {
        id: column

        z: -2
        width: page.width
        height: page.height

        SilicaListView {
            id: view

            width: parent.width
            height: parent.height - entry.height

            PullDownMenu {
                id: pullDownMenu

                MenuItem {
                    text: qsTr("About IRC")
                    onClicked: pageStack.push(aboutDialogComponent)
                }
                MenuItem {
                    text: qsTr("Clear messages")
                    visible: storage && storage.count
                    onClicked: {
                        // TODO: why the view position jumps?
                        storage.clear()
                    }
                }
            }

            PushUpMenu {
                id: pushUpMenu
                transform: Translate { y: entry.height }

                MenuItem {
                    text: qsTr("Join a channel")
                    visible: BufferModel.connections.length > 0
                    onClicked: pageStack.push(joinDialog, {index: BufferModel.models.indexOf(buffer.model)})
                }
                MenuItem {
                    text: qsTr("Open a query")
                    visible: BufferModel.models.length > 0
                    onClicked: pageStack.push(queryDialog, {index: BufferModel.models.indexOf(buffer.model)})
                }
                MenuItem {
                    text: qsTr("Connect a network")
                    onClicked: pageStack.push(connectDialog)
                }
            }

            header: PageHeader { title: buffer ? buffer.title : "" }

            model: storage

            delegate: ListItem {
                contentHeight: label.height
                Label {
                    id: label
                    text: richtext
                    anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge }
                    wrapMode: Text.Wrap
                    textFormat: Text.RichText
                    linkColor: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    onLinkActivated: Qt.openUrlExternally(link)
                    color: highlight ? window.nickHighlight : event ? Theme.secondaryColor : Theme.primaryColor
                    font.bold: !seen
                }
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr("Copy")
                            onClicked: Clipboard.text = plaintext
                        }
                    }
                }
            }

            Component.onCompleted: {
                view.positionViewAtEnd();
            }
            onCountChanged: {
                if (view.visibleArea.yPosition + view.visibleArea.heightRatio > 0.9) {
                    positioner.restart();
                }
            }

            // <workaround>
            // the RIGHT WAY would be to listen to Qt.inputMethod.animatingChanged instead
            // details: https://together.jolla.com/question/8611/bug-qinputmethodanimatingchanged-is-never-emitted/
            onHeightChanged: {
                positioner.restart();
            }
            // </workaround>

            VerticalScrollDecorator { }

            Timer {
                id: idle
                property bool moving: view.moving || view.dragging || view.flicking
                property bool menuOpen: pullDownMenu.active || pushUpMenu.active
                onMovingChanged: if (!moving && !menuOpen) restart()
                interval: view.atYBeginning || view.atYEnd ? 300 : 2000
            }

            IconButton {
                icon.source: "images/arrow-up.png"
                opacity: (idle.moving || idle.running) && !idle.menuOpen && !positioner.running ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation { duration: 400 } }
                onClicked: {
                    view.cancelFlick();
                    view.positionViewAtBeginning();
                }
                anchors { top: parent.top; right: parent.right; margins: Theme.paddingLarge }
            }

            IconButton {
                icon.source: "images/arrow-down.png"
                opacity: (idle.moving || idle.running) && !idle.menuOpen && !positioner.running ? 1.0 : 0.0
                Behavior on opacity { FadeAnimation { duration: 400 } }
                onClicked: {
                    view.cancelFlick();
                    view.positionViewAtEnd();
                }
                anchors { bottom: parent.bottom; right: parent.right; margins: Theme.paddingLarge }
            }
        }

        TextEntry {
            id: entry
            buffer: page.buffer
            width: parent.width

            backgroundVisible: !positioner.running && !pushUpMenu.active && !view.atYEnd
            opacity: pullDownMenu.active || pushUpMenu.active ? 0.0 : 1.0
            Behavior on opacity { FadeAnimation { } }
        }
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: {
            if (!pullDownMenu.active && !pushUpMenu.active) {
                view.cancelFlick();
                view.positionViewAtEnd();
            }
        }
    }
}
