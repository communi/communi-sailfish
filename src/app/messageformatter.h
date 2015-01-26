/*
  Copyright (C) 2008-2014 The Communi Project

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

#ifndef MESSAGEFORMATTER_H
#define MESSAGEFORMATTER_H

#include <QHash>
#include <QColor>
#include <QDateTime>
#include <IrcGlobal>

IRC_FORWARD_DECLARE_CLASS(IrcBuffer)
IRC_FORWARD_DECLARE_CLASS(IrcUserModel)
IRC_FORWARD_DECLARE_CLASS(IrcTextFormat)
IRC_FORWARD_DECLARE_CLASS(IrcInviteMessage)
IRC_FORWARD_DECLARE_CLASS(IrcJoinMessage)
IRC_FORWARD_DECLARE_CLASS(IrcKickMessage)
IRC_FORWARD_DECLARE_CLASS(IrcModeMessage)
IRC_FORWARD_DECLARE_CLASS(IrcNamesMessage)
IRC_FORWARD_DECLARE_CLASS(IrcNickMessage)
IRC_FORWARD_DECLARE_CLASS(IrcNoticeMessage)
IRC_FORWARD_DECLARE_CLASS(IrcNumericMessage)
IRC_FORWARD_DECLARE_CLASS(IrcPartMessage)
IRC_FORWARD_DECLARE_CLASS(IrcPongMessage)
IRC_FORWARD_DECLARE_CLASS(IrcPrivateMessage)
IRC_FORWARD_DECLARE_CLASS(IrcQuitMessage)
IRC_FORWARD_DECLARE_CLASS(IrcTopicMessage)
IRC_FORWARD_DECLARE_CLASS(IrcMessage)

class MessageFormatter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QColor baseColor READ baseColor WRITE setBaseColor)
    Q_PROPERTY(bool stripNicks READ stripNicks WRITE setStripNicks)
    Q_PROPERTY(bool detailed READ isDetailed WRITE setDetailed)

public:
    MessageFormatter(QObject* parent = 0);

    IrcBuffer* buffer() const;
    void setBuffer(IrcBuffer* buffer);

    IrcTextFormat* textFormat() const;
    void setTextFormat(IrcTextFormat* format);

    QString timeStampFormat() const;
    void setTimeStampFormat(const QString& format);

    QColor baseColor() const;
    void setBaseColor(const QColor& color);

    bool stripNicks() const;
    void setStripNicks(bool strip);

    bool isDetailed() const;
    void setDetailed(bool detailed);

    Q_INVOKABLE QString formatMessage(IrcMessage* message, Qt::TextFormat format = Qt::RichText) const;
    QString formatLine(const QString& message, const QDateTime& timeStamp = QDateTime::currentDateTime(), Qt::TextFormat format = Qt::RichText) const;
    QString formatContent(const QString& message, Qt::TextFormat format = Qt::RichText) const;

protected:
    QString formatInviteMessage(IrcInviteMessage* message, Qt::TextFormat format) const;
    QString formatJoinMessage(IrcJoinMessage* message, Qt::TextFormat format) const;
    QString formatKickMessage(IrcKickMessage* message, Qt::TextFormat format) const;
    QString formatModeMessage(IrcModeMessage* message, Qt::TextFormat format) const;
    QString formatNamesMessage(IrcNamesMessage* message, Qt::TextFormat format) const;
    QString formatNickMessage(IrcNickMessage* message, Qt::TextFormat format) const;
    QString formatNoticeMessage(IrcNoticeMessage* message, Qt::TextFormat format) const;
    QString formatNumericMessage(IrcNumericMessage* message, Qt::TextFormat format) const;
    QString formatPartMessage(IrcPartMessage* message, Qt::TextFormat format) const;
    QString formatPongMessage(IrcPongMessage* message, Qt::TextFormat format) const;
    QString formatPrivateMessage(IrcPrivateMessage* message, Qt::TextFormat format) const;
    QString formatQuitMessage(IrcQuitMessage* message, Qt::TextFormat format) const;
    QString formatTopicMessage(IrcTopicMessage* message, Qt::TextFormat format) const;
    QString formatUnknownMessage(IrcMessage* message, Qt::TextFormat format) const;

    QString formatPingReply(const QString& nick, const QString& arg, Qt::TextFormat format) const;

    QString formatNick(const QString& nick, Qt::TextFormat format, bool own = false) const;
    QString formatPrefix(const QString& prefix, Qt::TextFormat format, bool strip = true, bool own = false) const;

    QString formatIdleTime(int secs) const;

    QString formatNames(const QStringList& names, Qt::TextFormat format, int columns = 6) const;

    QString messagePrefix(IrcMessage* message) const;

private slots:
    void setNames(const QStringList& names);

private:
    struct Private {
        bool strip;
        bool detailed;
        QColor baseColor;
        IrcBuffer* buffer;
        IrcUserModel* userModel;
        QString timeStampFormat;
        IrcTextFormat* textFormat;
        QMultiHash<QChar, QString> names;
        mutable QHash<IrcBuffer*, bool> repeats[2];
    } d;
};

#endif // MESSAGEFORMATTER_H
