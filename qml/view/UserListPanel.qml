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
    * Neither the name of the copyright holder nor the names of its
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
import Communi 3.2
import Sailfish.Silica 1.0
import SortedUserModel 1.0
import StringFilterModel 1.0

Panel {
    id: panel

    property IrcBuffer buffer

    signal clicked(IrcUser user)
    signal queried(IrcUser user)

    SortedUserModel {
        id: userModel
        channel: panel.buffer ? panel.buffer.toChannel() : null
        // Error: Cannot assign int to IRC_PREPEND_NAMESPACE(Irc::SortMethod
        // sortMethod: Irc.SortByTitle
    }

    SilicaListView {
        pressDelay: 0
        anchors.fill: parent
        anchors.bottomMargin: toolbar.height - 2

        model: StringFilterModel {
            filter: toolbar.filter
            sourceModel: userModel
        }

        delegate: ListItem {
            contentHeight: Theme.itemSizeSmall
            Label {
                text: title
                truncationMode: TruncationMode.Fade
                anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge; verticalCenter: parent.verticalCenter }
            }
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr("Query")
                        // #86 Querying certain users from right-pulley will cause unhandled page fault
                        onClicked: queryTimer.start()
                        Timer {
                            id: queryTimer
                            interval: 200
                            onTriggered: panel.queried(model.user)
                        }
                    }
                    MenuItem {
                        readonly property bool isOp: model.prefix.indexOf("@") !== -1
                        text: isOp ? qsTr("Deop") : qsTr("Op")
                        onClicked: buffer.sendCommand(ircCommand.createMode(buffer.title, isOp ? "-o" : "+o", model.name))
                    }
                    MenuItem {
                        readonly property bool isVoice: model.prefix.indexOf("+") !== -1
                        text: isVoice ? qsTr("Devoice") : qsTr("Voice")
                        onClicked: buffer.sendCommand(ircCommand.createMode(buffer.title, isVoice ? "-v" : "+v", model.name))
                    }
                    MenuItem {
                        text: qsTr("Kick")
                        onClicked: buffer.sendCommand(ircCommand.createKick(buffer.title, model.name))
                    }
                    MenuItem {
                        text: qsTr("Ban")
                        onClicked: buffer.sendCommand(ircCommand.createMode(buffer.title, "+b", model.name + "!*@*"))
                    }
                }
            }

            onClicked: panel.clicked(model.user)
        }

        VerticalScrollDecorator { }
    }

    onBufferChanged: toolbar.clear()

    SearchToolBar {
        id: toolbar
        width: parent.width
        anchors.bottom: parent.bottom
        icon: "../images/filter.png"
    }
}
