/* ROSA Media Player
 * Copyright (c) 2012 ROSA  <support@rosalab.ru>
 * Authors:
 *  Sergey Borovkov <sergey.borovkov@osinit.ru>
 *  Evgeniy Auzhin <evgeniy.augin@osinit.ru>
 *  Julia Mineeva <julia.mineeva@osinit.ru>
 * License: GPLv3
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation; either version 3,
 *   or (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details
 *
 *   You should have received a copy of the GNU General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef REPLY_H
#define REPLY_H

#include <QtCore/QObject>

class Request;

/**
 * @brief The PluginReply class encapsulates replies from social plugins
 *        and allows to read those replies from QML. This class is necessary because
 *        Request is interface and can not be used from QML (and it does not have signals
 *        plugins are supposed to emit for the same reasons).
 */
class Reply : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool finished READ isFinished NOTIFY finished)
    Q_PROPERTY(QString errorString READ errorString)
public:
    /**
     * @brief PluginReply
     * @param request Pointer to Request
     * @param parent
     */
    Reply(Request *request, QObject *parent = 0);

    /**
     * @brief isComplete Check if request already completed
     * @return bool
     */
    Q_INVOKABLE bool isFinished() const;

    /**
     * @brief errorString
     * @return last error string
     */
    Q_INVOKABLE QString errorString() const;

signals:
    void success(Reply *);
    void failure(Reply *);
    void finished();

private slots:
    void requestSuccess();
    void error(QString error);

private:
    QString m_errorString;

    bool m_finished;
};

#endif // REPLY_H
