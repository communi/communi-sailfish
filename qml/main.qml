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
import Qt.labs.settings 1.0
import org.nemomobile.ngf 1.0
import org.nemomobile.notifications 1.0
import org.nemomobile.configuration 1.0
import "dialogs"
import "cover"
import "view"

ApplicationWindow {
    id: window

    property Page currentPage: pageStack.currentPage
    property color nickHighlight: "#ff4d4d"

    allowedOrientations: Orientation.Portrait | Orientation.Landscape | Orientation.LandscapeInverted

    onCurrentPageChanged: {
        if (currentPage && currentPage.buffer)
            BufferModel.currentBuffer = currentPage.buffer
    }

    function openConnections() {
        if (NetworkSession.open()) {
            var buffer = null
            for (var i = 0; i < BufferModel.connections.length; i++) {
                var connection = BufferModel.connections[i]
                if (connection.enabled) {
                    connection.open()
                    if (!buffer)
                        buffer = BufferModel.server(connection)
                }
            }

            var prev = pageStack.previousPage()
            if (prev && prev.buffer === null)
                prev.buffer = buffer
            else
                scheduler.replace(bufferPage, { buffer: buffer })
        }
    }

    PageStackScheduler {
        id: scheduler
    }

    initialPage: bufferPage
    cover: CoverPage { }

    Binding {
        target: MessageStorage
        property: "baseColor"
        value: Theme.highlightColor // alternatively a bit less prominent Theme.secondaryHighlightColor
    }

    Binding {
        target: BufferModel
        property: "sortMethod"
        value: sortMethodConfig.value
    }

    Connections {
        target: Qt.application
        onAboutToQuit: notification.close()
    }

    Connections {
        target: MessageStorage
        onHighlighted: {
            if (Qt.application.active) {
                feedback.give()
            } else {
                notification.buffer = buffer
                notification.summary = qsTr("IRC: highlight on %1").arg(buffer.title)
                notification.previewSummary = qsTr("%1 on %2:").arg(sender).arg(buffer.title)
                notification.previewBody = message
                notification.body = message
                notification.publish()
            }
        }
        onMissed: {
            if (Qt.application.active) {
                feedback.give()
            } else {
                notification.buffer = buffer
                notification.summary = qsTr("IRC: message from %1").arg(buffer.title)
                notification.previewSummary = qsTr("%1 in private:").arg(buffer.title)
                notification.previewBody = message
                notification.body = message
                notification.publish()
            }
        }
        onActiveHighlightsChanged: {
            if (MessageStorage.activeHighlights === 0)
                notification.close()
        }
    }

    Connections {
        target: NetworkSession
        onOnlineStateChanged: {
            if (NetworkSession.online && NetworkSession.open())
                BufferModel.connections.forEach(function(c) { c.open() })
            else
                BufferModel.connections.forEach(function(c) { c.close() })
        }
        onConnectionChanged: {
            if (NetworkSession.open())
                BufferModel.connections.forEach(function(c) { c.close(); c.open() })
        }
    }

    Connections {
        target: BufferModel
        onConnected: {
            var commands = connection.userData['commands']
            var server = BufferModel.server(connection)
            if (commands) {
                var cmds = []
                commands.split(/\r?\n/).forEach(function(cmd) {
                    if (cmd) {
                        if (cmd[0] !== "/")
                            cmd = "/" + cmd
                        cmds.push(cmd)
                    }
                })
                currentPage.textEntry.sendLines(server, cmds)
            }
        }
        onNickNameRequired: {
            var originalNickName = connection.userData['originalNickName']
            var alternateNickName = connection.userData['alternateNickName']
            if (!!alternateNickName && alternateNickName !== connection.nickName && alternateNickName !== reserved)
                connection.nickName = alternateNickName
            else if (!!originalNickName && originalNickName !== connection.nickName && originalNickName !== reserved)
                connection.nickName = originalNickName
            else
                scheduler.push(nickDialog, {nick: connection.nickName, index: BufferModel.connections.indexOf(connection)})
        }
        onChannelKeyRequired: {
            scheduler.push(joinDialog, {channel: channel, index: BufferModel.connections.indexOf(connection)})
        }
        onBufferAboutToBeRemoved: {
            if (buffer === BufferModel.currentBuffer) {
                var idx = buffer.model.indexOf(buffer)
                var replacement = buffer.model.get(idx + 1) || buffer.model.get(Math.max(0, idx - 1))
                if (replacement !== buffer)
                    scheduler.replace(bufferPage, {buffer: replacement})
            }
        }
        onReseted: {
            if (!window.currentPage.__isWelcomeDialog)
                scheduler.replace(welcomeDialog)
        }
    }

    ConfigurationValue {
       id: fontSizeConfig
       key: "/apps/harbour-communi/settings/fontsize"
       defaultValue: Theme.fontSizeSmall
    }

    ConfigurationValue {
       id: fontStyleConfig
       key: "/apps/harbour-communi/settings/fontstyle"
       defaultValue: false
    }

    ConfigurationValue {
        id: sortMethodConfig
        key: "/apps/harbour-communi/settings/sortmethod"
        defaultValue: Irc.SortByTitle
    }

    ConfigurationValue {
       id: eventsConfig
       key: "/apps/harbour-communi/settings/events"
       defaultValue: true
    }

    ConfigurationValue {
       id: eventsLimitConfig
       key: "/apps/harbour-communi/settings/eventslimit"
       defaultValue: 150
    }

    ConfigurationValue {
       id: notifyConfig
       key: "/apps/harbour-communi/settings/notify"
       defaultValue: true
    }

    ConfigurationValue {
       id: feedbackConfig
       key: "/apps/harbour-communi/settings/feedback"
       defaultValue: true
    }

    NonGraphicalFeedback {
        id: feedback
        event: "chat_fg"
        property real previous: 0
        function give() {
            if (!!feedbackConfig.value) {
                var current = new Date().getTime()
                if (current - previous > 750)
                    play()
                previous = current
            }
        }
    }

    Notification {
        id: notification
        property IrcBuffer buffer
        category: notifyConfig.value ? "x-nemo.messaging.im" : ""
        itemCount: MessageStorage.activeHighlights

        onClicked: {
            if (buffer && buffer !== BufferModel.currentBuffer)
                scheduler.replace(bufferPage, { buffer: buffer })
            window.activate()
        }
        onClosed: buffer = null
    }

    Component {
        id: welcomeDialog
        WelcomeDialog {
            property bool __isWelcomeDialog: true
            onAccepted: window.openConnections()
            Component.onCompleted: NetworkSession.enabled = false
            Component.onDestruction: NetworkSession.enabled = true
        }
    }

    PanelView {
        id: viewer

        // a workaround to avoid TextAutoScroller picking up PanelView as an "outer"
        // flickable and doing undesired contentX adjustments (the right side panel
        // slides partially in) meanwhile typing/scrolling long TextEntry content
        property bool maximumFlickVelocity: false

        width: pageStack.currentPage.width
        panelWidth: Screen.width / 3 * 2
        panelHeight: pageStack.currentPage.height
        height: currentPage && currentPage.contentHeight || pageStack.currentPage.height
        visible: (!!currentPage && !!currentPage.__isBufferPage) || !viewer.closed

        rotation: pageStack.currentPage.rotation

        property int ori: pageStack.currentPage.orientation

        anchors.centerIn: parent
        anchors.verticalCenterOffset: ori === Orientation.Portrait ? -(panelHeight - height) / 2 :
                                      ori === Orientation.PortraitInverted ? (panelHeight - height) / 2 : 0
        anchors.horizontalCenterOffset: ori === Orientation.Landscape ? (panelHeight - height) / 2 :
                                        ori === Orientation.LandscapeInverted ? -(panelHeight - height) / 2 : 0

        Connections {
            target: pageStack
            onCurrentPageChanged: viewer.hidePanel()
        }

        leftPanel: BufferListPanel {
            id: leftPanel
            busy: viewer.closed && !!BufferModel.connections && BufferModel.connections.some(function (c) { return c.active && !c.connected })
            highlighted: MessageStorage.activeHighlights > 0
            onClicked: {
                if (buffer !== BufferModel.currentBuffer)
                    scheduler.replace(bufferPage, {buffer: buffer})
                else
                    viewer.hidePanel()
            }
        }

        rightPanel: UserListPanel {
            id: rightPanel
            buffer: BufferModel.currentBuffer
            active: !!buffer && buffer.channel && buffer.active
            onClicked: {
                currentPage.textEntry.insertName(user.name)
                viewer.hidePanel()
            }
            onQueried: {
                var buffer = user.channel.model.add(user.name)
                scheduler.replace(bufferPage, {buffer: buffer})
            }
        }
    }

    IrcCommand {
        id: ircCommand
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

    Component {
        id: networkDialog
        ConnectDialog {
            id: dialog
            title: qsTr("Add network")
            onAccepted: {
                var connection = dialog.connection
                BufferModel.addConnection(connection)
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
                var connection = dialog.connection
                BufferModel.addConnection(connection)

                if (NetworkSession.open()) {
                    connection.open()

                    if (!currentPage || !currentPage.__isBufferPage) {
                        var newBuffer = BufferModel.servers[BufferModel.servers.length - 1]
                        scheduler.replace(bufferPage, { buffer: newBuffer })
                    }
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
        property var ignores
    }

    Component.onCompleted: {
        IgnoreManager.ignores = settings.ignores || []
        BufferModel.restoreState(settings.state)
        pageStack.push(welcomeDialog)
    }

    Component.onDestruction: {
        settings.ignores = IgnoreManager.ignores
        settings.state = BufferModel.saveState()
    }
}
