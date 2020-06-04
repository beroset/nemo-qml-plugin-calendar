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

#ifndef CALENDAREVENT_H
#define CALENDAREVENT_H

#include <QObject>
#include <QDateTime>

#include <KDateTime>

class CalendarManager;

class CalendarEvent : public QObject
{
    Q_OBJECT
    Q_ENUMS(Reminder)
    Q_ENUMS(TimeSpec)
    Q_ENUMS(Secrecy)
    Q_ENUMS(Response)

    Q_PROPERTY(QString displayLabel READ displayLabel NOTIFY displayLabelChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QDateTime startTime READ startTime NOTIFY startTimeChanged)
    Q_PROPERTY(QDateTime endTime READ endTime NOTIFY endTimeChanged)
    Q_PROPERTY(bool allDay READ allDay NOTIFY allDayChanged)
    Q_PROPERTY(CalendarEvent::Recur recur READ recur NOTIFY recurChanged)
    Q_PROPERTY(QDateTime recurEndDate READ recurEndDate NOTIFY recurEndDateChanged)
    Q_PROPERTY(bool hasRecurEndDate READ hasRecurEndDate NOTIFY hasRecurEndDateChanged)
    Q_PROPERTY(int reminder READ reminder NOTIFY reminderChanged)
    Q_PROPERTY(QString uniqueId READ uniqueId NOTIFY uniqueIdChanged)
    Q_PROPERTY(QString recurrenceId READ recurrenceIdString CONSTANT)
    Q_PROPERTY(QString color READ color NOTIFY colorChanged)
    Q_PROPERTY(bool readOnly READ readOnly CONSTANT)
    Q_PROPERTY(QString calendarUid READ calendarUid NOTIFY calendarUidChanged)
    Q_PROPERTY(QString location READ location NOTIFY locationChanged)
    Q_PROPERTY(CalendarEvent::Secrecy secrecy READ secrecy NOTIFY secrecyChanged)
    Q_PROPERTY(CalendarEvent::Response ownerStatus READ ownerStatus NOTIFY ownerStatusChanged)
    Q_PROPERTY(bool rsvp READ rsvp NOTIFY rsvpChanged)
    Q_PROPERTY(bool externalInvitation READ externalInvitation NOTIFY externalInvitationChanged)

public:
    enum Recur {
        RecurOnce,
        RecurDaily,
        RecurWeekly,
        RecurBiweekly,
        RecurMonthly,
        RecurMonthlyByDayOfWeek,
        RecurYearly,
        RecurCustom
    };
    Q_ENUM(Recur)

    enum TimeSpec {
        SpecLocalZone,
        SpecClockTime
    };

    enum Secrecy {
        SecrecyPublic,
        SecrecyPrivate,
        SecrecyConfidential
    };

    enum Response {
        ResponseUnspecified,
        ResponseAccept,
        ResponseTentative,
        ResponseDecline
    };

    CalendarEvent(CalendarManager *manager, const QString &uid, const KDateTime &recurrenceId);
    ~CalendarEvent();

    QString displayLabel() const;
    QString description() const;
    QDateTime startTime() const;
    QDateTime endTime() const;
    bool allDay() const;
    Recur recur() const;
    QDateTime recurEndDate() const;
    bool hasRecurEndDate() const;
    int reminder() const;
    QString uniqueId() const;
    QString color() const;
    bool readOnly() const;
    QString calendarUid() const;
    QString location() const;
    KDateTime recurrenceId() const;
    QString recurrenceIdString() const;
    Secrecy secrecy() const;
    Response ownerStatus() const;
    bool rsvp() const;
    bool externalInvitation() const;

    Q_INVOKABLE bool sendResponse(int response);
    Q_INVOKABLE QString iCalendar(const QString &prodId = QString()) const;

private slots:
    void notebookColorChanged(QString notebookUid);
    void eventUidChanged(QString oldUid, QString newUid);

signals:
    void displayLabelChanged();
    void descriptionChanged();
    void startTimeChanged();
    void endTimeChanged();
    void allDayChanged();
    void recurChanged();
    void reminderChanged();
    void uniqueIdChanged();
    void colorChanged();
    void calendarUidChanged();
    void locationChanged();
    void recurEndDateChanged();
    void hasRecurEndDateChanged();
    void secrecyChanged();
    void ownerStatusChanged();
    void rsvpChanged();
    void externalInvitationChanged();

private:
    CalendarManager *mManager;
    QString mUniqueId;
    KDateTime mRecurrenceId;
};

#endif // CALENDAREVENT_H
