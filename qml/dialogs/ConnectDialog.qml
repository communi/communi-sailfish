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
import Sailfish.Silica 1.0
import QtGraphicalEffects 1.0

Dialog {
    id: dialog

    property string host: hostField.text
    property string port: portField.text
    property alias secure: secureBox.checked
    property string nickName: nickNameField.text
    property string userName: userNameField.text
    property string realName: realNameField.text || defaultRealName
    property string password: passwordField.text
    property var channels: channelField.text.split(/[\s,]+/)

    property string defaultPort: secure ? "6697" : "6667"
    property string defaultNickName: "Communi" + Math.floor(Math.random() * 12345)
    property string defaultUserName: "communi"
    property string defaultRealName: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)

    canAccept: !!host && !!nickName && !!userName && !!port

    SilicaListView {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: DialogHeader { title: qsTr("Connect") }
        model: VisualItemModel {
            TextField {
                id: nickNameField
                width: parent.width
                label: qsTr("Nick name")
                errorHighlight: !text
                placeholderText: qsTr("Enter nick name")
                text: defaultNickName
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: realNameField.forceActiveFocus()
            }

            TextField {
                id: realNameField
                width: parent.width
                label: qsTr("Real name")
                text: defaultRealName
                placeholderText: qsTr("Enter real name")
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: hostField.forceActiveFocus()
            }

            TextField {
                id: hostField
                width: parent.width
                label: qsTr("Server address")
                errorHighlight: !text
                text: "irc.freenode.net"
                placeholderText: qsTr("Enter server address")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: channelField.forceActiveFocus()
            }

            TextField {
                id: channelField
                width: parent.width
                label: qsTr("Channels")
                text: "#sailfishos"
                placeholderText: qsTr("Enter channels")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                EnterKey.iconSource: "image://theme/icon-m-enter-next"
                EnterKey.onClicked: nickNameField.forceActiveFocus()
            }

            Item {
                width: parent.width
                height: button.height + 2 * Theme.paddingLarge
                Button {
                    id: button
                    text: qsTr("Details")
                    anchors.centerIn: parent
                    onClicked: pageStack.push(details)
                }
                ColorOverlay {
                    source: button
                    anchors.fill: button
                    color: !userName || !port ? "#ff4d4d" : "transparent"
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
            header: DialogHeader { title: qsTr("Details") }
            model: VisualItemModel {
                TextField {
                    id: userNameField
                    width: parent.width
                    label: qsTr("Username")
                    text: defaultUserName
                    errorHighlight: !text
                    placeholderText: qsTr("Enter user name")
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText
                    EnterKey.iconSource: "image://theme/icon-m-enter-next"
                    EnterKey.onClicked: passwordField.forceActiveFocus()
                }

                TextField {
                    id: passwordField
                    width: parent.width
                    label: qsTr("Password")
                    placeholderText: qsTr("Enter password")
                    echoMode: TextInput.PasswordEchoOnEdit
                    EnterKey.iconSource: "image://theme/icon-m-enter-next"
                    EnterKey.onClicked: portField.forceActiveFocus()
                }

                TextSwitch {
                    id: secureBox
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    description: qsTr("SSL provides a secure, encrypted connection with the server")
                    text: qsTr("SSL")
                }

                TextField {
                    id: portField
                    width: parent.width
                    label: qsTr("Port")
                    text: defaultPort
                    errorHighlight: !text
                    inputMethodHints: Qt.ImhDigitsOnly
                    placeholderText: qsTr("Enter port")
                    EnterKey.iconSource: "image://theme/icon-m-enter-next"
                    EnterKey.onClicked: userNameField.forceActiveFocus()
                }
            }
            VerticalScrollDecorator { }
        }
    }
}
