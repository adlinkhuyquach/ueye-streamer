#ifndef CAMERALISTMODEL_H
#define CAMERALISTMODEL_H

#include <QAbstractTableModel>
#include <QStringList>
#include <QVector>
#include <QThread>

#include "ueye.h"

class EventThread : public QThread
{
    Q_OBJECT
public:
    EventThread(int eventID, QObject* parent = 0) :
        QThread(parent), m_EventID(eventID)
    {
#if defined Q_OS_WIN32
        for (int i = 0; i < 2; ++i)
        {
            m_EventHandle[i] = 0;
        }
#elif defined Q_OS_LINUX
        m_bStop = false;
#endif
    }

    ~EventThread()
    {
        stop();
    }

public slots:
    void run()
    {
#if defined Q_OS_WIN32
        for (int i = 0; i < 2; ++i)
        {
            m_EventHandle[i] = CreateEvent(0, FALSE, FALSE, 0);
        }
        is_InitEvent(0, m_EventHandle[1], m_EventID);

#elif defined Q_OS_LINUX
        m_bStop = false;
#endif

        is_EnableEvent(0, m_EventID);

        for(;;)
        {
#if defined Q_OS_WIN32
            DWORD waitObject = WaitForMultipleObjects(2, m_EventHandle, FALSE, INFINITE);

            if (waitObject == 1)
            {
                emit eventSignaled();
                continue;
            }
#elif defined Q_OS_LINUX
            int ret = is_WaitEvent(0, m_EventID, 1000);
            if (IS_SUCCESS == ret)
            {
                emit eventSignaled();
            }

            if (!m_bStop)
            {
                continue;
            }
#endif

            break;
        }

        is_DisableEvent(0, m_EventID);
        is_ExitEvent(0, m_EventID);

#if defined Q_OS_WIN32
        for (int i = 0; i < 2; ++i)
        {
            CloseHandle(m_EventHandle[i]);
            m_EventHandle[i] = 0;
        }
#endif
    }

    void stop()
    {
#if defined Q_OS_WIN32
        if (0 != m_EventHandle[0])
        {
            SetEvent(m_EventHandle[0]);
        }
#elif defined Q_OS_LINUX
        m_bStop = true;
#endif
    }

signals:
    void eventSignaled();

private:
    int m_EventID;
#if defined Q_OS_WIN32
    HANDLE m_EventHandle[2];
#elif defined Q_OS_LINUX
    bool m_bStop;
#endif
};

class CameraListModel : public QAbstractTableModel
{
    Q_OBJECT
public:
    enum { Available = 0, CamID, DevID, Model, SerNr };

    typedef QVector<UEYE_CAMERA_INFO> CameraList;
    explicit CameraListModel(QObject *parent = 0);
    ~CameraListModel();

protected:
    int rowCount(const QModelIndex &parent = QModelIndex()) const;
    int columnCount(const QModelIndex &parent = QModelIndex()) const;

    QVariant data(const QModelIndex &index, int role) const;
    QVariant headerData(int section, Qt::Orientation orientation,
                            int role = Qt::DisplayRole) const;

private:
    CameraList GetCameraList();

    QStringList m_HeaderHorizontal;
    CameraList m_CameraList;

    EventThread* m_ThreadDeviceConnected;
    EventThread* m_ThreadDeviceRemoved;

private slots:
    void onDeviceRemoved();
    void onDeviceConnected();
};



#endif // CAMERALISTMODEL_H
