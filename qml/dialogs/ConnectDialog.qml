/*
  Copyright (C) 2013-2015 The Communi Project

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

Dialog {
    id: dialog

    property IrcConnection connection

    property string title: qsTr("Connect")
    property string defaultPort: "6667"
    property string defaultSslPort: "6697"
   // property string defaultQuasselPort: "4242"
    property string defaultNickName: qsTr("Sailor%1").arg(Math.floor(Math.random() * 12345))
    property string defaultUserName: "sailfish"
    property string defaultRealName: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)

    canAccept: !!hostField.text && !!nickNameField.text && !!userNameField.text && !!portField.text
    allowedOrientations: window.allowedOrientations

    Component.onCompleted: {
        if (connection) {
            title = qsTr("Edit")
            hostField.text = connection.host
            portField.text = connection.port
            secureBox.checked = connection.secure
            saslBox.checked = !!connection.saslMechanism
            nickNameField.text = connection.userData['originalNickName'] || connection.nickName
            alternateNickNameField.text = connection.userData['alternateNickName'] || connection.nickName + "_"
            userNameField.text = connection.userName
            realNameField.text = connection.realName
            passwordField.text = connection.password
            displayNameField.text = connection.displayName
         //   quasselBox.checked = !!connection.userData['quassel']
            commandField.text = connection.userData['commands'] || ""
        }
    }

    onAccepted: {
        if (!connection)
            connection = ircConnection.createObject(BufferModel)

        connection.host = hostField.text
        connection.port = portField.text
        connection.secure = secureBox.checked
        connection.saslMechanism = saslBox.checked ? connection.supportedSaslMechanisms[0]  : ""
        connection.nickName = nickNameField.text
        connection.userName = userNameField.text
        connection.realName = realNameField.text || defaultRealName
        connection.password = passwordField.text
        if (!!displayNameField.text)
            connection.displayName = displayNameField.text
        var userData = connection.userData
        userData['originalNickName'] = nickNameField.text
        userData['alternateNickName'] = alternateNickNameField.text
        userData['quassel'] = quasselBox.checked
        userData['commands'] = commandField.text
        connection.userData = userData
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

            SectionHeader { text: qsTr("Basic") }

            TextField {
                id: nickNameField
                width: parent.width
                label: qsTr("Nick name")
                errorHighlight: !text
                placeholderText: qsTr("Enter nick name")
                text: defaultNickName

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
            }

            TextField {
                id: realNameField
                width: parent.width
                label: qsTr("Real name")
                text: defaultRealName
                placeholderText: qsTr("Enter real name")

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
            }

            TextField {
                id: hostField
                width: parent.width
                label: qsTr("Server address")
                errorHighlight: !text
                text: "irc.freenode.net"
                placeholderText: qsTr("Enter server address")
                inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText | Qt.ImhUrlCharactersOnly

                EnterKey.iconSource: "image://theme/icon-m-enter-close"
                EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
            }

            SectionHeader { text: qsTr("Advanced") }

            ListItem {
                id: userItem
                Label {
                    text: qsTr("User details")
                    color: userItem.down ? Theme.highlightColor : Theme.secondaryColor
                    anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge; verticalCenter: parent.verticalCenter }
                }
                onClicked: pageStack.push(userPage)
            }

            ListItem {
                id: networkItem
                Label {
                    text: qsTr("Network settings")
                    color: networkItem.down ? Theme.highlightColor : Theme.secondaryColor
                    anchors { left: parent.left; right: parent.right; margins: Theme.paddingLarge; verticalCenter: parent.verticalCenter }
                }
                onClicked: pageStack.push(networkPage)
            }
        }
        VerticalScrollDecorator { }
    }

    Page {
        id: networkPage
        backNavigation: !!portField.text
        allowedOrientations: window.allowedOrientations
        SilicaListView {
            anchors.fill: parent
            spacing: Theme.paddingMedium

            header: PageHeader { title: qsTr("Network settings") }

            model: VisualItemModel {
                TextField {
                    id: displayNameField
                    width: parent.width
                    label: qsTr("Network name")
                    placeholderText: qsTr("Enter network name")

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
                }

                TextField {
                    id: portField
                    width: parent.width
                    label: qsTr("Port")
                    text: defaultPort
                    errorHighlight: !text
                    inputMethodHints: Qt.ImhDigitsOnly
                    placeholderText: qsTr("Enter port")

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
                }

                TextSwitch {
                    id: secureBox
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    description: qsTr("SSL provides a secure, encrypted connection with the server")
                    text: qsTr("Use SSL")
                    onCheckedChanged: {
                        if (checked && portField.text === defaultPort)
                            portField.text = defaultSslPort
                        else if (!checked && portField.text === defaultSslPort)
                            portField.text = defaultPort
                    }
                }

         /*       TextSwitch {
                    id: quasselBox
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    description: qsTr("The Quassel protocol support is experimental")
                    text: qsTr("Use Quassel protocol")
                    onCheckedChanged: {
                        if (checked && (portField.text === defaultPort || portField.text === defaultSslPort))
                            portField.text = defaultQuasselPort
                        else if (!checked && portField.text === defaultQuasselPort)
                            portField.text = secureBox.checked ? defaultSslPort : defaultPort
                    }
                }
*/

                TextArea {
                    id: commandField
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    placeholderText: qsTr("Perform commands")
                    label: qsTr("Commands to perform on connect")
                }
            }
            VerticalScrollDecorator { }
        }
    }

    Page {
        id: userPage
        backNavigation: !!userNameField.text
        allowedOrientations: window.allowedOrientations
        SilicaListView {
            anchors.fill: parent
            spacing: Theme.paddingMedium

            header: PageHeader { title: qsTr("User details") }

            model: VisualItemModel {
                TextField {
                    id: alternateNickNameField
                    width: parent.width
                    label: qsTr("Alternate nick name")
                    placeholderText: qsTr("Enter alternate nick name")
                    text: nickNameField.text.trim() + "_"

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
                }

                TextField {
                    id: userNameField
                    width: parent.width
                    label: qsTr("Username")
                    text: defaultUserName
                    errorHighlight: !text
                    placeholderText: qsTr("Enter user name")
                    inputMethodHints: Qt.ImhNoAutoUppercase | Qt.ImhNoPredictiveText

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
                }

                TextField {
                    id: passwordField
                    width: parent.width
                    label: qsTr("Password")
                    placeholderText: qsTr("Enter password")
                    echoMode: TextInput.Password

                    EnterKey.iconSource: "image://theme/icon-m-enter-close"
                    EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
                }

                TextSwitch {
                    id: saslBox
                    anchors { left: parent.left; right: parent.right; rightMargin: Theme.paddingLarge }
                    description: qsTr("SASL provides a secure, encrypted authentication with the server")
                    text: qsTr("Use SASL")
                }
            }
            VerticalScrollDecorator { }
        }
    }
}
