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
import QtFeedback 5.0
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
    onStatusChanged: {
        if (dialog.status === PageStatus.Active) {
            // Set text field to default channel names
            setChannelsPage.setChannelNames();
        }
    }

    // The default list of channels to join
    ListModel {
        id: setChannelsListModel
        ListElement { channelName: "#communi" }
        ListElement { channelName: "#jollamobile" }
        ListElement { channelName: "#sailfishos" }
    }

    SilicaListView {
        anchors.fill: parent
        spacing: Theme.paddingMedium
        header: DialogHeader { title: qsTr("Connect") }
        model: VisualItemModel {

            SectionHeader {
                text: qsTr("Basic settings")
            }

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
            }

            SectionHeader {
                text: qsTr("Advanced settings")
            }

            Item {
                width: parent.width
                height: advancedLabel.height + 2 * Theme.paddingMedium

                Label {
                    id: advancedLabel
                    width: parent.width - 2 * Theme.paddingLarge
                    text: qsTr("Tap the button to set advanced settings.")
                    enabled: false
                    wrapMode: Text.Wrap
                    color: Theme.secondaryColor
                    anchors.centerIn: parent

                }
            }

            Item {
                width: parent.width
                height: button.height + 2 * Theme.paddingMedium

                Button {
                    id: button
                    text: qsTr("Go advanced")
                    anchors.centerIn: parent
                    onClicked: pageStack.push(details)
                }
                ColorOverlay {
                    source: button
                    anchors.fill: button
                    color: !userName || !port ? "#ff4d4d" : "transparent"
                }
            }

            SectionHeader {
                text: qsTr("Channels")
            }

            Item {
                width: parent.width
                height: channelField.height + 2 * Theme.paddingMedium

                Label {
                    id: channelField
                    width: parent.width - 2 * Theme.paddingLarge
                    text: ""
                    enabled: false
                    wrapMode: Text.Wrap
                    color: Theme.secondaryColor
                    anchors.centerIn: parent

                }
            }

            Item {
                width: parent.width
                height: button.height +  Theme.paddingLarge

                Button {
                    id: setChannelsButton
                    text: qsTr("Set channels")
                    onClicked: pageStack.push(setChannelsPage)
                    anchors {
                        horizontalCenter: parent.horizontalCenter
                        top: parent.top
                    }
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
            header: DialogHeader { title: qsTr("Advanced settings") }
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

    Page {
        id: setChannelsPage
        onStatusChanged: {
            if (setChannelsPage.status === PageStatus.Deactivating) {
                addChannelField.focus = false;
                setChannelNames();
            }
            else if (setChannelsPage.status === PageStatus.Active) {
                addChannelField.forceActiveFocus();
            }
        }

        function setChannelNames() {
            var allChannelsText = "";

            // Aggregate channel names
            for (var i = 0; i < setChannelsListModel.count; i++) {
                var item = setChannelsListModel.get(i);
                if (i !== 0) {
                    allChannelsText += ", ";
                }
                allChannelsText += item.channelName;
            }

            channelField.text = allChannelsText;
        }

        SilicaListView {
            anchors {
                top: parent.top
                left: parent.left
                right: parent.right
                bottom: setChannelsToolbar.top
            }

            spacing: Theme.paddingMedium
            header: DialogHeader {
                title: qsTr("Set channels")
            }
            model: setChannelsListModel
            delegate: ListItem {
                id: listItem
                menu: Component {
                    ContextMenu {
                        MenuItem {
                            text: qsTr("Remove")
                            onClicked: {
                                // NOTE: since this is not actual important data, we don't need a remorse action here
                                setChannelsListModel.remove(index, 1);
                            }
                        }
                    }
                }
                Label {
                    x: Theme.paddingLarge
                    text: channelName
                    anchors.verticalCenter: parent.verticalCenter
                    color: listItem.highlighted ? Theme.highlightColor : Theme.primaryColor
                }
            }


            VerticalScrollDecorator {}
        }

        // Sort of toolbar thingy for adding new channels quickly
        Rectangle {
            id: setChannelsToolbar
            height: addChannelField.height + 2 * Theme.paddingSmall
            color: Theme.rgba(Theme.primaryColor, 0.15)
            anchors {
                bottom: parent.bottom
                left: parent.left
                right: parent.right
            }

            // Adds the currently entered channel to the list
            function addChannel() {
                var channelName = addChannelField.text;
                if (channelName && channelName !== "#") {
                    // Check if channel is already added
                    for (var i = 0; i < setChannelsListModel.count; i++) {
                        var item = setChannelsListModel.get(i);

                        if (item.channelName === channelName) {
                            cantAddBuzz.play();
                            return;
                        }
                    }

                    // Clear text field
                    addChannelField.text = "#";
                    // Add to list model
                    setChannelsListModel.append({ channelName: channelName });
                }
            }

            // Input for the channel to add
            TextField {
                id: addChannelField
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingSmall
                    right: parent.right
                    rightMargin: Theme.paddingSmall
                    bottom: parent.bottom
                }
                text: "#"
                label: qsTr("Enter a channel name")
                EnterKey.text: qsTr("Add")
                EnterKey.onClicked: {
                    setChannelsToolbar.addChannel();
                }
            }

            // Effect used to tell the user that the entered channel can't be added
            ThemeEffect {
                id: cantAddBuzz
                effect: ThemeEffect.PressStrong
            }
        }
    }
}
