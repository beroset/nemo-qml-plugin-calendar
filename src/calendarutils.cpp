/*
 * Copyright (C) 2015 Jolla Ltd.
 * Contact: Petri M. Gerdt <petri.gerdt@jollamobile.com>
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

#include "calendarutils.h"

#include "calendareventquery.h"

// kcalcore
#include <icalformat.h>
#include <vcalformat.h>

//mkcal
#include <servicehandler.h>

// Qt
#include <QFile>
#include <QUrl>
#include <QString>
#include <QByteArray>
#include <QtDebug>

NemoCalendarEvent::Recur NemoCalendarUtils::convertRecurrence(const KCalCore::Event::Ptr &event)
{
    if (!event->recurs())
        return NemoCalendarEvent::RecurOnce;

    if (event->recurrence()->rRules().count() != 1)
        return NemoCalendarEvent::RecurCustom;

    ushort rt = event->recurrence()->recurrenceType();
    int freq = event->recurrence()->frequency();

    if (rt == KCalCore::Recurrence::rDaily && freq == 1) {
        return NemoCalendarEvent::RecurDaily;
    } else if (rt == KCalCore::Recurrence::rWeekly && freq == 1) {
        return NemoCalendarEvent::RecurWeekly;
    } else if (rt == KCalCore::Recurrence::rWeekly && freq == 2) {
        return NemoCalendarEvent::RecurBiweekly;
    } else if (rt == KCalCore::Recurrence::rMonthlyDay && freq == 1) {
        return NemoCalendarEvent::RecurMonthly;
    } else if (rt == KCalCore::Recurrence::rYearlyMonth && freq == 1) {
        return NemoCalendarEvent::RecurYearly;
    }

    return NemoCalendarEvent::RecurCustom;
}

NemoCalendarEvent::Secrecy NemoCalendarUtils::convertSecrecy(const KCalCore::Event::Ptr &event)
{
    KCalCore::Incidence::Secrecy s = event->secrecy();
    switch (s) {
    case KCalCore::Incidence::SecrecyPrivate:
        return NemoCalendarEvent::SecrecyPrivate;
    case KCalCore::Incidence::SecrecyConfidential:
        return NemoCalendarEvent::SecrecyConfidential;
    case KCalCore::Incidence::SecrecyPublic:
    default:
        return NemoCalendarEvent::SecrecyPublic;
    }
}

int NemoCalendarUtils::getReminderSeconds(const KCalCore::Event::Ptr &event, bool *hasReminder)
{
    KCalCore::Alarm::List alarms = event->alarms();

    KCalCore::Alarm::Ptr alarm;

    for (int ii = 0; ii < alarms.count(); ++ii) {
        if (alarms.at(ii)->type() == KCalCore::Alarm::Procedure)
            continue;
        alarm = alarms.at(ii);
        break;
    }

    if (!alarm) {
        *hasReminder = false;
        return 0;
    }

    KCalCore::Duration d = alarm->startOffset();
    *hasReminder = true;
    return d.asSeconds();
}

NemoCalendarEvent::Reminder NemoCalendarUtils::getReminder(const KCalCore::Event::Ptr &event)
{
    bool hasReminder = false;
    int sec = getReminderSeconds(event, &hasReminder);

    if (!hasReminder) {
        return NemoCalendarEvent::ReminderNone;
    }

    switch (sec) {
    case 0:
        return NemoCalendarEvent::ReminderTime;
    case -5 * 60:
        return NemoCalendarEvent::Reminder5Min;
    case -15 * 60:
        return NemoCalendarEvent::Reminder15Min;
    case -30 * 60:
        return NemoCalendarEvent::Reminder30Min;
    case -60 * 60:
        return NemoCalendarEvent::Reminder1Hour;
    case -2 * 60 * 60:
        return NemoCalendarEvent::Reminder2Hour;
    case -24 * 60 * 60:
        return NemoCalendarEvent::Reminder1Day;
    case -2 * 24 * 60 * 60:
        return NemoCalendarEvent::Reminder2Day;
    default:
        return NemoCalendarEvent::ReminderNone;
    }
}

QList<NemoCalendarData::Attendee> NemoCalendarUtils::getEventAttendees(const KCalCore::Event::Ptr &event, const QString &ownerEmail)
{
    QList<NemoCalendarData::Attendee> result;
    KCalCore::Person::Ptr calOrganizer = event->organizer();

    NemoCalendarData::Attendee organizer;

    if (!calOrganizer.isNull() && !calOrganizer->isEmpty()) {
        organizer.isOrganizer = true;
        organizer.name = calOrganizer->name();
        organizer.email = calOrganizer->email();
        organizer.isOwner = organizer.email == ownerEmail;
        organizer.participationRole = KCalCore::Attendee::Chair;
        result.append(organizer);
    }

    KCalCore::Attendee::List attendees = event->attendees();
    NemoCalendarData::Attendee attendee;
    attendee.isOrganizer = false;
    attendee.isOwner = false;

    foreach (KCalCore::Attendee::Ptr calAttendee, attendees) {
        attendee.name = calAttendee->name();
        attendee.email = calAttendee->email();
        if (attendee.name == organizer.name && attendee.email == organizer.email) {
            // avoid duplicate info
            continue;
        }
        attendee.isOwner = attendee.email == ownerEmail;
        if (attendee.isOwner) {
            attendee.status = calAttendee->status();
        }
        attendee.participationRole = calAttendee->role();
        result.append(attendee);
    }

    return result;
}

QList<QObject *> NemoCalendarUtils::convertAttendeeList(const QList<NemoCalendarData::Attendee> &list)
{
    QList<QObject*> result;
    foreach (const NemoCalendarData::Attendee &attendee, list) {
        QObject *person = new Person(attendee.name, attendee.email, attendee.isOrganizer,
                                     attendee.participationRole);
        result.append(person);
    }

    return result;
}

NemoCalendarData::EventOccurrence NemoCalendarUtils::getNextOccurrence(const KCalCore::Event::Ptr &event,
                                                                       const QDateTime &start)
{
    NemoCalendarData::EventOccurrence occurrence;
    if (event) {
        QDateTime dtStart = event->dtStart().toLocalZone().dateTime();
        QDateTime dtEnd = event->dtEnd().toLocalZone().dateTime();

        if (!start.isNull() && event->recurs()) {
            KDateTime startTime = KDateTime(start, KDateTime::Spec(KDateTime::LocalZone));
            KCalCore::Recurrence *recurrence = event->recurrence();
            if (recurrence->recursAt(startTime)) {
                dtStart = startTime.toLocalZone().dateTime();
                dtEnd = KCalCore::Duration(event->dtStart(), event->dtEnd()).end(startTime).toLocalZone().dateTime();
            } else {
                KDateTime match = recurrence->getNextDateTime(startTime);
                if (match.isNull())
                    match = recurrence->getPreviousDateTime(startTime);

                if (!match.isNull()) {
                    dtStart = match.toLocalZone().dateTime();
                    dtEnd = KCalCore::Duration(event->dtStart(), event->dtEnd()).end(match).toLocalZone().dateTime();
                }
            }
        }

        occurrence.eventUid = event->uid();
        occurrence.recurrenceId = event->recurrenceId();
        occurrence.startTime = dtStart;
        occurrence.endTime = dtEnd;
    }

    return occurrence;
}

bool NemoCalendarUtils::importFromFile(const QString &fileName,
                                       KCalCore::Calendar::Ptr calendar)
{
    QString filePath;
    QUrl url(fileName);
    if (url.isLocalFile())
        filePath = url.toLocalFile();
    else
        filePath = fileName;

    if (!(filePath.endsWith(".vcs") || filePath.endsWith(".ics"))) {
        qWarning() << "Unsupported file format" << filePath;
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Unable to open file for reading" << filePath;
        return false;
    }
    QByteArray fileContent(file.readAll());

    bool ok = false;
    if (filePath.endsWith(".vcs")) {
        KCalCore::VCalFormat vcalFormat;
        ok = vcalFormat.fromRawString(calendar, fileContent);
    } else if (filePath.endsWith(".ics")) {
        KCalCore::ICalFormat icalFormat;
        ok = icalFormat.fromRawString(calendar, fileContent);
    }
    if (!ok)
        qWarning() << "Failed to import from file" << filePath;

    return ok;
}

bool NemoCalendarUtils::importFromIcsRawData(const QByteArray &icsData,
                                             KCalCore::Calendar::Ptr calendar)
{
    bool ok = false;
    KCalCore::ICalFormat icalFormat;
    ok = icalFormat.fromRawString(calendar, icsData);
    if (!ok)
        qWarning() << "Failed to import from raw data";

    return ok;
}

NemoCalendarEvent::Response NemoCalendarUtils::convertPartStat(KCalCore::Attendee::PartStat status)
{
    switch (status) {
    case KCalCore::Attendee::Accepted:
        return NemoCalendarEvent::ResponseAccept;
    case KCalCore::Attendee::Declined:
        return NemoCalendarEvent::ResponseDecline;
    case KCalCore::Attendee::Tentative:
        return NemoCalendarEvent::ResponseTentative;
    case KCalCore::Attendee::NeedsAction:
    case KCalCore::Attendee::None:
    default:
        return NemoCalendarEvent::ResponseUnspecified;
    }
}

KCalCore::Attendee::PartStat NemoCalendarUtils::convertResponse(NemoCalendarEvent::Response response)
{
    switch (response) {
    case NemoCalendarEvent::ResponseAccept:
        return KCalCore::Attendee::Accepted;
    case NemoCalendarEvent::ResponseTentative:
        return KCalCore::Attendee::Tentative;
    case NemoCalendarEvent::ResponseDecline:
        return KCalCore::Attendee::Declined;
    default:
        return KCalCore::Attendee::NeedsAction;
    }
}
