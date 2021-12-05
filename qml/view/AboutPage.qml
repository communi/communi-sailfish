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
import Sailfish.Silica 1.0

Page {
    allowedOrientations: window.allowedOrientations

    SilicaListView {
        anchors.fill: parent
        spacing: Theme.paddingLarge

        header: PageHeader {
            title: qsTr("About")
        }

        model: VisualItemModel {


            Image {
                id: communiImage
                source: "image://theme/" + AboutData.applicationName
                anchors {
                    horizontalCenter: parent.horizontalCenter
                    margins: Theme.paddingLarge
                }

                fillMode: Image.PreserveAspectFit
                width: 2/3 * parent.width
                height: 1/3 * parent.width

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            Qt.openUrlExternally(AboutData.url)
                        }
                    }
            }

            Label {
                text: AboutData.displayName + " " + Qt.application.version
                horizontalAlignment: Text.AlignHCenter
                font.pixelSize: Theme.fontSizeExtraLarge
                anchors {
                    horizontalCenter: parent.horizontalCenter
                }
            }

            Label {
                wrapMode: Text.WordWrap
                anchors {
                    left: parent ? parent.left : undefined
                    right: parent ? parent.right : undefined
                    margins: Theme.paddingLarge
                }
                font.pixelSize: Theme.fontSizeSmall
                text: AboutData.description
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
                    text: qsTr("IRC channel: %1 on %2").arg("#communi").arg("irc.libera.chat")
                }
                Label {
                    width: parent.width
                    textFormat: Text.StyledText
                    linkColor: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: qsTr("Mailing list")+": <a href='mailto:communi-sailfish@googlegroups.com'>communi-sailfish@googlegroups.com</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                }
                Label {
                    width: parent.width
                    textFormat: Text.StyledText
                    linkColor: Theme.highlightColor
                    font.pixelSize: Theme.fontSizeSmall
                    truncationMode: TruncationMode.Fade
                    text: qsTr("Bug reports")+": <a href='https://github.com/communi/communi-sailfish/issues'>https://github.com/communi/communi-sailfish/issues</a>"
                    onLinkActivated: Qt.openUrlExternally(link)
                }
            }

            SectionHeader {
                text: qsTr("Contributors")
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
                text: AboutData.contributors
            }

            Item { width: 1; height: 1 }
        }

        VerticalScrollDecorator { }
    }
}
