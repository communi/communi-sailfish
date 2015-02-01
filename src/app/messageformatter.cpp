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

#include "messageformatter.h"
#include <IrcTextFormat>
#include <IrcConnection>
#include <IrcUserModel>
#include <IrcMessage>
#include <IrcPalette>
#include <IrcChannel>
#include <Irc>
#include <QHash>
#include <QTime>
#include <QColor>
#include <QCoreApplication>
#include <QTextBoundaryFinder>

IRC_USE_NAMESPACE

MessageFormatter::MessageFormatter(QObject* parent) : QObject(parent)
{
    d.buffer = 0;
    d.strip = false;
    d.detailed = true;
    d.timeStampFormat = "[hh:mm:ss]";
    d.textFormat = new IrcTextFormat(this);
    d.textFormat->setSpanFormat(IrcTextFormat::SpanClass);
    d.baseColor = QColor::fromHsl(359, 102, 134);

    d.userModel = new IrcUserModel(this);
    connect(d.userModel, SIGNAL(namesChanged(QStringList)), this, SLOT(setNames(QStringList)));
}

IrcBuffer* MessageFormatter::buffer() const
{
    return d.buffer;
}

void MessageFormatter::setBuffer(IrcBuffer* buffer)
{
    if (d.buffer != buffer) {
        d.buffer = buffer;
        d.userModel->setChannel(qobject_cast<IrcChannel*>(buffer));
    }
}

IrcTextFormat* MessageFormatter::textFormat() const
{
    return d.textFormat;
}

void MessageFormatter::setTextFormat(IrcTextFormat* format)
{
    d.textFormat = format;
}

QString MessageFormatter::timeStampFormat() const
{
    return d.timeStampFormat;
}

void MessageFormatter::setTimeStampFormat(const QString& format)
{
    d.timeStampFormat = format;
}

QColor MessageFormatter::baseColor() const
{
    return d.baseColor;
}

void MessageFormatter::setBaseColor(const QColor& color)
{
    d.baseColor = color;
}

bool MessageFormatter::stripNicks() const
{
    return d.strip;
}

void MessageFormatter::setStripNicks(bool strip)
{
    d.strip = strip;
}

bool MessageFormatter::isDetailed() const
{
    return d.detailed;
}

void MessageFormatter::setDetailed(bool detailed)
{
    d.detailed = detailed;
}

QString MessageFormatter::formatMessage(IrcMessage* message, Qt::TextFormat format) const
{
    QString formatted;
    switch (message->type()) {
        case IrcMessage::Invite:
            formatted = formatInviteMessage(static_cast<IrcInviteMessage*>(message), format);
            break;
        case IrcMessage::Join:
            formatted = formatJoinMessage(static_cast<IrcJoinMessage*>(message), format);
            break;
        case IrcMessage::Kick:
            formatted = formatKickMessage(static_cast<IrcKickMessage*>(message), format);
            break;
        case IrcMessage::Mode:
            formatted = formatModeMessage(static_cast<IrcModeMessage*>(message), format);
            break;
        case IrcMessage::Names:
            formatted = formatNamesMessage(static_cast<IrcNamesMessage*>(message), format);
            break;
        case IrcMessage::Nick:
            formatted = formatNickMessage(static_cast<IrcNickMessage*>(message), format);
            break;
        case IrcMessage::Notice:
            formatted = formatNoticeMessage(static_cast<IrcNoticeMessage*>(message), format);
            break;
        case IrcMessage::Numeric:
            formatted = formatNumericMessage(static_cast<IrcNumericMessage*>(message), format);
            break;
        case IrcMessage::Part:
            formatted = formatPartMessage(static_cast<IrcPartMessage*>(message), format);
            break;
        case IrcMessage::Pong:
            formatted = formatPongMessage(static_cast<IrcPongMessage*>(message), format);
            break;
        case IrcMessage::Private:
            formatted = formatPrivateMessage(static_cast<IrcPrivateMessage*>(message), format);
            break;
        case IrcMessage::Quit:
            formatted = formatQuitMessage(static_cast<IrcQuitMessage*>(message), format);
            break;
        case IrcMessage::Topic:
            formatted = formatTopicMessage(static_cast<IrcTopicMessage*>(message), format);
            break;
        case IrcMessage::Unknown:
            formatted = formatUnknownMessage(static_cast<IrcMessage*>(message), format);
            break;
        default:
            break;
    }
    if (format == Qt::RichText && !formatted.isEmpty()) {
        const QString prefix = messagePrefix(message);
        if (!prefix.isEmpty())
            formatted.prepend(prefix + " ");
    }
    return formatLine(formatted, message->timeStamp(), format);
}

