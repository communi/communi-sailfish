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

FocusScope {
    id: entry

    property IrcBuffer buffer

    property bool backgroundVisible: false

    function insertName(name) {
        var txt = field.text
        if (!!txt) {
            if (txt[txt.length - 1] !== " ")
                txt += " "
            txt += name
        } else {
            txt = name + completer.suffix
        }
        field.text = txt + " "
        field.forceActiveFocus()
    }

    function sendLines(buffer, lines) {
        for (var i = 0; i < lines.length; ++i) {
            var cmd = parser.parse(lines[i])
            if (cmd) {
                if (cmd.type === IrcCommand.Custom) {
                    if (cmd.parameters[0] === "CLEAR") {
                        MessageStorage.get(buffer).clear()
                    } else if (cmd.parameters[0] === "CLOSE") {
                        pageStack.pop()
                        buffer.close(qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version))
                    } else if (cmd.parameters[0] === "QUERY") {
                        var query = buffer.model.add(cmd.parameters[1])
                        pageStack.replace(bufferPage, {buffer: query})
                        if (cmd.parameters.length > 2) {
                            var queryCmd = ircCommand.createMessage(query.title, cmd.parameters.slice(2))
                            query.sendCommand(queryCmd)
                            if (!buffer.network.isCapable("echo-message"))
                                query.receiveMessage(queryCmd.toMessage(query.connection.nickName, query.connection))
                        }
                    } else if (cmd.parameters[0] === "MSG") {
                        var msgCmd = ircCommand.createMessage(cmd.parameters[1], cmd.parameters.slice(2))
                        buffer.sendCommand(msgCmd)
                        var msgMsg = msgCmd.toMessage(buffer.connection.nickName, buffer.connection)
                        var msgBuf = buffer.model.add(msgMsg.target)
                        if (msgBuf !== buffer)
                            pageStack.replace(bufferPage, {buffer: msgBuf})
                        if (!buffer.network.isCapable("echo-message"))
                            msgBuf.receiveMessage(msgMsg)
                        else
                            msgMsg.destroy()
                    } else if (cmd.parameters[0] === "IGNORE") {
                        var imask = cmd.parameters[1]
                        if (!imask) {
                            if (IgnoreManager.ignores.length) {
                                MessageStorage.get(buffer).info(qsTr("Ignores:"))
                                for (var j = 0; j < IgnoreManager.ignores.length; ++j)
                                    MessageStorage.get(buffer).info(IgnoreManager.ignores[j])
                            } else {
                                MessageStorage.get(buffer).info(qsTr("No ignores"))
                            }
                        } else {
                            imask = IgnoreManager.addIgnore(imask)
                            MessageStorage.get(buffer).info(qsTr("Ignored: %1").arg(imask))
                        }
                    } else if (cmd.parameters[0] === "UNIGNORE") {
                        var umask = IgnoreManager.removeIgnore(cmd.parameters[1])
                        MessageStorage.get(buffer).info(qsTr("Unignored: %1").arg(umask))
                    }
                } else {
                    buffer.connection.sendCommand(cmd)
                    if (cmd.type === IrcCommand.Message
                            || cmd.type === IrcCommand.CtcpAction
                            || cmd.type === IrcCommand.Notice) {
                        if (!buffer.network.isCapable("echo-message")) {
                            var msg = cmd.toMessage(buffer.connection.nickName, buffer.connection)
                            buffer.receiveMessage(msg)
                        }
                    }
                }
                field.text = ""
            }
        }
    }

    implicitWidth: field.implicitWidth
    implicitHeight: field.implicitHeight

    focus: true
    enabled: buffer && buffer.connection.active

    MouseArea {
        anchors.fill: parent
        onReleased: field.forceActiveFocus()
        Rectangle {
            id: background
            anchors.fill: parent
            opacity: parent.pressed || entry.backgroundVisible ? 1.0 : 0.0
            Behavior on opacity { FadeAnimation { } }
            color: Theme.rgba(Theme.highlightDimmerColor, 0.8)
        }
    }

    TextField {
        id: field

        anchors.left: parent.left
        anchors.right: parent.right
        anchors.verticalCenter: parent.verticalCenter
        anchors.verticalCenterOffset: Theme.paddingLarge
        textLeftMargin: Theme.itemSizeSmall + 2 * Theme.paddingSmall

        EnterKey.text: qsTr("Send")
        EnterKey.enabled: !!text.trim()
        EnterKey.highlighted: true

        placeholderText: buffer ? (buffer.connection.active ? qsTr("Hi, %1").arg(buffer.title) : qsTr("Not connected")) : ""
        placeholderColor: Theme.secondaryHighlightColor
        inputMethodHints: Qt.ImhNoAutoUppercase
        focusOutBehavior: FocusBehavior.ClearPageFocus

        Keys.onReturnPressed: {
            var lines = text.split(/\r?\n/)
            if (lines.length > 2)
                remorse.execute(qsTr("Warning: sending %1 lines").arg(lines.length), function() { entry.sendLines(entry.buffer, lines)})
            else
                entry.sendLines(entry.buffer, lines)
        }

        Keys.onTabPressed: field.autoComplete()

        function autoComplete() {
            field.forceActiveFocus()
            Qt.inputMethod.commit()
            completer.complete(field.text, field.cursorPosition)
        }
    }

    IconButton {
        id: tabButton
        icon.source: "../images/complete.png"
        onClicked: field.autoComplete()
        highlighted: down || field.activeFocus
        width: Theme.itemSizeSmall
        height: Theme.itemSizeSmall
        anchors {
            left: parent.left
            leftMargin: Theme.paddingSmall
            verticalCenter: parent.verticalCenter
        }
    }

    IrcCommandParser {
        id: parser

        tolerant: true
        triggers: ["/"]
        channels: buffer ? buffer.model.channels : []
        target: buffer ? buffer.title : ""

        Component.onCompleted: {
            parser.addCommand(IrcCommand.CtcpAction, "ACTION <target> <message...>")
            parser.addCommand(IrcCommand.Admin, "ADMIN (<server>)")
            parser.addCommand(IrcCommand.Away, "AWAY (<reason...>)")
            parser.addCommand(IrcCommand.Info, "INFO (<server>)")
            parser.addCommand(IrcCommand.Invite, "INVITE <user> (<#channel>)")
            parser.addCommand(IrcCommand.Join, "JOIN <#channel> (<key>)")
            parser.addCommand(IrcCommand.Kick, "KICK (<#channel>) <user> (<reason...>)")
            parser.addCommand(IrcCommand.Knock, "KNOCK <#channel> (<message...>)")
            parser.addCommand(IrcCommand.List, "LIST (<channels>) (<server>)")
            parser.addCommand(IrcCommand.CtcpRequest, "CTCP <target> <command...>")
            parser.addCommand(IrcCommand.CtcpAction, "ME [target] <message...>")
            parser.addCommand(IrcCommand.Mode, "MODE (<channel/user>) (<mode>) (<arg>)")
            parser.addCommand(IrcCommand.Motd, "MOTD (<server>)")
            parser.addCommand(IrcCommand.Names, "NAMES (<#channel>)")
            parser.addCommand(IrcCommand.Nick, "NICK <nick>")
            parser.addCommand(IrcCommand.Notice, "NOTICE <#channel/user> <message...>")
            parser.addCommand(IrcCommand.Part, "PART (<#channel>) (<message...>)")
            parser.addCommand(IrcCommand.Ping, "PING (<user>)")
            parser.addCommand(IrcCommand.Quit, "QUIT (<message...>)")
            parser.addCommand(IrcCommand.Quote, "QUOTE <command> (<parameters...>)")
            parser.addCommand(IrcCommand.Stats, "STATS <query> (<server>)")
            parser.addCommand(IrcCommand.Time, "TIME (<user>)")
            parser.addCommand(IrcCommand.Topic, "TOPIC (<#channel>) (<topic...>)")
            parser.addCommand(IrcCommand.Trace, "TRACE (<target>)")
            parser.addCommand(IrcCommand.Users, "USERS (<server>)")
            parser.addCommand(IrcCommand.Version, "VERSION (<user>)")
            parser.addCommand(IrcCommand.Who, "WHO <mask>")
            parser.addCommand(IrcCommand.Whois, "WHOIS <user>")
            parser.addCommand(IrcCommand.Whowas, "WHOWAS <user>")

            parser.addCommand(IrcCommand.Custom, "CLEAR")
            parser.addCommand(IrcCommand.Custom, "CLOSE")
            parser.addCommand(IrcCommand.Custom, "QUERY <user> (<message...>)")
            parser.addCommand(IrcCommand.Custom, "MSG <user/channel> <message...>")
            parser.addCommand(IrcCommand.Custom, "IGNORE (<mask>)");
            parser.addCommand(IrcCommand.Custom, "UNIGNORE <mask>");
        }
    }

    IrcCompleter {
        id: completer
        buffer: entry.buffer
        parser: parser
        onCompleted: {
            field.text = text
            field.cursorPosition = cursor
        }
    }
}
