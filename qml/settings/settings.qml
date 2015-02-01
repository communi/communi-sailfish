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
import org.nemomobile.configuration 1.0

Page {
    id: page

    property var fontSizes: [Theme.fontSizeTiny, Theme.fontSizeExtraSmall,
                             Theme.fontSizeSmall, Theme.fontSizeMedium]
    property var sortMethods: [2 /* Irc.SortByTitle */, 3 /* Irc.SortByActivity*/ ]

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
        defaultValue: sortMethods[0]
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

    SilicaListView {
        anchors.fill: parent

        header: PageHeader { title: qsTr("Settings") } // TODO: "IRC" (system app settings)

        model: VisualItemModel {
            ComboBox {
                id: sortOrderBox
                width: parent.width
                label: qsTr("Sort views:")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Alphabetically") }
                    MenuItem { text: qsTr("By activity") }
                }
                currentIndex: page.sortMethods.indexOf(sortMethodConfig.value)
                onCurrentIndexChanged: sortMethodConfig.value = page.sortMethods[currentIndex]
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Show events")
                description: qsTr("Specifies whether joins/parts/quits are shown when the amount of user on the channel is less than:")
                checked: eventsConfig.value
                onCheckedChanged: eventsConfig.value = checked
            }

            Slider {
                width: parent.width
                stepSize: 50
                minimumValue: 0
                maximumValue: 500
                enabled: eventsConfig.value
                opacity: enabled ? 1.0 : 0.4
                value: eventsLimitConfig.value
                onValueChanged: eventsLimitConfig.value = value
                valueText: value === 0 ? qsTr("Unlimited") : qsTr("%1 users").arg(value)
            }

            SectionHeader { text: qsTr("Font") }

            ComboBox {
                id: fontSizeBox
                width: parent.width
                label: qsTr("Size:")
                menu: ContextMenu {
                    MenuItem { text: qsTr("Tiny") }
                    MenuItem { text: qsTr("Small") }
                    MenuItem { text: qsTr("Medium") }
                    MenuItem { text: qsTr("Large") }
                }
                currentIndex: page.fontSizes.indexOf(fontSizeConfig.value)
                onCurrentIndexChanged: fontSizeConfig.value = page.fontSizes[currentIndex]
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Fixed-width")
                description: qsTr("Specifies whether a monospaced font is used for messages")
                checked: fontStyleConfig.value
                onCheckedChanged: fontStyleConfig.value = checked
            }

            SectionHeader { text: qsTr("Notifications") }

            TextSwitch {
                width: parent.width
                text: qsTr("Use notification sounds")
                description: qsTr("Specifies whether notification sounds are used for highlighted messages")
                checked: notifyConfig.value
                onCheckedChanged: notifyConfig.value = checked
            }

            TextSwitch {
                width: parent.width
                text: qsTr("Use haptic feedback")
                description: qsTr("Specifies whether haptic feedback is used for highlighted messages")
                checked: feedbackConfig.value
                onCheckedChanged: feedbackConfig.value = checked
            }
        }
        VerticalScrollDecorator { }
    }
}
