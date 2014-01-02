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

Page {
    id: page

    property IrcBuffer buffer

    Binding {
        target: MessageStorage.get(buffer)
        property: "active"
        value: page.status === PageStatus.Active
    }

    SilicaListView {
        id: view
        clip: true
        anchors { top: parent.top; left: parent.left; right: parent.right; bottom: entry.top }

        PullDownMenu {
            id: bufferPagePullDownMenu
            MenuItem {
                text: qsTr("Clear")
                enabled: MessageStorage.get(buffer).count
                onClicked: {
                    // TODO: why the view position jumps?
                    MessageStorage.get(buffer).clear()
                }
            }
        }

        header: PageHeader { title: buffer.title }

        model: MessageStorage.get(buffer)

        delegate: Label {
            text: display
            anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge }
            wrapMode: Text.Wrap
            linkColor: Theme.highlightColor
            font.pixelSize: Theme.fontSizeSmall
            onLinkActivated: Qt.openUrlExternally(link)
            color: highlight ? "#ff4d4d" : seen ? Theme.secondaryColor : Theme.primaryColor
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
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: {
            view.cancelFlick();
            view.positionViewAtEnd();
        }
        triggeredOnStart: false
        running: false
        repeat: false
    }

    TextEntry {
        id: entry
        buffer: page.buffer
        anchors {
            left: parent.left
            right: parent.right
            bottom: parent.bottom
        }
    }
}
