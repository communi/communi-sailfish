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
    * Neither the name of the Communi Project nor the names of its
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
import Sailfish.Silica 1.0

Dialog {
    allowedOrientations: window.allowedOrientations

    SilicaListView {
        anchors.fill: parent
        spacing: Theme.paddingLarge

        header: DialogHeader {
            title: qsTr("About")
        }

        model: VisualItemModel {

            PageHeader {
                title: qsTr("%1 %2").arg(Qt.application.name).arg(Qt.application.version)
            }

            Label {
                wrapMode: Text.WordWrap
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    margins: Theme.paddingLarge
                }
                font.pixelSize: Theme.fontSizeSmall
                text: qsTr("Internet Relay Chat (IRC) is a protocol for live interactive Internet text messaging (chat) or synchronous conferencing. It is mainly designed for group communication in discussion forums, called channels, but also allows one-to-one communication via private messages.")
            }

            SectionHeader {
                text: qsTr("Contact")
            }

            Column {
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    margins: Theme.paddingLarge
                }
                Label {
                    width: parent.width
                    elide: Text.ElideRight
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: "IRC channel: #communi on irc.freenode.net"
                }
                Label {
                    width: parent.width
                    textFormat: Text.StyledText
                    linkColor: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: "Mailing list: <a href='mailto:communi-sailfish@googlegroups.com'>communi-sailfish@googlegroups.com</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                }
                Label {
                    width: parent.width
                    textFormat: Text.StyledText
                    linkColor: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: "Issue tracker: <a href='https://github.com/communi/communi-sailfish/issues'>https://github.com/communi/communi-sailfish/issues</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }

            SectionHeader {
                text: qsTr("Developers")
            }

            Label {
                textFormat: Qt.RichText
                wrapMode: Text.WordWrap
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    margins: Theme.paddingLarge
                }
                font.pixelSize: Theme.fontSizeSmall
                text: "J-P Nurmi &mdash; jpnurmi<br/>
                       Timur Kristóf &mdash; Venemo<br/>
                       Robin Burchell &mdash; w00t"
            }

            SectionHeader {
                text: qsTr("Artwork")
            }

            Label {
                textFormat: Qt.RichText
                wrapMode: Text.WordWrap
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    margins: Theme.paddingLarge
                }
                font.pixelSize: Theme.fontSizeSmall
                text: "Stephan Beyerle &mdash; Morpog<br/>
                       Stanisław Dac &mdash; stsdc"
            }
        }

        VerticalScrollDecorator { }
    }
}
