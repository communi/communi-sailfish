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

QtObject {
    id: scheduler

    function push(page, properties) {
        __queue("push", page, properties)
    }

    function replace(page, properties) {
        __queue("replace", page, properties)
    }

    function pop() {
        __queue("pop")
    }

    function __queue(operation, page, properties) {
        __operations.push(operation)
        __pages.push(page)
        __properties.push(properties)
        timer.start()
    }

    function __operate(operation, page, properties) {
        pageStack[operation](page, properties)
    }

    property var __operations: []
    property var __pages: []
    property var __properties: []

    property Connections __connections: Connections {
        target: pageStack
        onBusyChanged: {
            if (!pageStack.busy && __operations.length)
                timer.start()
        }
    }

    property Timer __timer: Timer {
        id: timer
        interval: 16
        onTriggered: {
            if (!pageStack.busy && __operations.length)
                __operate(__operations.shift(), __pages.shift(), __properties.shift())
            else
                timer.start()
        }
    }
}