QString MessageFormatter::formatLine(const QString& message, const QDateTime& timeStamp, Qt::TextFormat format) const
{
    QString formatted = message;
    if (formatted.isEmpty())
        return QString();

    if (format == Qt::RichText) {
        QString cls = "message";
        switch (formatted.at(0).unicode()) {
            case '!': cls = "event"; break;
            case '[': cls = "notice"; break;
            case '*': cls = "action"; break;
            case '?': cls = "unknown"; break;
            default: break;
        }
        // inject link color for sailfish
        formatted.replace("<a href=", QString("<a style='color:%1' href=").arg(d.baseColor.name()));
        formatted = QCoreApplication::translate("MessageFormatter", "<span class='%1'>%2</span>").arg(cls, formatted);
    }

    if (!d.timeStampFormat.isEmpty()) {
        if (format == Qt::RichText)
            formatted = QCoreApplication::translate("MessageFormatter", "<font size='2'>%1</font> %3").arg(timeStamp.time().toString(d.timeStampFormat), formatted);
        else
            formatted = QCoreApplication::translate("MessageFormatter", "%1 %2").arg(timeStamp.time().toString(d.timeStampFormat), formatted);
    }

    return formatted;
}

QString MessageFormatter::formatInviteMessage(IrcInviteMessage* message, Qt::TextFormat format) const
{
    const QString nick = formatNick(message->nick(), format);
    return QCoreApplication::translate("MessageFormatter", "%1 invited to %3").arg(nick, message->channel());
}

QString MessageFormatter::formatJoinMessage(IrcJoinMessage* message, Qt::TextFormat format) const
{
    const bool repeat = d.repeats[format].value(d.buffer);
    if (message->isOwn())
        d.repeats[format].insert(d.buffer, false);
    const QString sender = formatPrefix(message->prefix(), format, d.strip, message->isOwn());
    if (message->isOwn() && repeat)
        return QCoreApplication::translate("MessageFormatter", "%1 rejoined %2").arg(sender, message->channel());
    else
        return QCoreApplication::translate("MessageFormatter", "%1 joined %2").arg(sender, message->channel());
}

QString MessageFormatter::formatKickMessage(IrcKickMessage* message, Qt::TextFormat format) const
{
    const QString kicker = formatNick(message->nick(), format, message->isOwn());
    const QString user = formatNick(message->user(), format, !message->user().compare(message->connection()->nickName()));
    if (d.detailed && !message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "%1 kicked %2 (%3)").arg(kicker, user, message->reason());
    else
        return QCoreApplication::translate("MessageFormatter", "%1 kicked %2").arg(kicker, user);
}

QString MessageFormatter::formatModeMessage(IrcModeMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatNick(message->nick(), format, message->isOwn());
    if (message->isReply())
        return QCoreApplication::translate("MessageFormatter", "%1 mode is %2 %3").arg(message->target(), message->mode(), message->argument());
    else
        return QCoreApplication::translate("MessageFormatter", "%1 sets mode %2 %3").arg(sender, message->mode(), message->argument());
}

QString MessageFormatter::formatNamesMessage(IrcNamesMessage* message, Qt::TextFormat format) const
{
    const bool repeat = d.repeats[format].value(d.buffer);
    d.repeats[format].insert(d.buffer, true);
    if (!repeat)
        return QCoreApplication::translate("MessageFormatter", "%1 has %2 users").arg(message->channel()).arg(message->names().count());
    QStringList names = message->names();
    qSort(names);
    return QCoreApplication::translate("MessageFormatter", "%1 has %3 users: %2").arg(message->channel(), formatNames(names, format)).arg(names.count());
}

