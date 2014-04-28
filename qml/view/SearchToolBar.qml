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
import QtGraphicalEffects 1.0

Rectangle {
    id: root

    property alias icon: image.source
    property bool checkable: false
    property int checked: Qt.Unchecked
    property alias filter: field.text
    property bool highlighted: false

    function clear() {
        field.text = ""
    }

    color: Theme.highlightDimmerColor
    width: parent ? parent.width : 0
    height: Theme.itemSizeExtraSmall

    TextField {
        id: field
        font.pixelSize: Theme.fontSizeSmall
        textLeftMargin: icon.width + Theme.paddingMedium
        textRightMargin: clearButton.width
        anchors {
            left: parent.left
            right: parent.right
            verticalCenter: parent.verticalCenter
            verticalCenterOffset: Theme.paddingLarge
        }

        EnterKey.iconSource: "image://theme/icon-m-enter-close"
        EnterKey.onClicked: { focus = false; Qt.inputMethod.hide() }
    }

    MouseArea {
        id: icon
        width: Theme.itemSizeExtraSmall
        height: parent.height

        property bool stoleFocus: false
        onPressed: stoleFocus = field.activeFocus
        onClicked: {
            if (stoleFocus)
                field.forceActiveFocus()
            if (checkable)
                checked = (checked + 1) % (Qt.Checked + 1)
        }

        ColorOverlay {
            width: Theme.itemSizeExtraSmall - Theme.paddingLarge
            height: Theme.itemSizeExtraSmall - Theme.paddingLarge
            anchors { right: parent.right; verticalCenter: parent.verticalCenter }
            color: checked == Qt.Checked ? window.nickHighlight :
                   checked == Qt.PartiallyChecked ? Theme.highlightColor : Theme.primaryColor
            source: Image { id: image }
        }
    }

    IconButton {
        id: clearButton
        width: icon.width
        height: parent.height
        anchors { right: parent.right; rightMargin: Theme.paddingSmall }

        icon.source: "image://theme/icon-m-clear"
        opacity: field.text.length > 0 ? 1 : 0
        Behavior on opacity { FadeAnimation { } }

        property bool stoleFocus: false
        onPressed: stoleFocus = field.activeFocus
        onClicked: {
            if (stoleFocus)
                field.forceActiveFocus()
            Qt.inputMethod.commit()
            field.text = ""
        }
    }

    Separator {
        color: root.highlighted ? window.nickHighlight : Theme.highlightColor
        primaryColor: root.highlighted ? Theme.rgba(window.nickHighlight, 0.8) : Theme.rgba(Theme.highlightColor, 0.5)
        anchors { top: parent.top; left: parent.left; right: parent.right }
    }
}
