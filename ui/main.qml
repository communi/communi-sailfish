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
import Qt.labs.settings 1.0
import org.nemomobile.ngf 1.0
import org.nemomobile.notifications 1.0
import "dialogs"
import "cover"
import "misc"

ApplicationWindow {
    id: window

    property Page currentPage: pageStack.currentPage
    property color nickHighlight: "#ff4d4d"

    onCurrentPageChanged: {
        if (currentPage && currentPage.buffer)
            MessageStorage.currentBuffer = currentPage.buffer
    }

    // Opens all IRC connections
    function openAllConnections() {
        if (NetworkSession.open()) {
            var buffer = null;
            for (var i = 0; i < BufferModel.connections.length; i++) {
                var connection = BufferModel.connections[i];
                if (connection.enabled) {
                    connection.open();
                    if (!buffer)
                        buffer = BufferModel.model(connection).get(0);
                }
            }

            var prev = pageStack.previousPage();
            if (prev && prev.buffer === null)
                prev.buffer = buffer;
            else
                scheduler.replace(bufferPage, { buffer: buffer });

        }
        else {
            // TODO: display error message?
        }
    }

    PageStackScheduler {
        id: scheduler
    }

    initialPage: bufferPage
    cover: CoverPage {
        id: appCover
    }
    onApplicationActiveChanged: {
        if (window.applicationActive) {
            // Clear cover
            appCover.resetCover();

            // TODO: instead of clearing all notifications here, remove them when the user actually looks at the buffer that created them
            notification.close();
        }
    }

    Binding {
        target: MessageFormatter
        property: "baseColor"
        value: Theme.highlightColor // alternatively a bit less prominent Theme.secondaryHighlightColor
    }

    Connections {
        target: Qt.application
        onAboutToQuit: {
            // These notifications become useless when the user quits the app, so let's clear them
            notification.close();
        }
    }

    Connections {
        target: MessageStorage
        onHighlighted: {
            if (Qt.application.active) {
                activeEffect.play();
            } else {
                // Increase highlight count on cover
                appCover.unreadHighlights += 1;

                notification.buffer = buffer;
                notification.previewBody = qsTr("%1: %2").arg(message.nick).arg(message.content);
                notification.body = qsTr("%1: %2").arg(message.nick).arg(message.content);
                notification.publish();
            }
        }
        onMessageCountChanged: {
            if (!window.applicationActive) {
                appCover.addActiveBuffer(bufferDisplayName);
            }
        }
    }

    Connections {
        target: NetworkSession
        onOnlineStateChanged: {
            if (NetworkSession.online && NetworkSession.open())
                BufferModel.connections.forEach(function(c) { c.open(); })
            else
                BufferModel.connections.forEach(function(c) { c.close(); })
        }
        onConnectionChanged: {
            if (NetworkSession.open())
                BufferModel.connections.forEach(function(c) { c.close(); c.open(); })
        }
    }

    Connections {
        target: BufferModel
        onNickNameReserved: {
            scheduler.push(nickDialog, {nick: connection.nickName, index: BufferModel.connections.indexOf(connection)})
        }
        onChannelKeyRequired: {
            scheduler.push(joinDialog, {channel: channel, index: BufferModel.connections.indexOf(connection)})
        }
        onBufferAboutToBeRemoved: {
            if (buffer === MessageStorage.currentBuffer) {
                var idx = buffer.model.indexOf(buffer)
                var replacement = buffer.model.get(idx + 1) || buffer.model.get(Math.max(0, idx - 1))
                if (replacement !== buffer)
                    scheduler.replace(bufferPage, {buffer: replacement})
            }
        }
        onReseted: {
            // Hacky instanceof
            if (String(window.currentPage).indexOf("WelcomeDialog") !== 0) {
                NetworkSession.enabled = false;
                scheduler.replace(welcomeDialogComponent);
            }
        }
    }

    NonGraphicalFeedback {
        id: activeEffect
        event: "chat_fg"
    }

    Notification {
        id: notification
        property IrcBuffer buffer
        category: "x-nemo.messaging.im"
        itemCount: appCover.unreadHighlights
        previewSummary: buffer ? buffer.title : ""
        summary: buffer ? qsTr("IRC highlight - %1").arg(buffer.title) : ""

        onClicked: {
            if (buffer && buffer !== MessageStorage.currentBuffer)
                scheduler.replace(bufferPage, { buffer: buffer });
            window.activate();
        }
        onClosed: buffer = null
    }

    Component {
        id: welcomeDialogComponent
        WelcomeDialog {
            onAccepted: {
                NetworkSession.enabled = true;
                window.openAllConnections();
            }
        }
    }

    Component {
        id: aboutDialogComponent
        AboutDialog { }
    }

    PanelView {
        id: viewer

        // a workaround to avoid TextAutoScroller picking up PanelView as an "outer"
        // flickable and doing undesired contentX adjustments (the right side panel
        // slides partially in) meanwhile typing/scrolling long TextEntry content
        property bool maximumFlickVelocity: false

        width: parent.width
        panelHeight: pageStack.height
        height: currentPage && currentPage.contentHeight || 0
        visible: (!!currentPage && !!currentPage.__isBufferPage) || !viewer.closed

        Binding {
            target: viewer
            property: "currentIndex"
            value: 1
            when: !!currentPage && !!currentPage.__isBufferPage
        }

        Connections {
            target: pageStack
            onCurrentPageChanged: viewer.hidePanel()
        }

        leftPanel: BufferListPanel {
            id: leftPanel
            busy: viewer.closed && !!BufferModel.connections && BufferModel.connections.some(function (c) { return c.active && !c.connected; })
            highlighted: MessageStorage.activeHighlight
            onClicked: {
                if (buffer !== MessageStorage.currentBuffer)
                    scheduler.replace(bufferPage, {buffer: buffer})
                else
                    viewer.hidePanel()
            }
        }

        rightPanel: UserListPanel {
            id: rightPanel
            buffer: MessageStorage.currentBuffer
            active: !!buffer && buffer.channel && buffer.active
            onClicked: {
                var buffer = user.channel.model.add(user.name)
                scheduler.replace(bufferPage, {buffer: buffer})
            }
        }
    }

    Component {
        id: bufferPage
        BufferPage {
            id: bp
            property bool __isBufferPage: true
            showNavigationIndicator: false
            Binding {
                target: bp.contentItem
                property: "parent"
                value: bp.status === PageStatus.Active ? viewer : bp
            }
        }
    }

    IrcCommand {
        id: ircCommand
    }

    Component {
        id: addConnectionComponent
        ConnectDialog {
            id: dialog
            title: qsTr("Add network")
            onAccepted: {
                var connection = dialog.connection;
                BufferModel.addConnection(connection);
            }
        }
    }

    Component {
        id: editDialog
        ConnectDialog {
            title: qsTr("Edit network")
        }
    }

    Component {
        id: connectDialog
        ConnectDialog {
            id: dialog
            onAccepted: {
                var connection = dialog.connection;
                BufferModel.addConnection(connection);

                if (NetworkSession.open()) {
                    connection.open();

                    if (!currentPage || !currentPage.__isBufferPage) {
                        var newBuffer = BufferModel.models[BufferModel.models.length - 1].get(0);
                        scheduler.replace(bufferPage, { buffer: newBuffer });
                    }
                }
                else {
                    // TODO: show error message?
                }
            }
        }
    }

    Component {
        id: joinDialog
        JoinDialog {
            id: dialog
            onAccepted: {
                var buffer = dialog.model.add(dialog.channel.trim())
                if (buffer.channel)
                    buffer.join(dialog.password)
                var prev = pageStack.previousPage()
                if (prev && buffer !== prev.buffer)
                    scheduler.replace(bufferPage, {buffer: buffer})
            }
        }
    }

    Component {
        id: queryDialog
        QueryDialog {
            id: dialog
            onAccepted: scheduler.replace(bufferPage, {buffer: dialog.model.add(dialog.user)})
        }
    }

    Component {
        id: nickDialog
        NickDialog {
            id: dialog
            onAccepted: dialog.model.connection.nickName = dialog.nick.trim()
        }
    }

    Settings {
        id: settings
        property var state
    }

    Component.onCompleted: {
        BufferModel.restoreState(settings.state)
        // TODO: check "connect automatically" setting and call window.openAllConnections if true
        pageStack.push(welcomeDialogComponent);
    }

    Component.onDestruction: settings.state = BufferModel.saveState()
}
