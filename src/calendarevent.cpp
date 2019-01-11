/*
 * Copyright (C) 2013 Jolla Ltd.
 * Contact: Robin Burchell <robin.burchell@jollamobile.com>
 *
 * You may use this file under the terms of the BSD license as follows:
 *
 * "Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 *   * Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *   * Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in
 *     the documentation and/or other materials provided with the
 *     distribution.
 *   * Neither the name of Nemo Mobile nor the names of its contributors
 *     may be used to endorse or promote products derived from this
 *     software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 * OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 */

#include "calendarevent.h"

#include <QQmlInfo>

// kcalcore
#include <KDateTime>

#include "calendarmanager.h"

NemoCalendarEvent::NemoCalendarEvent(NemoCalendarManager *manager, const QString &uid, const KDateTime &recurrenceId)
    : QObject(manager), mManager(manager), mUniqueId(uid), mRecurrenceId(recurrenceId)
{
    connect(mManager, SIGNAL(notebookColorChanged(QString)),
            this, SLOT(notebookColorChanged(QString)));
    connect(mManager, SIGNAL(eventUidChanged(QString,QString)),
            this, SLOT(eventUidChanged(QString,QString)));
}

NemoCalendarEvent::~NemoCalendarEvent()
{
}

QString NemoCalendarEvent::displayLabel() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).displayLabel;
}

QString NemoCalendarEvent::description() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).description;
}

QDateTime NemoCalendarEvent::startTime() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).startTime.dateTime();
}

QDateTime NemoCalendarEvent::endTime() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).endTime.dateTime();
}

bool NemoCalendarEvent::allDay() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).allDay;
}

NemoCalendarEvent::Recur NemoCalendarEvent::recur() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).recur;
}

QDateTime NemoCalendarEvent::recurEndDate() const
{
    return QDateTime(mManager->getEvent(mUniqueId, mRecurrenceId).recurEndDate);
}

bool NemoCalendarEvent::hasRecurEndDate() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).recurEndDate.isValid();
}

int NemoCalendarEvent::reminder() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).reminder;
}

QString NemoCalendarEvent::uniqueId() const
{
    return mUniqueId;
}

QString NemoCalendarEvent::color() const
{
    return mManager->getNotebookColor(mManager->getEvent(mUniqueId, mRecurrenceId).calendarUid);
}

bool NemoCalendarEvent::readonly() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).readonly;
}

QString NemoCalendarEvent::calendarUid() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).calendarUid;
}

QString NemoCalendarEvent::location() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).location;
}

NemoCalendarEvent::Secrecy NemoCalendarEvent::secrecy() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).secrecy;
}

NemoCalendarEvent::Response NemoCalendarEvent::ownerStatus() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).ownerStatus;
}

bool NemoCalendarEvent::rsvp() const
{
    return mManager->getEvent(mUniqueId, mRecurrenceId).rsvp;
}

bool NemoCalendarEvent::sendResponse(int response)
{
    return mManager->sendResponse(mManager->getEvent(mUniqueId, mRecurrenceId), (Response)response);
}

KDateTime NemoCalendarEvent::recurrenceId() const
{
    return mRecurrenceId;
}

QString NemoCalendarEvent::recurrenceIdString() const
{
    if (mRecurrenceId.isValid()) {
        return mRecurrenceId.toString();
    } else {
        return QString();
    }
}

// Returns the event as a VCalendar string
QString NemoCalendarEvent::vCalendar(const QString &prodId) const
{
    if (mUniqueId.isEmpty()) {
        qWarning() << "Event has no uid, returning empty VCalendar string."
                   << "Save event before calling this function";
        return "";
    }

    return mManager->convertEventToVCalendarSync(mUniqueId, prodId);
}

void NemoCalendarEvent::notebookColorChanged(QString notebookUid)
{
    if (mManager->getEvent(mUniqueId, mRecurrenceId).calendarUid == notebookUid)
        emit colorChanged();
}

void NemoCalendarEvent::eventUidChanged(QString oldUid, QString newUid)
{
    if (mUniqueId == oldUid) {
        mUniqueId = newUid;
        emit uniqueIdChanged();
        // Event uid changes when the event is moved between notebooks, calendar uid has changed
        emit calendarUidChanged();
    }
}
