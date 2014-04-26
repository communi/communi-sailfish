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
import Communi 3.2
import Sailfish.Silica 1.0

Panel {
    id: panel

    signal clicked(IrcBuffer buffer)

    SilicaListView {
        id: listview

        property int firstIndex: -1
        property int lastIndex: -1

        onContentYChanged: {
            var first = indexAt(0, contentY + Theme.itemSizeExtraSmall * 4/3)
            if (first === -1) {
                // the first item is a section item (a network delegate)
                var firstItem = contentItem.childAt(0, contentY + Theme.itemSizeExtraSmall)
                if (firstItem && firstItem.buffer)
                    first = BufferModel.indexOf(firstItem.buffer)
            }
            if (first !== -1)
                firstIndex = first

            var last = indexAt(0, contentY + height - Theme.itemSizeExtraSmall * 1/3)
            if (last === -1) {
                // the last item is a section item (a network delegate)
                var lastItem = contentItem.childAt(0, contentY + height)
                if (lastItem && lastItem.buffer)
                    last = BufferModel.indexOf(lastItem.buffer)
            }
            if (last !== -1)
                lastIndex = last
        }

        pressDelay: 0
        anchors.fill: parent
        anchors.bottomMargin: toolbar.height - 2

        // a workaround for missing buffers
        cacheBuffer: 500 * Theme.itemSizeSmall

        model: FilterModel

        section.property: "section"
        section.labelPositioning: ViewSection.InlineLabels | ViewSection.CurrentLabelAtStart

        section.delegate: NetworkDelegate {
            buffer: BufferModel.servers[section] || null
            highlighted: section === listview.currentSection
                      && MessageStorage.firstActiveHighlight !== -1
                      && MessageStorage.firstActiveHighlight < listview.firstIndex
            onClicked: panel.clicked(buffer)
        }

        delegate: BufferDelegate {
            buffer: model.buffer || null
            onClicked: panel.clicked(model.buffer)
        }

        VerticalScrollDecorator {
            anchors.left: parent.left
            anchors.right: undefined
        }
    }

    Binding {
        target: FilterModel
        property: "filterStatus"
        value: toolbar.checked
    }

    Binding {
        target: FilterModel
        property: "filterString"
        value: toolbar.filter
    }

    SearchToolBar {
        id: toolbar
        checkable: true
        width: parent.width
        icon: "../images/chanfilter.png"
        highlighted: MessageStorage.lastActiveHighlight !== -1
                  && MessageStorage.lastActiveHighlight > listview.lastIndex
        anchors.bottom: parent.bottom
    }
}
