#include "camera.h"
#include <stdexcept>

namespace
{
void throwOnError(const int errorCode, const QString& errorMsg)
{
    if (IS_SUCCESS != errorCode)
    {
        static char msg[256];
        strcpy(msg, errorMsg.toLocal8Bit());

        throw std::runtime_error(msg);
    }
}
}

int GetCameraCount()
{
    int camRet = 0;
    int cameraCount = 0;
    camRet = is_GetNumberOfCameras(&cameraCount);
    throwOnError(camRet, QString("Receiving the number of cameras failed with error code: %1").arg(camRet));

    return cameraCount;
}

QList<CameraInfo> GetCameraList()
{
    int camRet = 0;
    int cameraCount = 0;

    cameraCount = GetCameraCount();
    QList<CameraInfo> cameraList;

    if (cameraCount > 0)
    {
        UEYE_CAMERA_LIST* pucl;

        pucl = (UEYE_CAMERA_LIST*) new BYTE[sizeof(ULONG) + cameraCount * sizeof(UEYE_CAMERA_INFO)];
        pucl->dwCount = cameraCount;

        camRet = is_GetCameraList(pucl);
        throwOnError(camRet, QString("Receiving the camera list failed with error code: %1").arg(camRet));

        for (int i = 0; i < static_cast<int>(pucl->dwCount); ++i)
        {
            CameraInfo camInfo;

            camInfo.cameraID = pucl->uci[i].dwCameraID;
            camInfo.deviceID = pucl->uci[i].dwDeviceID;
            camInfo.inUse = pucl->uci[i].dwInUse == 0 ? false : true;
            camInfo.sensorID = pucl->uci[i].dwSensorID;
            camInfo.model = pucl->uci[i].Model;
            camInfo.serialNr = pucl->uci[i].SerNo;
            camInfo.status = pucl->uci[i].dwStatus;

            cameraList.push_back(camInfo);
        }

        delete[] pucl;
    }

    return cameraList;
}

int GetBitsPerPixel(Camera::ColorMode colorMode)
{
    int bpp = 0;

    switch(colorMode)
    {
    case Camera::Raw8:
    case Camera::Mono8:
        bpp = 8;
        break;

    case Camera::BGR8Packed:
    case Camera::RGB8Packed:
        bpp = 24;
        break;

    case Camera::BGRA8Packed:
    case Camera::RGBA8Packed:
        bpp = 32;
        break;

    default:
        throwOnError(IS_INVALID_PARAMETER, "GetBitsPerPixel");
    }

    return bpp;
}

Camera::Camera(QObject *parent) : QObject(parent), m_Camera(0)
{
    m_IsCapturing = false;
}

Camera::~Camera()
{
    if (0 != m_Camera)
    {
        if (m_IsCapturing)
        {
            StopVideo(true);
        }

        if (m_ThreadWaitForFrame)
        {
            disconnect(m_ThreadWaitForFrame, SIGNAL(frameReceived(char*,int)), this, SLOT(onFrameReceived(char*,int)));
            disconnect(m_ThreadWaitForFrame, SIGNAL(frameFailed(char*,int)), this, SLOT(onFrameFailed(char*,int)));

            m_ThreadWaitForFrame->stop();
            m_ThreadWaitForFrame->wait();

            delete m_ThreadWaitForFrame;
        }

        foreach (const ImageBuffer &imageBuffer, m_ImageBuffers)
        {
            is_FreeImageMem(GetHandle(), imageBuffer.memBuffer, imageBuffer.memID);
        }

        is_ExitCamera(m_Camera);
        m_Camera = 0;
    }
}

int Camera::GetHandle() const
{
    return m_Camera;
}

void Camera::Init(const HIDS deviceID)
{
    int camRet = 0;

    m_Camera = deviceID | IS_USE_DEVICE_ID;
    camRet = is_InitCamera(&m_Camera, 0);
    throwOnError(camRet, QString("Initializing the camera failed with error code: %1").arg(camRet));

    m_ThreadWaitForFrame = new WaitForFrameThread(*this);

    connect(m_ThreadWaitForFrame, SIGNAL(frameReceived(char*,int)), this, SLOT(onFrameReceived(char*,int)));
    connect(m_ThreadWaitForFrame, SIGNAL(frameFailed(char*,int)), this, SLOT(onFrameFailed(char*,int)));
}

