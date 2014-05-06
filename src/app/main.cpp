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

#include <QGuiApplication>
#include <QQuickView>
#include <QtQml>

#if QT_VERSION < 0x050200
#include "qqmlsettings_p.h"
#endif

#include <sailfishapp.h>

#include "activitymodel.h"
#include "bufferfiltermodel.h"
#include "bufferproxymodel.h"
#include "networksession.h"
#include "messagestorage.h"
#include "messageformatter.h"
#include "stringfiltermodel.h"
#include "messagefilter.h"

#include <IrcCore>
#include <IrcModel>
#include <IrcUtil>

IRC_USE_NAMESPACE

static void registerCommuniTypes()
{
    Irc::registerMetaTypes();

    qRegisterMetaType<IrcBuffer*>("IrcBuffer*");
    qRegisterMetaType<IrcBufferModel*>("IrcBufferModel*");
    qRegisterMetaType<IrcChannel*>("IrcChannel*");
    qRegisterMetaType<IrcUser*>("IrcUser*");
    qRegisterMetaType<IrcUserModel*>("IrcUserModel*");

    qRegisterMetaType<IrcCommandParser*>("IrcCommandParser*");
    qRegisterMetaType<IrcCompleter*>("IrcCompleter*");
    qRegisterMetaType<IrcLagTimer*>("IrcLagTimer*");
    qRegisterMetaType<IrcPalette*>("IrcPalette*");
    qRegisterMetaType<IrcTextFormat*>("IrcTextFormat*");
}

Q_DECL_EXPORT int main(int argc, char* argv[])
{
    QScopedPointer<QGuiApplication> app(SailfishApp::application(argc, argv));

    QGuiApplication::setApplicationName("IRC for Sailfish");
    QGuiApplication::setOrganizationName("harbour-communi");
    QGuiApplication::setApplicationVersion(APP_VERSION);

    QScopedPointer<QQuickView> viewer(SailfishApp::createView());
    viewer->engine()->addImportPath("/usr/share/harbour-communi/qml");

#if QT_VERSION < 0x050200
    qmlRegisterType<QQmlSettings>("Qt.labs.settings", 1, 0, "Settings");
#endif

    registerCommuniTypes();

    qRegisterMetaType<QAbstractItemModel*>();
    qmlRegisterType<MessageFilter>("MessageFilter", 1, 0, "MessageFilter");
    qmlRegisterType<StringFilterModel>("StringFilterModel", 1, 0, "StringFilterModel");

    NetworkSession* session = new NetworkSession(app.data());
    viewer->rootContext()->setContextProperty("NetworkSession", session);

    BufferProxyModel* model = new BufferProxyModel(app.data());
    viewer->rootContext()->setContextProperty("BufferModel", model);

    MessageStorage* storage = new MessageStorage(model);
    viewer->rootContext()->setContextProperty("MessageStorage", storage);
    QObject::connect(model, SIGNAL(bufferAdded(IrcBuffer*)), storage, SLOT(add(IrcBuffer*)));

    BufferFilterModel* filter = new BufferFilterModel(storage);
    filter->setSourceModel(model);
    viewer->rootContext()->setContextProperty("FilterModel", filter);

    ActivityModel* activity = new ActivityModel(app.data());
    viewer->rootContext()->setContextProperty("ActivityModel", activity);
    QObject::connect(storage, SIGNAL(added(MessageModel*)), activity, SLOT(add(MessageModel*)));
    QObject::connect(storage, SIGNAL(removed(MessageModel*)), activity, SLOT(remove(MessageModel*)));

    viewer->setSource(QUrl("qrc:///qml/main.qml"));
    viewer->showFullScreen();

    return app->exec();
}
