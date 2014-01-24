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

Dialog {
    id: welcomePage

    canAccept: BufferModel.models.length > 0 && BufferModel.connections.some(function (c) { return c.enabled; })
    backNavigation: false

    SilicaListView {
        id: view
        anchors.fill: parent
        header: DialogHeader {
            title: view.count > 0 ? qsTr("Connect") : qsTr("Welcome to IRC")
        }
        ViewPlaceholder {
            enabled: BufferModel.connections.length === 0
            text: qsTr("Pull down to add a network")
        }
        model: BufferModel.connections
        delegate: ListItem {
            id: listItem
            showMenuOnPressAndHold: false
            contentHeight: connectionSwitch.height + Theme.paddingMedium
            menu: Component {
                ContextMenu {
                    MenuItem {
                        text: qsTr("Remove")
                        onClicked: {
                            var m = BufferModel;
                            var conn = modelData;

                            listItem.remorseAction(qsTr("Removing"), function() {
                                m.removeConnection(conn);
                            }, 3000);
                        }
                    }
                    MenuItem {
                        text: qsTr("Edit")
                        onClicked: {
                            pageStack.push(editDialog, { connection: modelData });
                        }
                    }
                }
            }

            // Switch to select which network you want to connect to
            TextSwitch {
                // This is for preventing a binding loop
                property bool bindNow: false

                id: connectionSwitch
                text: modelData.displayName
                description: modelData.nickName
                checked: modelData.enabled
                automaticCheck: false
                anchors {
                    left: parent.left
                    leftMargin: Theme.paddingLarge
                    top: parent.top
                }
                Binding {
                    target: modelData
                    property: "enabled"
                    value: connectionSwitch.checked
                    when: connectionSwitch.bindNow
                }
            }

            // This is a hack to allow single tap to toggle the switch AND press-and-hold to show the menu
            MouseArea {
                anchors.fill: connectionSwitch
                onPressAndHold: {
                    listItem.showMenu();
                }
                onClicked: {
                    connectionSwitch.bindNow = true;
                    connectionSwitch.checked = !connectionSwitch.checked;
                    connectionSwitch.bindNow = false;
                }
            }
        }

        PullDownMenu {
            MenuItem {
                text: qsTr("About IRC")
                onClicked: {
                    pageStack.push(Qt.resolvedUrl("AboutDialog.qml"))
                }
            }
            MenuItem {
                text: qsTr("Add a network")
                onClicked: {
                    pageStack.push(networkDialog);
                }
            }
        }

        VerticalScrollDecorator { }
    }
}
