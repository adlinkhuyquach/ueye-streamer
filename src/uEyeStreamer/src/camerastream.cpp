#include "camerastream.h"

CameraStream::CameraStream()
{
    streamer.moveToThread(&workerThread);
    workerThread.start();

    QObject::connect(&camera, SIGNAL(newFrameData(char*)), &streamer, SLOT(submitFrame(char*)));
}

CameraStream::~CameraStream()
{
    Stop();
}

Camera& CameraStream::GetCamera()
{
    return camera;
}

Streamer& CameraStream::GetStreamer()
{
    return streamer;
}

void CameraStream::Stop()
{
    QObject::disconnect(&camera, SIGNAL(newFrameData(char*)), &streamer, SLOT(submitFrame(char*)));

    workerThread.quit();
    workerThread.wait();
}
