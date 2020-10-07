#ifndef CAMERASTREAM_H
#define CAMERASTREAM_H

#include <QThread>
#include <QSharedPointer>

#include "camera.h"
#include "streamer.h"

class CameraStream
{
public:
    CameraStream();
    ~CameraStream();

    Camera& GetCamera();
    Streamer& GetStreamer();

    void Stop();

private:
    Camera camera;
    Streamer streamer;

    QThread workerThread;
};

typedef QSharedPointer<CameraStream> CameraStreamPtr;

#endif // CAMERASTREAM_H
