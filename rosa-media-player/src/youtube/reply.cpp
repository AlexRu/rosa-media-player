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

#include "reply.h"
#include "request.h"

Reply::Reply(Request *request, QObject *parent)
    : QObject(parent)
    , m_finished(false)
{
    connect(request, SIGNAL(success()), SLOT(requestSuccess()));
    connect(request, SIGNAL(error(QString)), SLOT(error(QString)));
}

bool Reply::isFinished() const
{
    return m_finished;
}

QString Reply::errorString() const
{
    return m_errorString;
}

void Reply::error(QString error)
{
    m_errorString = error;
    m_finished = true;
    emit failure(this);
    emit finished();
}
void Reply::requestSuccess()
{
    m_finished = true;
    emit success(this);
    emit finished();
}
