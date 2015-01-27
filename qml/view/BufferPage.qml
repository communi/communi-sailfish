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
import MessageFilter 1.0
import Sailfish.Silica 1.0

Page {
    id: page

    property IrcBuffer buffer
    property alias contentItem: column
    readonly property alias textEntry: entry
    readonly property alias contentHeight: view.height
    readonly property var storage: MessageStorage.get(buffer)

    allowedOrientations: window.allowedOrientations

    Binding {
        target: storage
        property: "visible"
        value: Qt.application.active && page.status !== PageStatus.Inactive
    }

    Column {
        id: column

        z: -2
        width: page.width
        height: page.height

        SilicaListView {
            id: view

            width: parent.width
            height: parent.height - entry.height

            RemorsePopup {
                id: remorse
            }

            PullDownMenu {
                id: pullDownMenu

                MenuItem {
                    text: qsTr("About IRC")
                    onClicked: pageStack.push(Qt.resolvedUrl("../dialogs/AboutDialog.qml"))
                }
                MenuItem { // TODO: remove (system app settings)
                    text: qsTr("Settings")
                    onClicked: pageStack.push(Qt.resolvedUrl("../settings/settings.qml"))
                }
                MenuItem {
                    text: qsTr("Clear messages")
                    visible: storage && storage.count
                    onClicked: {
                        // TODO: why the view position jumps?
                        storage.clear()
                    }
                }
            }

            PushUpMenu {
                id: pushUpMenu
                transform: Translate { y: entry.height }

                MenuItem {
                    text: qsTr("Join a channel")
                    visible: BufferModel.connections.length > 0
                    onClicked: pageStack.push(joinDialog, {index: BufferModel.models.indexOf(buffer.model)})
                }
                MenuItem {
                    text: qsTr("Open a query")
                    visible: BufferModel.models.length > 0
                    onClicked: pageStack.push(queryDialog, {index: BufferModel.models.indexOf(buffer.model)})
                }
                MenuItem {
                    text: qsTr("Connect a network")
                    onClicked: pageStack.push(connectDialog)
                }
            }

            header: PageHeader { title: buffer ? buffer.title : "" }

            highlightMoveDuration: 0
            highlightResizeDuration: 0
            currentIndex: storage ? storage.separator : -1
            highlight: Item {
                visible: view.currentIndex > 0 && view.currentIndex < view.count - 1
                Rectangle {
                    rotation: 90
                    width: 1
                    anchors.centerIn: parent
                    anchors.verticalCenterOffset: Math.ceil(parent.height / 2) - Theme.paddingMedium / 2
                    height: parent.width - 2 * Theme.paddingLarge
                    gradient: Gradient {
                        GradientStop { position: 0.0; color: "transparent" }
                        GradientStop { position: 0.25; color: Theme.secondaryColor }
                        GradientStop { position: 0.75; color: Theme.secondaryColor }
                        GradientStop { position: 1.0; color: "transparent" }
                    }
                }
            }

            section.property: "date"
            section.delegate: SectionHeader { text: section }

            IrcUserModel {
                id: userModel
                channel: buffer ? buffer.toChannel() : null
            }

            model: MessageFilter {
                source: storage
                showEvents: !!eventsConfig.value && (!eventsLimitConfig.value || userModel.count < eventsLimitConfig.value)
            }

            delegate: ListItem {
                id: delegate
                contentHeight: label.height + (index > 0 && index < view.count - 1 && ListView.isCurrentItem ? Theme.paddingMedium : 0)
                Text {
                    id: stamp
                    text: timestamp
                    font.pixelSize: Theme.fontSizeTiny
                    color: seen ? Theme.secondaryColor : Theme.primaryColor
                    anchors { baseline: label.baseline; left: parent.left; leftMargin: Theme.paddingLarge }
                }
                Text {
                    id: label
                    anchors { left: stamp.right; right: parent.right; leftMargin: Theme.paddingSmall; rightMargin: Theme.paddingLarge }
                    text: richtext
                    wrapMode: Text.Wrap
                    textFormat: Text.RichText
                    linkColor: Theme.highlightColor
                    font.pixelSize: fontSizeConfig.value
                    font.family: fontStyleConfig.value ? "Monospace" : Theme.fontFamily
                    onLinkActivated: Qt.openUrlExternally(link)
                    color: highlight ? window.nickHighlight : event ? Theme.secondaryColor : Theme.primaryColor
                    onWidthChanged: doLayout()
                }
                menu: Component {
                    ContextMenu {
                        id: menu
                        readonly property var urls: model.urls
                        readonly property var rawurls: model.rawurls
                        MenuItem {
                            text: qsTr("Copy")
                            onClicked: Clipboard.text = plaintext
                        }
                        MenuItem {
                            text: qsTr("Reply")
                            visible: !model.own && !model.event && buffer.channel
                            onClicked: entry.insertName(sender)
                        }
                        Repeater {
                            model: Math.min(menu.urls.length, 3)
                            MenuItem {
                                width: delegate.width
                                elide: Text.ElideMiddle
                                text: qsTr("Open %1").arg(menu.urls[index])
                                onClicked: Qt.openUrlExternally(menu.rawurls[index])
                            }
                        }
                    }
                }
            }

            Component.onCompleted: view.positionViewAtEnd()
            onCountChanged: {
                if (view.atYEnd && !view.dragging && !view.flicking)
                    positioner.restart()
            }

            // <workaround>
            // the RIGHT WAY would be to listen to Qt.inputMethod.animatingChanged instead
            // details: https://together.jolla.com/question/8611/bug-qinputmethodanimatingchanged-is-never-emitted/
            onHeightChanged: positioner.restart()
            // </workaround>

            VerticalScrollDecorator { }
        }

        TextEntry {
            id: entry
            buffer: page.buffer
            width: parent.width

            backgroundVisible: !positioner.running && !pushUpMenu.active && !view.atYEnd
            opacity: !viewer.closed || pullDownMenu.active || pushUpMenu.active ? 0.0 : 1.0
            Behavior on opacity { FadeAnimation { } }
        }
    }

    Timer {
        id: positioner
        interval: 100
        onTriggered: {
            if (!pullDownMenu.active && !pushUpMenu.active) {
                view.cancelFlick()
                view.positionViewAtEnd()
            }
        }
    }
}
