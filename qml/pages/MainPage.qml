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
import org.nemomobile.ngf 1.0
import org.nemomobile.notifications 1.0
import "../dialogs"

Page {
    id: page

    SilicaListView {
        id: view
        anchors.fill: parent

        PullDownMenu {
            MenuItem {
                text: qsTr("About Communi")
                onClicked: pageStack.push(Qt.resolvedUrl("../dialogs/AboutDialog.qml"))
            }
            MenuItem {
                text: qsTr("Connect a network")
                onClicked: pageStack.push(connectDialog)
            }
            MenuItem {
                text: qsTr("Open a query")
                visible: BufferModel.models.length > 0
                onClicked: pageStack.push(queryDialog, {model: BufferModel.models})
            }
            MenuItem {
                text: qsTr("Join a channel")
                visible: BufferModel.connections.length > 0
                onClicked: pageStack.push(joinDialog, {model: BufferModel.connections})
            }
        }

        ViewPlaceholder {
            enabled: view.count === 0
            text: qsTr("Connect to a network")
        }

        header: PageHeader { title: "Communi" }

        model: BufferModel

        delegate: BufferDelegate {
            buffer: model.buffer
            onClicked: {
                pageStack.push(bufferPage, {buffer: model.buffer})
                if (model.buffer.channel)
                    pageStack.pushAttached(usersPage, {channel: model.buffer.toChannel()})
            }
        }
        VerticalScrollDecorator { }
    }

    NonGraphicalFeedback {
        id: activeEffect
        event: "chat_fg"
    }

    Notification {
        id: backgroundNotification
        category: "x-nemo.messaging.im"
        property IrcBuffer buffer

        onClicked: {
            if (pageStack.depth > 1)
                pageStack.replace(bufferPage, {buffer: backgroundNotification.buffer})
            else
                pageStack.push(bufferPage, {buffer: backgroundNotification.buffer})
            window.activate()
        }
    }

    Connections {
        target: MessageStorage
        onHighlighted: {
            if (Qt.application.active) {
                activeEffect.play();
            } else {
                // Send notification
                backgroundNotification.summary = qsTr("New message")
                backgroundNotification.body = qsTr("%1: %2").arg(message.nick).arg(message.content)
                backgroundNotification.previewSummary = qsTr("New message")
                backgroundNotification.previewBody = qsTr("%1: %2").arg(message.nick).arg(message.content)
                backgroundNotification.buffer = buffer
                backgroundNotification.publish()

                // Increase highlight count on cover
                appCover.unreadHighlights += 1;
            }
        }
    }

    Component {
        id: bufferPage
        BufferPage { }
    }

    Component {
        id: usersPage
        Page {
            property alias channel: userModel.channel
            SilicaListView {
                anchors.fill: parent
                spacing: Theme.paddingMedium
                header: PageHeader { title: channel.title }
                model: IrcUserModel {
                    id: userModel
                    sortMethod: Irc.SortByTitle
                }
                delegate: ListItem {
                    Label {
                        text: title
                        anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge; verticalCenter: parent.verticalCenter }
                    }
                    onClicked: pageStack.replace(bufferPage, {buffer: channel.model.add(model.name)})
                }
                VerticalScrollDecorator { }
            }
        }
    }

    Component {
        id: component
        IrcConnection { }
    }

    IrcCommand {
        id: ircCommand
    }

    Component {
        id: connectDialog
        ConnectDialog {
            id: dialog
            onAccepted: {
                var connection = component.createObject(BufferModel)
                connection.host = dialog.host
                connection.port = dialog.port
                connection.secure = dialog.secure
                connection.nickName = dialog.nickName
                connection.realName = dialog.realName
                connection.userName = dialog.userName
                connection.password = dialog.password
                for (var i = 0; i < dialog.channels.length; ++i)
                    connection.sendCommand(ircCommand.createJoin(dialog.channels[i]))

                BufferModel.addConnection(connection)
                if (NetworkSession.open())
                    connection.open()
            }
        }
    }

    Component {
        id: joinDialog
        JoinDialog {
            id: dialog
            onAccepted: dialog.connection.sendCommand(ircCommand.createJoin(dialog.channel, dialog.password))
        }
    }

    Component {
        id: queryDialog
        QueryDialog {
            id: dialog
            onAccepted: pageStack.replace(bufferPage, {buffer: dialog.network.add(dialog.user)})
        }
    }
}
