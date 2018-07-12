//
// PROJECT:         Aspia
// FILE:            host/host_settings.cc
// LICENSE:         GNU General Public License 3
// PROGRAMMERS:     Dmitry Chapyshev (dmitry@aspia.ru)
//

#include "build_config.h"
#include "host/host_settings.h"

#include <QDebug>

namespace aspia {

HostSettings::HostSettings()
    : settings_(QSettings::SystemScope, QStringLiteral("Aspia"), QStringLiteral("Host"))
{
    // Nothing
}

bool HostSettings::isWritable() const
{
    return settings_.isWritable();
}

// static
QString HostSettings::defaultLocale()
{
    return QStringLiteral("en");
}

QString HostSettings::locale() const
{
    return settings_.value(QStringLiteral("Locale"), defaultLocale()).toString();
}

void HostSettings::setLocale(const QString& locale)
{
    settings_.setValue(QStringLiteral("Locale"), locale);
}

int HostSettings::tcpPort() const
{
    return settings_.value(QStringLiteral("TcpPort"), kDefaultHostTcpPort).toInt();
}

bool HostSettings::setTcpPort(int port)
{
    if (!settings_.isWritable())
        return false;

    settings_.setValue(QStringLiteral("TcpPort"), port);
    return true;
}

QList<User> HostSettings::userList() const
{
    QList<User> user_list;

    int size = settings_.beginReadArray(QStringLiteral("UserList"));
    for (int i = 0; i < size; ++i)
    {
        settings_.setArrayIndex(i);

        User user;

        QString user_name = settings_.value(QStringLiteral("UserName")).toString();
        if (!user.setName(user_name))
        {
            qDebug() << "Invalid user name: " << user_name << ". The list of users is corrupted";
            return QList<User>();
        }

        QByteArray password_hash = settings_.value(QStringLiteral("PasswordHash")).toByteArray();
        if (!user.setPasswordHash(password_hash))
        {
            qDebug() << "Invalid password hash: " << password_hash
                     << ". The list of users is corrupted";
            return QList<User>();
        }

        user.setFlags(settings_.value(QStringLiteral("Flags")).toUInt());
        user.setSessions(settings_.value(QStringLiteral("Sessions")).toUInt());

        user_list.push_back(user);
    }
    settings_.endArray();

    return user_list;
}

bool HostSettings::setUserList(const QList<User>& user_list)
{
    if (!settings_.isWritable())
        return false;

    settings_.remove(QStringLiteral("UserList"));

    settings_.beginWriteArray(QStringLiteral("UserList"));
    for (int i = 0; i < user_list.size(); ++i)
    {
        settings_.setArrayIndex(i);

        const User& user = user_list.at(i);

        settings_.setValue(QStringLiteral("UserName"), user.name());
        settings_.setValue(QStringLiteral("PasswordHash"), user.passwordHash());
        settings_.setValue(QStringLiteral("Flags"), user.flags());
        settings_.setValue(QStringLiteral("Sessions"), user.sessions());
    }
    settings_.endArray();

    return true;
}

} // namespace aspia
