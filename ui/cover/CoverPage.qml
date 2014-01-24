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
import Sailfish.Silica 1.0

CoverBackground {
    id: cover

    anchors.fill: parent

    Image {
        source: "../images/cover.png"
        opacity: 0.1
        anchors.horizontalCenter: parent.horizontalCenter
        width: parent.width
        height: sourceSize.height * width / sourceSize.width
    }
    Label {
        id: unreadCount
        text: MessageStorage.activeHighlights
        x: Theme.paddingLarge
        y: Theme.paddingMedium
        font.pixelSize: Theme.fontSizeHuge
        font.family: Theme.fontFamilyHeading
        visible: NetworkSession.enabled
    }
    Label {
        id: unreadLabel
        text: qsTr("Unread\nhighlights")
        font.pixelSize: Theme.fontSizeExtraSmall
        font.family: Theme.fontFamilyHeading
        font.weight: Font.Light
        lineHeight: 0.8
        height: implicitHeight + Theme.paddingLarge
        anchors {
            right: parent.right
            left: unreadCount.right
            leftMargin: Theme.paddingMedium
            rightMargin: Theme.paddingLarge
            baseline: unreadCount.baseline
            baselineOffset: -implicitHeight/2
        }
        visible: NetworkSession.enabled
    }
    OpacityRampEffect {
        sourceItem: unreadLabel
    }

    Column {
        id: recentChannels
        visible: NetworkSession.enabled
        anchors {
            top: unreadCount.bottom
            left: parent.left
            right: parent.right
            bottom: parent.bottom
            margins: Theme.paddingLarge
        }
        Label {
            text: qsTr("Latest activity:")
            font.pixelSize: Theme.fontSizeExtraSmall
            font.family: Theme.fontFamilyHeading
            font.weight: Font.Light
            width: parent.width
            visible: repeater.count > 0
        }
        Repeater {
            id: repeater
            model: ActivityModel
            delegate: Label {
                color: Theme.highlightColor
                text: model.title
                elide: Text.ElideRight
                width: recentChannels.width
            }
        }
    }
    OpacityRampEffect {
        sourceItem: recentChannels
    }
}
