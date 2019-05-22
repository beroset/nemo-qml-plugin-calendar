/*
 * Copyright (C) 2014 Jolla Ltd.
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

#ifndef CALENDARWORKER_H
#define CALENDARWORKER_H

#include "calendardata.h"

#include <QObject>
#include <QHash>

// mkcal
#include <extendedstorage.h>

// libaccounts-qt
namespace Accounts { class Manager; }

class NemoCalendarInvitationQuery;

class NemoCalendarWorker : public QObject, public mKCal::ExtendedStorageObserver
{
    Q_OBJECT
    
public:
    NemoCalendarWorker();
    ~NemoCalendarWorker();

    /* mKCal::ExtendedStorageObserver */
    void storageModified(mKCal::ExtendedStorage *storage, const QString &info);
    void storageProgress(mKCal::ExtendedStorage *storage, const QString &info);
    void storageFinished(mKCal::ExtendedStorage *storage, bool error, const QString &info);

public slots:
    void init();
    void save();

    void saveEvent(const NemoCalendarData::Event &eventData);
    void replaceOccurrence(const NemoCalendarData::Event &eventData, const QDateTime &startTime);
    void deleteEvent(const QString &uid, const KDateTime &recurrenceId, const QDateTime &dateTime);
    void deleteAll(const QString &uid);
    bool sendResponse(const NemoCalendarData::Event &eventData, const NemoCalendarEvent::Response response);
    QString convertEventToVCalendar(const QString &uid, const QString &prodId) const;

    QList<NemoCalendarData::Notebook> notebooks() const;
    void setNotebookColor(const QString &notebookUid, const QString &color);
    void setExcludedNotebooks(const QStringList &list);
    void excludeNotebook(const QString &notebookUid, bool exclude);
    void setDefaultNotebook(const QString &notebookUid);

    void loadData(const QList<NemoCalendarData::Range> &ranges,
                  const QStringList &uidList, bool reset);

    NemoCalendarData::EventOccurrence getNextOccurrence(const QString &uid, const KDateTime &recurrenceId,
                                                        const QDateTime &startTime) const;
    QList<NemoCalendarData::Attendee> getEventAttendees(const QString &uid, const KDateTime &recurrenceId);

    void findMatchingEvent(const QString &invitationFile);

signals:
    void storageModifiedSignal(QString info);

    void eventNotebookChanged(QString oldEventUid, QString newEventUid, QString notebookUid);

    void excludedNotebooksChanged(QStringList excludedNotebooks);
    void notebookColorChanged(NemoCalendarData::Notebook notebook);
    void notebooksChanged(QList<NemoCalendarData::Notebook> notebooks);

    void dataLoaded(QList<NemoCalendarData::Range> ranges,
                    QStringList uidList,
                    QMultiHash<QString, NemoCalendarData::Event> events,
                    QHash<QString, NemoCalendarData::EventOccurrence> occurrences,
                    QHash<QDate, QStringList> dailyOccurrences,
                    bool reset);

    void occurrenceExceptionFailed(NemoCalendarData::Event eventData, QDateTime startTime);
    void occurrenceExceptionCreated(NemoCalendarData::Event eventData, QDateTime startTime, KDateTime newRecurrenceId);

    void findMatchingEventFinished(QString invitationFile,
                                   NemoCalendarData::Event eventData);

private:
    void setEventData(KCalCore::Event::Ptr &event, const NemoCalendarData::Event &eventData);
    void loadNotebooks();
    QStringList excludedNotebooks() const;
    bool saveExcludeNotebook(const QString &notebookUid, bool exclude);

    bool setRecurrence(KCalCore::Event::Ptr &event, NemoCalendarEvent::Recur recur);
    bool setReminder(KCalCore::Event::Ptr &event, int reminderSeconds);
    bool needSendCancellation(KCalCore::Event::Ptr &event) const;
    QString getNotebookAddress(const KCalCore::Event::Ptr &event) const;

    NemoCalendarData::Event createEventStruct(const KCalCore::Event::Ptr &event) const;
    QHash<QString, NemoCalendarData::EventOccurrence> eventOccurrences(const QList<NemoCalendarData::Range> &ranges) const;
    QHash<QDate, QStringList> dailyEventOccurrences(const QList<NemoCalendarData::Range> &ranges,
                                                    const QMultiHash<QString, KDateTime> &allDay,
                                                    const QList<NemoCalendarData::EventOccurrence> &occurrences);

    Accounts::Manager *mAccountManager;

    mKCal::ExtendedCalendar::Ptr mCalendar;
    mKCal::ExtendedStorage::Ptr mStorage;

    // mDeletedEvents and mExceptionEvents are used to make sure
    // that we are sending a cancellation email for events only
    // when user actually saved (so truly deleted) changes by calling of save()
    QStringList mDeletedEvents;
    QList<QPair<QString, QDateTime>> mExceptionEvents;

    QHash<QString, NemoCalendarData::Notebook> mNotebooks;

    // Tracks which events have been already passed to manager. Maps Uid -> RecurrenceId
    QMultiHash<QString, KDateTime> mSentEvents;
};

#endif // CALENDARWORKER_H