QString MessageFormatter::formatNickMessage(IrcNickMessage* message, Qt::TextFormat format) const
{
    const QString oldNick = formatNick(message->oldNick(), format, message->isOwn());
    const QString newNick = formatNick(message->newNick(), format, message->isOwn());
    return QCoreApplication::translate("MessageFormatter", "%1 changed nick to %2").arg(oldNick, newNick);
}

QString MessageFormatter::formatNoticeMessage(IrcNoticeMessage* message, Qt::TextFormat format) const
{
    if (message->isReply()) {
        const QStringList params = message->content().split(" ", QString::SkipEmptyParts);
        const QString cmd = params.value(0);
        QString arg = params.value(1);
        if (cmd.toUpper() == "PING") {
            bool ok;
            int seconds = arg.toInt(&ok);
            if (ok) {
                QDateTime time = QDateTime::fromTime_t(seconds);
                arg = QCoreApplication::translate("MessageFormatter", "%1s").arg(time.secsTo(QDateTime::currentDateTime()));
            }
        }
        return QCoreApplication::translate("MessageFormatter", "%1 replied CTCP %2: %3").arg(formatNick(message->nick(), format), cmd, QStringList(params.mid(1)).join(" "));
    }

    const QString sender = formatNick(message->nick(), format, message->isOwn());
    const QString msg = formatContent(message->content(), format);
    if (format == Qt::PlainText)
        return msg;
    QString pfx = message->statusPrefix();
    if (!pfx.isEmpty())
        pfx.append(" ");
    return QCoreApplication::translate("MessageFormatter", "%1[%2] %3").arg(pfx, sender, msg);
}

#define P_(x) message->parameters().value(x)
#define MID_(x) QStringList(message->parameters().mid(x)).join(" ")

QString MessageFormatter::formatNumericMessage(IrcNumericMessage* message, Qt::TextFormat format) const
{
    const bool repeat = d.repeats[format].value(d.buffer);
    if (message->code() < 300)
        return !repeat ? formatContent(MID_(1), format) : QString();

    switch (message->code()) {
        case Irc::RPL_MOTDSTART:
        case Irc::RPL_MOTD:
            if (!repeat)
                return formatContent(MID_(1), format);
            return QString();
        case Irc::RPL_ENDOFMOTD:
        case Irc::ERR_NOMOTD:
            d.repeats[format].insert(d.buffer, true);
            if (repeat)
                return QCoreApplication::translate("MessageFormatter", "%1 reconnected").arg(d.buffer->connection()->nickName());
            return QString();
        case Irc::RPL_AWAY:
            return QCoreApplication::translate("MessageFormatter", "%1 is away (%2)").arg(P_(1), MID_(2));
        case Irc::RPL_ENDOFWHOIS:
            return QString();
        case Irc::RPL_WHOISOPERATOR:
        case Irc::RPL_WHOISMODES: // "is using modes"
        case Irc::RPL_WHOISREGNICK: // "is a registered nick"
        case Irc::RPL_WHOISHELPOP: // "is available for help"
        case Irc::RPL_WHOISSPECIAL: // "is identified to services"
        case Irc::RPL_WHOISHOST: // nick is connecting from <...>
        case Irc::RPL_WHOISSECURE: // nick is using a secure connection
            return MID_(1);
        case Irc::RPL_WHOISUSER:
            return QCoreApplication::translate("MessageFormatter", "%1 is %2@%3 (%4)").arg(P_(1), P_(2), P_(3), formatContent(MID_(5), format));
        case Irc::RPL_WHOISSERVER:
            return QCoreApplication::translate("MessageFormatter", "%1 connected via %2 (%3)").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOISACCOUNT: // nick user is logged in as
            return QCoreApplication::translate("MessageFormatter", "%1 %3 %2").arg(P_(1), P_(2), P_(3));
        case Irc::RPL_WHOWASUSER:
            return QCoreApplication::translate("MessageFormatter", "%1 was %2@%3 %4 %5").arg(P_(1), P_(2), P_(3), P_(4), P_(5));
        case Irc::RPL_WHOISIDLE: {
            QDateTime signon = QDateTime::fromTime_t(P_(3).toInt());
            QString idle = formatIdleTime(P_(2).toInt());
            return QCoreApplication::translate("MessageFormatter", "%1 has been online since %2 (idle for %3)").arg(P_(1), signon.toString(), idle);
        }
        case Irc::RPL_WHOISCHANNELS:
            return QCoreApplication::translate("MessageFormatter", "%1 is on channels %2").arg(P_(1), P_(2));

        case Irc::RPL_CHANNEL_URL:
            return !repeat ? QCoreApplication::translate("MessageFormatter", "%1 url is %2").arg(P_(1), formatContent(P_(2), format)) : QString();
        case Irc::RPL_CREATIONTIME:
            if (!repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(2).toInt());
                return QCoreApplication::translate("MessageFormatter", "%1 was created %2").arg(P_(1), dateTime.toString());
            }
            return QString();
        case Irc::RPL_TOPICWHOTIME:
            if (!repeat) {
                QDateTime dateTime = QDateTime::fromTime_t(P_(3).toInt());
                return QCoreApplication::translate("MessageFormatter", "%1 topic was set %2 by %3").arg(P_(1), dateTime.toString(), formatPrefix(P_(2), format, d.strip));
            }
            return QString();

        case Irc::RPL_INVITING:
            return QCoreApplication::translate("MessageFormatter", "inviting %1 to %2").arg(formatNick(P_(1), format), P_(2));
        case Irc::RPL_VERSION:
            return QCoreApplication::translate("MessageFormatter", "%1 version is %2").arg(formatNick(message->nick(), format), P_(1));
        case Irc::RPL_TIME:
            return QCoreApplication::translate("MessageFormatter", "%1 time is %2").arg(formatNick(P_(1), format), P_(2));
        case Irc::RPL_UNAWAY:
        case Irc::RPL_NOWAWAY:
            return P_(1);

        case Irc::RPL_TOPIC:
        case Irc::RPL_NAMREPLY:
        case Irc::RPL_ENDOFNAMES:
            return QString();

        default:
            return formatContent(MID_(1), format);
    }
}