void Camera::AllocBuffer()
{
    int camRet = 0;

    QRect rectAoi = GetAoi();
    int bpp = GetBitsPerPixel(GetColorMode());

    for (int i = 0; i < 3; ++i)
    {
        ImageBuffer imageBuffer;

        camRet = is_AllocImageMem(GetHandle(), rectAoi.width(), rectAoi.height(), bpp, &imageBuffer.memBuffer, &imageBuffer.memID);
        throwOnError(camRet, QString("Image memory allocation failed with error code: %1").arg(camRet));

        camRet = is_AddToSequence(GetHandle(), imageBuffer.memBuffer, imageBuffer.memID);
        throwOnError(camRet, QString("Adding image memory to sequence failed with error code: %1").arg(camRet));

        m_ImageBuffers.push_back(imageBuffer);
    }

    camRet = is_InitImageQueue(GetHandle(), 0);
    throwOnError(camRet, QString("Initializing the image queue failed with error code: %1").arg(camRet));

    m_ThreadWaitForFrame->start();
}

void Camera::SetColorMode(const ColorMode colorMode)
{
    int camRet = 0;
    camRet = is_SetColorMode(GetHandle(), static_cast<int>(colorMode));
    throwOnError(camRet, QString("Setting the camera color mode failed with error code: %1").arg(camRet));
}

Camera::ColorMode Camera::GetColorMode()
{
    ColorMode colorMode;
    colorMode = static_cast<ColorMode>(is_SetColorMode(GetHandle(), IS_GET_COLOR_MODE));

    return colorMode;
}

void Camera::onFrameReceived(char *memBuffer, int memID)
{
    emit newFrameData(memBuffer);

    is_UnlockSeqBuf(GetHandle(), memID, memBuffer);
}

void Camera::onFrameFailed(char *memBuffer, int memID)
{
    is_UnlockSeqBuf(GetHandle(), memID, memBuffer);
}

void Camera::CaptureVideo(bool wait)
{
    int camRet = 0;
    camRet = is_CaptureVideo(GetHandle(), wait ? IS_WAIT : IS_DONT_WAIT);
    throwOnError(camRet, QString("Starting the frame acquisition failed with error code: %1").arg(camRet));

    m_IsCapturing = true;
}

void Camera::FreezeVideo(bool wait)
{
    m_IsCapturing = false;

    int camRet = 0;
    camRet = is_FreezeVideo(GetHandle(), wait ? IS_WAIT : IS_DONT_WAIT);
    throwOnError(camRet, QString("Frame acquisition failed with error code: %1").arg(camRet));
}

void Camera::StopVideo(bool force)
{
    m_IsCapturing = false;

    int camRet = 0;
    camRet = is_StopLiveVideo(GetHandle(), force ? IS_FORCE_VIDEO_STOP : 0);
    throwOnError(camRet, QString("Stopping the frame acquisition failed with error code: %1").arg(camRet));
}

bool Camera::IsCapturing()
{
    return m_IsCapturing;
}

void Camera::SetAoi(const int posX, const int posY, const int sizeX, const int sizeY)
{
    IS_RECT rect;
    rect.s32Height = sizeY;
    rect.s32Width = sizeX;
    rect.s32X = posX;
    rect.s32Y = posY;

    int camRet = 0;
    camRet = is_AOI(GetHandle(), IS_AOI_IMAGE_SET_AOI, (void*)&rect, sizeof(rect));
    throwOnError(camRet, QString("Setting the area of interest failed with error code: %1").arg(camRet));
}

void Camera::SetAoi(const QRect& rect)
{
    Camera::SetAoi(rect.x(), rect.y(), rect.width(), rect.height());
}

