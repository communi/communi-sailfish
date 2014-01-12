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
import QtGraphicalEffects 1.0

Dialog {
    id: dialog

    property IrcConnection connection

    property string title: qsTr("Connect")
    property string defaultPort: secureBox.checked ? "6697" : "6667"
    property string defaultNickName: qsTr("Sailor%1").arg(Math.floor(Math.random() * 12345))
    property string defaultUserName: "sailfish"
    property string defaultRealName: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)

    backNavigation: BufferModel.models.length
    canAccept: !!hostField.text && !!nickNameField.text && !!userNameField.text && !!portField.text

    Component.onCompleted: {
        if (connection) {
            title = qsTr("Edit")
            hostField.text = connection.host
            portField.text = connection.port
            secureBox.checked = connection.secure
            nickNameField.text = connection.nickName
            userNameField.text = connection.userName
            realNameField.text = connection.realName
            passwordField.text = connection.password
        }
    }

    onAccepted: {
        if (!connection)
            connection = ircConnection.createObject(BufferModel)

        connection.host = hostField.text
        connection.port = portField.text
        connection.secure = secureBox.checked
        connection.nickName = nickNameField.text
        connection.userName = userNameField.text
        connection.realName = realNameField.text || defaultRealName
        connection.password = passwordField.text
    }

    Component {
        id: ircConnection
        IrcConnection { }
    }

    SilicaListView {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: DialogHeader { title: dialog.title }
        model: VisualItemModel {

            TextField {
                id: nickNameField
                width: parent.width
                label: qsTr("Nick name")
                errorHighlight: !text
                placeholderText: qsTr("Enter nick name")
                text: defaultNickName

                EnterKey.text: dialog.title
                EnterKey.enabled: dialog.canAccept
                EnterKey.highlighted: true
                EnterKey.onClicked: dialog.accept()
            }

            TextField {
                id: realNameField
                width: parent.width
                label: qsTr("Real name")
                text: defaultRealName
                placeholderText: qsTr("Enter real name")

                EnterKey.text: dialog.title
                EnterKey.enabled: dialog.canAccept
                EnterKey.highlighted: true
                EnterKey.onClicked: dialog.accept()
            }

            TextField {
                id: hostField
                width: parent.width
                label: qsTr("Server address")
                errorHighlight: !text
                text: "irc.freenode.net"
                placeholderText: qsTr("Enter server address")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly

                EnterKey.text: dialog.title
                EnterKey.enabled: dialog.canAccept
                EnterKey.highlighted: true
                EnterKey.onClicked: dialog.accept()
            }

            Item {
                width: parent.width
                height: button.height + 2 * Theme.paddingMedium

                Button {
                    id: button
                    text: qsTr("Advanced")
                    anchors.centerIn: parent
                    onClicked: pageStack.push(details)
                }
                ColorOverlay {
                    source: button
                    anchors.fill: button
                    color: !userNameField.text || !portField.text ? "#ff4d4d" : "transparent"
                }
            }
        }
        VerticalScrollDecorator { }
    }

    Page {
        id: details
        SilicaListView {
            anchors.fill: parent
            spacing: Theme.paddingMedium
            header: DialogHeader { title: qsTr("Advanced") }
            model: VisualItemModel {
                TextField {
                    id: userNameField
                    width: parent.width
                    label: qsTr("Username")
                    text: defaultUserName
                    errorHighlight: !text
                    placeholderText: qsTr("Enter user name")
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

                    EnterKey.text: dialog.title
                    EnterKey.enabled: dialog.canAccept
                    EnterKey.highlighted: true
                    EnterKey.onClicked: dialog.accept()
                }

                TextField {
                    id: passwordField
                    width: parent.width
                    label: qsTr("Password")
                    placeholderText: qsTr("Enter password")
                    echoMode: TextInput.PasswordEchoOnEdit

                    EnterKey.text: dialog.title
                    EnterKey.enabled: dialog.canAccept
                    EnterKey.highlighted: true
                    EnterKey.onClicked: dialog.accept()
                }

                TextSwitch {
                    id: secureBox
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    description: qsTr("SSL provides a secure, encrypted connection with the server")
                    text: qsTr("Use SSL")
                }

                TextField {
                    id: portField
                    width: parent.width
                    label: qsTr("Port")
                    text: defaultPort
                    errorHighlight: !text
                    inputMethodHints: Qt.ImhDigitsOnly
                    placeholderText: qsTr("Enter port")

                    EnterKey.text: dialog.title
                    EnterKey.enabled: dialog.canAccept
                    EnterKey.highlighted: true
                    EnterKey.onClicked: dialog.accept()
                }
            }
            VerticalScrollDecorator { }
        }
    }
}
