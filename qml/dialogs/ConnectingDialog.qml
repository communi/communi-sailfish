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

Dialog {
    id: dialog

    property var connection

    function channelNames() {
        var channels = [];
        for (var i = 0; i < setChannelsListModel.count; ++i)
            channels.push(setChannelsListModel.get(i).channelName);
        return channels;
    }

    canAccept: setChannelsListModel.count > 0

    ListModel {
        id: setChannelsListModel
        ListElement { channelName: "#jollamobile" }
        ListElement { channelName: "#sailfishos" }
    }

    DialogHeader {
        id: setChannelsDialogHeader
        title: qsTr("Connecting...")
    }

    SilicaListView {
        id: setChannelsView

        anchors {
            top: setChannelsDialogHeader.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            bottomMargin: setChannelsToolbar.height
        }
        clip: true

        spacing: Theme.paddingMedium
        header: Label {
            x: Theme.paddingLarge
            width: parent.width - 2 * Theme.paddingLarge
            wrapMode: Text.WordWrap
            color: Theme.secondaryColor
            text: qsTr("Enter channels to be joined, meanwhile the connection is being established.")
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

        // <workaround>
        // the RIGHT WAY would be to listen to Qt.inputMethod.animatingChanged instead
        // details: https://together.jolla.com/question/8611/bug-qinputmethodanimatingchanged-is-never-emitted/
        onHeightChanged: {
            positionViewAtEnd();
        }
        onContentHeightChanged: {
            positionViewAtEnd();
        }
        // </workaround>

        VerticalScrollDecorator {}
    }

    // Sort of toolbar thingy for adding new channels quickly
    Item {
        id: setChannelsToolbar
        height: addChannelField.height + 2 * Theme.paddingSmall
        anchors {
            bottom: parent.bottom
            left: parent.left
            right: parent.right
        }

        Rectangle {
            anchors.fill: parent
            color: Theme.rgba(Theme.highlightDimmerColor, 0.8)
            opacity: !setChannelsView.atYEnd ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation { } }
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

        // Makes the entire area of the thing clickable so it works even if the user doesn't tap exactly the text field
        MouseArea {
            anchors.fill: parent
            onClicked: {
                addChannelField.forceActiveFocus();
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