QRect Camera::GetAoi() const
{
    IS_RECT rect;

    int camRet = 0;
    camRet = is_AOI(GetHandle(), IS_AOI_IMAGE_GET_AOI, (void*)&rect, sizeof(rect));
    throwOnError(camRet, QString("Receiving the area of interest failed with error code: %1").arg(camRet));

    return QRect(rect.s32X, rect.s32Y, rect.s32Width, rect.s32Height);
}

bool Camera::IsAbsoluteAoi() const
{
    int camRet = 0;
    int enabled = 0;
    camRet = is_AOI(GetHandle(), IS_AOI_IMAGE_GET_POS_Y_ABS, (void*)&enabled, sizeof(enabled));
    throwOnError(camRet, QString("Receiving the the absolute aoi position failed with error code: %1").arg(camRet));

    if (!enabled)
    {
        camRet = is_AOI(GetHandle(), IS_AOI_IMAGE_GET_POS_X_ABS, (void*)&enabled, sizeof(enabled));
        throwOnError(camRet, QString("Receiving the the absolute aoi position failed with error code: %1").arg(camRet));
    }

    return enabled;
}

Camera::ImageFormatList Camera::GetImageFormatList()
{
    int formatCount = 0;
    int camRet = 0;

    camRet = is_ImageFormat(GetHandle(), IMGFRMT_CMD_GET_NUM_ENTRIES, &formatCount, sizeof(formatCount));
    throwOnError(camRet, QString("Receiving the number of image formats failed with error code: %1").arg(camRet));

    const int formatListSize = sizeof(IMAGE_FORMAT_LIST) + ((formatCount - 1) * sizeof(IMAGE_FORMAT_INFO));
    char *buffer = new char[formatListSize];

    IMAGE_FORMAT_LIST* pformatList = (IMAGE_FORMAT_LIST*)buffer;
    pformatList->nSizeOfListEntry = sizeof(IMAGE_FORMAT_INFO);
    pformatList->nNumListElements = formatCount;
    camRet = is_ImageFormat(GetHandle(), IMGFRMT_CMD_GET_LIST, pformatList, formatListSize);
    throwOnError(camRet, QString("Receiving the image format list failed with error code: %1").arg(camRet));

    ImageFormatList imageFormatList;
    for (unsigned int i = 0; i < pformatList->nNumListElements; ++i)
    {
        imageFormatList.push_back(pformatList->FormatInfo[i]);
    }

    delete [] buffer;

    return imageFormatList;
}

void Camera::SetImageFormat(const int id)
{
    int camRet = 0;

    int formatID = id;
    camRet = is_ImageFormat(GetHandle(), IMGFRMT_CMD_SET_FORMAT, &formatID, sizeof(formatID));
    throwOnError(camRet, QString("Setting the image format failed with error code: %1").arg(camRet));
}

double Camera::GetFramerate() const
{
    double fps = 0.;
    int camRet = 0;
    camRet = is_SetFrameRate(GetHandle(), IS_GET_FRAMERATE, &fps);
    throwOnError(camRet, QString("Receiving the frame rate failed with error code: %1").arg(camRet));

    return fps;
}

Camera::Range<double> Camera::GetFramerateRange() const
{
    int camRet = 0;
    double min = 0, max = 0, inc = 0;

    camRet = is_GetFrameTimeRange(GetHandle(), &min, &max, &inc);
    throwOnError(camRet, QString("Receiving the frame rate range failed with error code: %1").arg(camRet));

    Range<double> range;
    range.inc = inc;
    range.max = 1/min;
    range.min = 1/max;

    return range;
}

void Camera::SetFramerate(const double value)
{
    int camRet = 0;
    double fps = value;
    double fpsNew = 0.;
    camRet = is_SetFrameRate(GetHandle(), fps, &fpsNew);
    throwOnError(camRet, QString("Setting the frame rate failed with error code: %1").arg(camRet));
}

void Camera::LoadParameter(const std::string& file)
{
    int camRet = 0;
    std::wstring wStr(file.begin(), file.end());
    camRet = is_ParameterSet(GetHandle(), IS_PARAMETERSET_CMD_LOAD_FILE, (void*)wStr.c_str(), 0);
    throwOnError(camRet, QString("Loading parameter file failed with error code: %1").arg(camRet));
}