QString MessageFormatter::formatPartMessage(IrcPartMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatPrefix(message->prefix(), format, d.strip, message->isOwn());
    if (d.detailed && !message->reason().isEmpty())
        return QCoreApplication::translate("MessageFormatter", "%1 parted %2 (%3)").arg(sender, message->channel(), formatContent(message->reason(), format));
    else
        return QCoreApplication::translate("MessageFormatter", "%1 parted %2").arg(sender, message->channel());
}

QString MessageFormatter::formatPongMessage(IrcPongMessage* message, Qt::TextFormat format) const
{
    return formatPingReply(message->prefix(), message->argument(), format);
}

QString MessageFormatter::formatPrivateMessage(IrcPrivateMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatNick(message->nick(), format, message->isOwn());
    const QString msg = formatContent(message->content(), format);
    if (message->isAction())
        return QCoreApplication::translate("MessageFormatter", "%1 %2").arg(message->nick(), msg);
    else if (message->isRequest())
        return QCoreApplication::translate("MessageFormatter", "%1 requested CTCP %2").arg(sender, msg.split(" ").value(0).toLower());
    else if (format == Qt::PlainText)
        return msg;
    QString pfx = message->statusPrefix();
    if (!pfx.isEmpty())
        pfx.append(" ");
    return QCoreApplication::translate("MessageFormatter", "%1%2: %3").arg(pfx, sender, msg);
}

QString MessageFormatter::formatQuitMessage(IrcQuitMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatPrefix(message->prefix(), format, d.strip, message->isOwn());
    if (!message->reason().isEmpty()) {
        if (d.detailed)
            return QCoreApplication::translate("MessageFormatter", "%1 has quit (%2)").arg(sender, formatContent(message->reason(), format));
        QString reason = message->reason();
        if (reason.contains("Ping timeout"))
            return QCoreApplication::translate("MessageFormatter", "%1 has timed out").arg(sender);
        if (reason.contains("Remote host closed the connection"))
            return QCoreApplication::translate("MessageFormatter", "%1 has disconnected").arg(sender);
        if (reason.contains("Connection reset by peer"))
            return QCoreApplication::translate("MessageFormatter", "%1 has lost connection").arg(sender);
    }
    return QCoreApplication::translate("MessageFormatter", "%1 has quit").arg(sender);
}

