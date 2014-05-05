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

Rectangle {
    id: panel

    property bool active: true
    property bool busy: false
    property bool highlighted: false
    property int edge: Qt.LeftEdge
    property int index: edge === Qt.LeftEdge ? 0 : 2

    signal toggled()

    property var __view: null

    color: Theme.rgba(Theme.highlightDimmerColor, 0.8)

    onActiveChanged: {
        if (active)
            toggled()
        else
            closeAnimation.start()
    }

    SequentialAnimation {
        id: closeAnimation
        PropertyAction { target: __view; property: "currentIndex"; value: 1 }
        PauseAnimation { duration: 200 }
        ScriptAction { script: panel.toggled() }
    }

    GlassItem {
        id: indicator
        visible: __view
        width: Theme.paddingLarge
        height: parent.height / 2
        anchors.horizontalCenter: panel.edge === Qt.LeftEdge ? parent.right : parent.left
        anchors.verticalCenter: parent.verticalCenter
        color: panel.highlighted ? window.nickHighlight : Theme.highlightColor
        radius: 0.25
        falloffRadius: 0.17
        Behavior on falloffRadius {
            NumberAnimation { duration: panel.busy ? 450 : 50; easing.type: Easing.InOutQuad }
        }
        brightness: 1.0
        Behavior on brightness {
            NumberAnimation { duration: panel.busy ? 450 : 50; easing.type: Easing.InOutQuad }
        }
    }

    Timer {
        id: busyTimer
        running: panel.busy && Qt.application.active
        interval: 500
        repeat: true
        onRunningChanged: {
            indicator.brightness = 1.0
            indicator.falloffRadius = 0.17
        }
        onTriggered: {
            indicator.falloffRadius = indicator.falloffRadius < 0.09 ? 0.3 : 0.075
            indicator.brightness = indicator.brightness < 0.5 ? 1.0 : 0.4
        }
    }
}
