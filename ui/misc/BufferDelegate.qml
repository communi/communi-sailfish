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

ListItem {
    id: root

    property IrcBuffer buffer
    property string reason: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)
    property variant storage: MessageStorage.get(buffer)

    visible: buffer && !buffer.sticky
    contentHeight: buffer && !buffer.sticky ? Theme.itemSizeSmall : 0
    ListView.onRemove: animateRemoval(root)

    menu: Component {
        ContextMenu {
            id: menu
            MenuItem {
                visible: buffer && buffer.channel && buffer.connection.connected
                text: buffer && buffer.active ? qsTr("Part") : qsTr("Join")
                onClicked: buffer.active ? buffer.part(reason) : buffer.join()
            }
            MenuItem {
                text: qsTr("Remove")
                onClicked: buffer.close(reason)
            }
        }
    }

    Label {
        id: title
        truncationMode: TruncationMode.Fade
        text: buffer ? buffer.title : text
        verticalAlignment: Qt.AlignVCenter
        anchors { fill: parent; leftMargin: Theme.paddingLarge; rightMargin: glass.opacity > 0 ? glass.width : 0 }
        // inactive buffer > highlighted buffer > unread messages buffer > nothing special buffer
        color: (!buffer || !buffer.active) ? Theme.secondaryColor : (storage.activeHighlights > 0 ? window.nickHighlight : (storage.badge > 0 ? Theme.highlightColor : Theme.primaryColor))
    }

    GlassItem {
        id: glass
        opacity: buffer === MessageStorage.currentBuffer ? 1.0 : 0.0
        Behavior on opacity { FadeAnimation { } }
        color: Theme.primaryColor
        falloffRadius: 0.16
        radius: 0.15
        anchors { right: parent.right; verticalCenter: parent.verticalCenter }
    }
}
