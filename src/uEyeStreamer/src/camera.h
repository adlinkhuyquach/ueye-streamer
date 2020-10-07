#ifndef CAMERA_H
#define CAMERA_H

#include <QObject>
#include <QRect>
#include <QList>
#include <QThread>
#include <QVector>

#include "ueye.h"

struct CameraInfo
{
    int cameraID;
    int deviceID;
    int sensorID;
    bool inUse;
    QString serialNr;
    QString model;
    int status;
};

int GetCameraCount();
QList<CameraInfo> GetCameraList();

class WaitForFrameThread;

class Camera : public QObject
{
    Q_OBJECT
public:
    typedef QList<IMAGE_FORMAT_INFO> ImageFormatList;
    enum ColorMode {
            Raw8 = 11, Raw10 = 33, Raw12 = 27, Raw16 = 29,
            Mono8 = 6, Mono10 = 34, Mono12 = 26, Mono16 = 28,
            BGR8Packed = 1, BGRA8Packed = 0, BGR10Packed = 25,
            RGB8Packed = 1 | 0x80, RGBA8Packed = 0 | 0x80
        };

    struct ImageBuffer
    {
        char *memBuffer;
        int memID;
    };

    template<typename T>
    struct Range
    {
        T min;
        T max;
        T inc;
    };

    explicit Camera(QObject *parent = 0);
    ~Camera();

    void Init(const HIDS deviceID);

    void SetColorMode(const ColorMode colorMode);
    ColorMode GetColorMode();

    void CaptureVideo(bool wait = false);
    void FreezeVideo(bool wait = false);
    void StopVideo(bool force = false);

    int GetHandle() const;

    void SetAoi(const int posX, const int posY, const int sizeX, const int sizeY);
    void SetAoi(const QRect& rect);
    QRect GetAoi() const;
    bool IsAbsoluteAoi() const;

    double GetFramerate() const;
    Range<double> GetFramerateRange() const;
    void SetFramerate(const double value);

    void AllocBuffer();

    ImageFormatList GetImageFormatList();
    void SetImageFormat(const int id);

    void LoadParameter(const std::string& file);

    bool IsCapturing();

protected slots:
    void onFrameReceived(char *memBuffer, int memID);
    void onFrameFailed(char *memBuffer, int memID);

signals:
    void newFrameData(char *frameData);

private:
    HIDS m_Camera;
    bool m_IsCapturing;
    WaitForFrameThread *m_ThreadWaitForFrame;

    QVector<ImageBuffer> m_ImageBuffers;
};

class WaitForFrameThread : public QThread
{
    Q_OBJECT
public:
    WaitForFrameThread(Camera& camera) :
        m_Camera(camera) {}

public slots:
    void run()
    {
        m_Stop = false;
        while(!m_Stop)
        {
            char *memBuffer = 0;
            int memID = 0;
            int camRet = is_WaitForNextImage(m_Camera.GetHandle(), 500, &memBuffer, &memID);
            if (!m_Stop)
            {
                if (IS_SUCCESS == camRet)
                {
                    frameReceived(memBuffer, memID);
                }
                else if (IS_CAPTURE_STATUS == camRet)
                {
                    frameFailed(memBuffer, memID);
                }
                else
                {
                    /* silently discard */
                }
            }
        }
    }

    void stop()
    {
        m_Stop = true;
    }

signals:
    void frameReceived(char *memBuffer, int memID);
    void frameFailed(char *memBuffer, int memID);

private:
    const Camera& m_Camera;
    bool m_Stop;
};

#endif // CAMERA_H