QString MessageFormatter::formatTopicMessage(IrcTopicMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatNick(message->nick(), format, message->isOwn());
    const QString topic = formatContent(message->topic(), format);
    const QString channel = message->channel();
    if (message->isReply()) {
        if (topic.isEmpty())
            return QCoreApplication::translate("MessageFormatter", "%1 has no topic set").arg(channel);
        return QCoreApplication::translate("MessageFormatter", "%1 topic is \"%2\"").arg(channel, topic);
    }
    return QCoreApplication::translate("MessageFormatter", "%1 sets topic \"%2\" on %3").arg(sender, topic, channel);
}

QString MessageFormatter::formatUnknownMessage(IrcMessage* message, Qt::TextFormat format) const
{
    const QString sender = formatNick(message->nick(), format);
    return QCoreApplication::translate("MessageFormatter", "%1 %2 %3").arg(sender, message->command(), message->parameters().join(" "));
}

QString MessageFormatter::formatPingReply(const QString& nick, const QString& arg, Qt::TextFormat format) const
{
    bool ok;
    int seconds = arg.toInt(&ok);
    if (ok) {
        QDateTime time = QDateTime::fromTime_t(seconds);
        QString result = QString::number(time.secsTo(QDateTime::currentDateTime()));
        return QCoreApplication::translate("MessageFormatter", "%1 replied in %2s").arg(formatNick(nick, format), result);
    }
    return QString();
}

QString MessageFormatter::formatNick(const QString& nick, Qt::TextFormat format, bool own) const
{
    if (format == Qt::PlainText)
        return nick;
    int h = qHash(nick) % 359;
    int s = own ? 0 : d.baseColor.saturation();
    int l = d.baseColor.lightness();
    return QString("<font color='%1'>%2</font>").arg(QColor::fromHsl(h, s, l).name()).arg(nick);
}

QString MessageFormatter::formatPrefix(const QString& prefix, Qt::TextFormat format, bool strip, bool own) const
{
    QString nick = formatNick(Irc::nickFromPrefix(prefix), format, own);
    if (!strip) {
        QString ident = Irc::identFromPrefix(prefix);
        QString host = Irc::hostFromPrefix(prefix);
        if (!ident.isEmpty() && !host.isEmpty())
            return QString("%1 (%2@%3)").arg(nick, ident, host);
    }
    return nick;
}

QString MessageFormatter::formatIdleTime(int secs) const
{
    QStringList idle;
    if (int days = secs / 86400)
        idle += QCoreApplication::translate("MessageFormatter", "%1 days").arg(days);
    secs %= 86400;
    if (int hours = secs / 3600)
        idle += QCoreApplication::translate("MessageFormatter", "%1 hours").arg(hours);
    secs %= 3600;
    if (int mins = secs / 60)
        idle += QCoreApplication::translate("MessageFormatter", "%1 mins").arg(mins);
    idle += QCoreApplication::translate("MessageFormatter", "%1 secs").arg(secs % 60);
    return idle.join(" ");
}

QString MessageFormatter::formatContent(const QString& message, Qt::TextFormat format) const
{
    d.textFormat->parse(message);
    if (format == Qt::PlainText)
        return d.textFormat->plainText();

    QString msg = d.textFormat->html();
    if (!d.names.isEmpty()) {
        QTextBoundaryFinder finder = QTextBoundaryFinder(QTextBoundaryFinder::Word, msg);
        int pos = 0;
        while (pos < msg.length()) {
            const QChar c = msg.at(pos);
            if (!c.isSpace()) {
                // do not format nicks within links
                if (c == '<' && msg.midRef(pos, 3) == "<a ") {
                    const int end = msg.indexOf("</a>", pos + 3);
                    if (end != -1) {
                        pos = end + 4;
                        continue;
                    }
                }
                // test word start boundary
                finder.setPosition(pos);
                if (finder.isAtBoundary()) {
                    QMultiHash<QChar, QString>::const_iterator it = d.names.find(c);
                    while (it != d.names.constEnd() && it.key() == c) {
                        const QString& user = it.value();
                        if (msg.midRef(pos, user.length()) == user) {
                            // test word end boundary
                            finder.setPosition(pos + user.length());
                            if (finder.isAtBoundary()) {
                                const QString formatted = formatNick(user, format);
                                msg.replace(pos, user.length(), formatted);
                                pos += formatted.length();
                                finder = QTextBoundaryFinder(QTextBoundaryFinder::Word, msg);
                            }
                        }
                        ++it;
                    }
                }
            }
            ++pos;
        }
    }
    return msg;
}

