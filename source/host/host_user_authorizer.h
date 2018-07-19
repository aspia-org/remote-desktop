//
// Aspia Project
// Copyright (C) 2018 Dmitry Chapyshev <dmitry@aspia.ru>
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program. If not, see <https://www.gnu.org/licenses/>.
//

#ifndef ASPIA_HOST__HOST_USER_AUTHORIZER_H_
#define ASPIA_HOST__HOST_USER_AUTHORIZER_H_

#include <QPointer>

#include "host/user.h"
#include "network/network_channel.h"
#include "protocol/authorization.pb.h"

namespace aspia {

class NetworkChannel;

class HostUserAuthorizer : public QObject
{
    Q_OBJECT

public:
    enum State { NotStarted, Started, Finished };

    HostUserAuthorizer(QObject* parent = nullptr);
    ~HostUserAuthorizer();

    void setUserList(const QList<User>& user_list);
    void setNetworkChannel(NetworkChannel* network_channel);

    NetworkChannel* networkChannel() { return network_channel_; }
    proto::auth::Status status() const { return status_; }
    proto::auth::SessionType sessionType() const { return session_type_; }
    QString userName() const { return user_name_; }

public slots:
    void start();
    void stop();

signals:
    void finished(HostUserAuthorizer* authorizer);
    void writeMessage(int message_id, const QByteArray& buffer);
    void readMessage();

protected:
    // QObject implementation.
    void timerEvent(QTimerEvent* event) override;

private slots:
    void messageWritten(int message_id);
    void messageReceived(const QByteArray& buffer);

private:
    void readLogonRequest(const proto::auth::LogonRequest& logon_request);
    void readClientChallenge(const proto::auth::ClientChallenge& client_challenge);
    void writeServerChallenge(const QByteArray& nonce);
    void writeLogonResult(proto::auth::Status status);

    proto::auth::Status doBasicAuthorization(const QString& user_name,
                                             const QByteArray& session_key,
                                             proto::auth::SessionType session_type);

    State state_ = NotStarted;

    QList<User> user_list_;
    QPointer<NetworkChannel> network_channel_;

    QString user_name_;
    QByteArray nonce_;
    int timer_id_ = 0;

    proto::auth::Method method_ = proto::auth::METHOD_UNKNOWN;
    proto::auth::SessionType session_type_ = proto::auth::SESSION_TYPE_UNKNOWN;
    proto::auth::Status status_ = proto::auth::STATUS_ACCESS_DENIED;

    Q_DISABLE_COPY(HostUserAuthorizer)
};

} // namespace aspia

#endif // ASPIA_HOST__HOST_USER_AUTHORIZER_H_
