#ifndef CALENDARNOTEBOOKQUERY_H
#define CALENDARNOTEBOOKQUERY_H

#include <QObject>
#include "calendardata.h"

class CalendarNotebookQuery : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QString targetUid READ targetUid WRITE setTargetUid NOTIFY targetUidChanged)
    Q_PROPERTY(bool isValid READ isValid NOTIFY isValidChanged)
    Q_PROPERTY(QString name READ name NOTIFY nameChanged)
    Q_PROPERTY(QString description READ description NOTIFY descriptionChanged)
    Q_PROPERTY(QString color READ color NOTIFY colorChanged)
    Q_PROPERTY(int accountId READ accountId NOTIFY accountIdChanged)
    Q_PROPERTY(QUrl accountIcon READ accountIcon NOTIFY accountIconChanged)
    Q_PROPERTY(bool isDefault READ isDefault NOTIFY isDefaultChanged)
    Q_PROPERTY(bool localCalendar READ localCalendar NOTIFY localCalendarChanged)
    Q_PROPERTY(bool isReadOnly READ isReadOnly NOTIFY isReadOnlyChanged)

public:
    explicit CalendarNotebookQuery(QObject *parent = 0);
    ~CalendarNotebookQuery();

    QString targetUid() const;
    void setTargetUid(const QString &target);

    bool isValid() const;
    QString name() const;
    QString description() const;
    QString color() const;
    int accountId() const;
    QUrl accountIcon() const;
    bool isDefault() const;
    bool localCalendar() const;
    bool isReadOnly() const;

signals:
    void targetUidChanged();
    void isValidChanged();
    void nameChanged();
    void descriptionChanged();
    void colorChanged();
    void accountIdChanged();
    void accountIconChanged();
    void isDefaultChanged();
    void localCalendarChanged();
    void isReadOnlyChanged();

private slots:
    void updateData();

private:
    CalendarData::Notebook m_notebook;
    QString m_targetUid;
    bool m_isValid;
};

#endif
