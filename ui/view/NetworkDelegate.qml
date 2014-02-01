/*
  Copyright (C) 2013-2014 The Communi Project

  You may use this file under the terms of BSD license as follows:

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:
    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
    * Neither the name of the Communi Project nor the names of its
      contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

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

ListItem {
    id: root

    property IrcBuffer buffer
    property string reason: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)
    property variant storage: MessageStorage.get(buffer)

    property bool active: buffer && buffer.connection.active
    property bool connected: buffer && buffer.connection.connected
    property bool error: buffer && buffer.connection.status === IrcConnection.Error

    z: menuOpen ? 100 : 10
    contentHeight: Theme.itemSizeExtraSmall
    ListView.onRemove: animateRemoval(root)

    function remove(buffer, reason) {
        remorse.execute(root, qsTr("Removing"), function() { buffer.close(reason) }, 3000 )
    }

    RemorseItem {
        id: remorse
        z: 1000
    }

    menu: Component {
        ContextMenu {
            id: menu
            MenuItem {
                text: root.active ? qsTr("Disconnect") : qsTr("Connect")
                onClicked: {
                    if (root.active) {
                        buffer.connection.quit(reason)
                        buffer.connection.close()
                        buffer.connection.enabled = false
                    } else {
                        buffer.connection.enabled = true
                        buffer.connection.open()
                    }
                }
            }
            MenuItem {
                text: qsTr("Edit")
                onClicked: pageStack.push(editDialog, {connection: buffer.connection})
            }
            MenuItem {
                text: qsTr("Remove")
                onClicked: remove(root.buffer, root.reason)
            }
            property Rectangle background: Rectangle {
                id: background
                z: -1
                color: Theme.highlightDimmerColor
                Component.onCompleted: {
                    parent = menu
                    anchors.fill = menu
                }
            }
        }
    }

    Rectangle {
        z: -1
        rotation: 90
        width: parent.height
        height: parent.width
        anchors.centerIn: parent
        gradient: Gradient {
            GradientStop { position: 0.0; color: "transparent" }
            GradientStop { position: 0.1; color: Theme.highlightDimmerColor }
        }
    }

    SectionHeader {
        id: title
        text: buffer ? buffer.title : ""
        truncationMode: TruncationMode.Fade
        anchors { left: loader.right; right: parent.right; verticalCenter: parent.verticalCenter; margins: Theme.paddingLarge }
        color: (!storage || !root.connected) ? Theme.secondaryColor :
                buffer === MessageStorage.currentBuffer ? Theme.primaryColor :
               (storage.activeHighlights > 0 ? window.nickHighlight :
               (storage.badge > 0 ? Theme.highlightColor : Theme.primaryColor))
    }

    Loader {
        id: loader
        width: Theme.itemSizeExtraSmall - 2 * Theme.paddingMedium
        height: Theme.itemSizeExtraSmall - 2 * Theme.paddingMedium
        anchors { verticalCenter: parent.verticalCenter; left: parent.left; leftMargin: Theme.paddingLarge }

        sourceComponent: root.active && !root.connected ? busyIndicator : root.error ? errorImage : null

        Component {
            id: busyIndicator
            BusyIndicator { running: true }
        }

        Component {
            id: errorImage
            Image { source: "image://theme/icon-status-data-error" }
        }
    }

    Separator {
        color: Theme.highlightColor
        anchors { bottom: parent.bottom; left: parent.left; right: parent.right }
    }
}
