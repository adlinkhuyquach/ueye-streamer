#include "cameralistmodel.h"
#include <QtGlobal>
#include <QVariant>
#include <QIcon>

CameraListModel::CameraListModel(QObject *parent) :
    QAbstractTableModel(parent)
{
    m_CameraList = GetCameraList();
    m_HeaderHorizontal << tr("Avaiable") << tr("ID") << tr("Dev. ID") << tr("Model") << tr("Ser. Nr");

    m_ThreadDeviceConnected = new EventThread(IS_SET_EVENT_NEW_DEVICE);
    connect(m_ThreadDeviceConnected, SIGNAL(eventSignaled()), this, SLOT(onDeviceConnected()));
    m_ThreadDeviceConnected->start();

    m_ThreadDeviceRemoved = new EventThread(IS_SET_EVENT_REMOVAL);
    connect(m_ThreadDeviceRemoved, SIGNAL(eventSignaled()), this, SLOT(onDeviceRemoved()));
    m_ThreadDeviceRemoved->start();
}

CameraListModel::~CameraListModel()
{
    m_ThreadDeviceConnected->stop();
    m_ThreadDeviceRemoved->stop();
}

int CameraListModel::rowCount(const QModelIndex &/*parent*/) const
{
    return static_cast<int>(m_CameraList.size());
}

int CameraListModel::columnCount(const QModelIndex &/*parent*/) const
{
    return m_HeaderHorizontal.size();
}

QVariant CameraListModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    if (index.row() >= m_CameraList.size())
        return QVariant();

    if (role == Qt::DisplayRole)
    {
        switch(index.column())
        {
        case 0:
            return m_CameraList[index.row()].dwInUse ? "No" : "Yes";
        case 1:
            return static_cast<int>(m_CameraList[index.row()].dwCameraID);
        case 2:
            return static_cast<int>(m_CameraList[index.row()].dwDeviceID);
        case 3:
        {
            QString strModel = m_CameraList[index.row()].Model;
            strModel.replace("UI", "UI-");
            return strModel;
        }
        case 4:
            return (m_CameraList[index.row()].SerNo);
        }
    }
    else if (role == Qt::DecorationRole)
    {
        if (index.column() == 0)
        {
            return m_CameraList[index.row()].dwInUse ? QIcon(":img/cameraClose.png") : QIcon(":img/cameraOpen.png");
        }
    }

    return QVariant();
}

QVariant CameraListModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role != Qt::DisplayRole)
        return QVariant();

    if (orientation == Qt::Horizontal)
    {
        return m_HeaderHorizontal[section];
    }
    else
        return section;
}

CameraListModel::CameraList CameraListModel::GetCameraList()
{
    int nRet = 0;
    int cameraCount = 0;

    nRet = is_GetNumberOfCameras(&cameraCount);

    CameraListModel::CameraList cameraList;

    if (nRet == IS_SUCCESS && cameraCount > 0)
    {
        UEYE_CAMERA_LIST* pucl;

        pucl = (UEYE_CAMERA_LIST*) new BYTE [sizeof (ULONG) + cameraCount * sizeof (UEYE_CAMERA_INFO)];
        pucl->dwCount = cameraCount;

        nRet = is_GetCameraList(pucl);

        for (int i = 0; i < static_cast<int>(pucl->dwCount); ++i)
        {
            cameraList.push_back(pucl->uci[i]);
        }

        delete [] pucl;
    }

    return cameraList;
}

void CameraListModel::onDeviceRemoved()
{
#if QT_VERSION > QT_VERSION_CHECK(4, 6, 0)
    beginResetModel();
    m_CameraList = GetCameraList();
    endResetModel();
#endif
}

void CameraListModel::onDeviceConnected()
{
#if QT_VERSION > QT_VERSION_CHECK(4, 6, 0)
    beginResetModel();
    m_CameraList = GetCameraList();
    endResetModel();
#endif
}