QString MessageFormatter::formatNames(const QStringList &names, Qt::TextFormat format, int columns) const
{
    if (format == Qt::PlainText)
        return names.join(" ");
    QString message;
    message += "<table>";
    for (int i = 0; i < names.count(); i += columns)
    {
        message += "<tr>";
        for (int j = 0; j < columns; ++j) {
            QString nick = Irc::nickFromPrefix(names.value(i+j));
            if (nick.isEmpty())
                nick = names.value(i+j);
            message += "<td>" + formatNick(nick, format) + "&nbsp;</td>";
        }
        message += "</tr>";
    }
    message += "</table>";
    return message;
}

QString MessageFormatter::messagePrefix(IrcMessage* message) const
{
    switch (message->type()) {
        case IrcMessage::Private:
            if (IrcPrivateMessage* priv = static_cast<IrcPrivateMessage*>(message)) {
                if (priv->isAction())
                    return tr("*");
                if (priv->isRequest())
                    return tr("!");
            }
            return QString();
        case IrcMessage::Notice:
            if (IrcNoticeMessage* notice = static_cast<IrcNoticeMessage*>(message)) {
                if (notice->isReply())
                    return tr("!");
            }
            return QString();
        case IrcMessage::Numeric:
            if (static_cast<IrcNumericMessage*>(message)->code() < 300)
                return tr("[INFO]");
            if (Irc::codeToString(static_cast<IrcNumericMessage*>(message)->code()).startsWith("ERR_"))
                return tr("[ERROR]");
            switch (static_cast<IrcNumericMessage*>(message)->code()) {
                case Irc::RPL_MOTDSTART:
                case Irc::RPL_MOTD:
                    return tr("[MOTD]");
                case Irc::RPL_ENDOFMOTD:
                case Irc::ERR_NOMOTD:
                case Irc::RPL_AWAY:
                case Irc::RPL_WHOISOPERATOR:
                case Irc::RPL_WHOISMODES:
                case Irc::RPL_WHOISREGNICK:
                case Irc::RPL_WHOISHELPOP:
                case Irc::RPL_WHOISSPECIAL:
                case Irc::RPL_WHOISHOST:
                case Irc::RPL_WHOISSECURE:
                case Irc::RPL_WHOISUSER:
                case Irc::RPL_WHOISSERVER:
                case Irc::RPL_WHOISACCOUNT:
                case Irc::RPL_WHOWASUSER:
                case Irc::RPL_WHOISIDLE:
                case Irc::RPL_WHOISCHANNELS:
                case Irc::RPL_CHANNEL_URL:
                case Irc::RPL_CREATIONTIME:
                case Irc::RPL_TOPICWHOTIME:
                case Irc::RPL_INVITING:
                case Irc::RPL_VERSION:
                case Irc::RPL_TIME:
                case Irc::RPL_UNAWAY:
                case Irc::RPL_NOWAWAY:
                    return tr("!");
                default:
                    break;
            }
            return tr("[%1]").arg(static_cast<IrcNumericMessage*>(message)->code());
        case IrcMessage::Unknown:
            return tr("?");
        default:
            return tr("!");
    }
}

void MessageFormatter::setNames(const QStringList& names)
{
    d.names.clear();
    foreach (const QString& name, names) {
        if (!name.isEmpty())
            d.names.insert(name.at(0), name);
    }
}
