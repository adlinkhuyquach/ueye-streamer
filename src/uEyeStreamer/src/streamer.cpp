#include "streamer.h"
#include <sstream>
#include <map>
#include <stdexcept>

namespace {
void throwOnError(const int errorCode, const QString& errorMsg)
{   
    if (IS_STREAM_SUCCESS != errorCode)
    {
        static char msg[256];
        strcpy(msg, errorMsg.toLocal8Bit());

        throw std::runtime_error(msg);
    }
}

static std::map<int, QString> ErrorMap = {
    { IS_STREAM_SUCCESS, ""},
    { IS_STREAM_NO_SUCCESS, "Unknown error occured!"},
    { IS_STREAM_ALREADY_INITIALIZED, "Server is already initialized!"},
    { IS_STREAM_INVALID_PARAMETER, "Invalid parameter!"},
    { IS_STREAM_NAME_IN_USE, "Session name already in use!"},
    { IS_STREAM_INVALID_ID, "Invalid given session or stream identifier!"},
    { IS_STREAM_NO_MEMORY, "Internal memory allocation failed!"},
    { IS_STREAM_ENCODER_ERROR, "Internal encoder error!"}
};
}

bool Streamer::m_Initialized = false;

Streamer::Streamer(QObject *parent) : QObject(parent)
{
    m_SessionID = 0;
}

Streamer::~Streamer()
{

}

void Streamer::init(int port)
{
    int ret = is_Stream(IS_STREAM_INIT, &port, sizeof(port));
    throwOnError(ret, QString("Initializing streaming library failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

    m_Initialized = true;
}

void Streamer::exit()
{
    int ret = is_Stream(IS_STREAM_EXIT, 0, 0);
    throwOnError(ret, QString("Deinitializing streaming library failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

    m_Initialized = false;
}

bool Streamer::IsInitialzed()
{
    return m_Initialized;
}

void Streamer::addSession(const QString& streamName, const QString& streamDescription, const QString& streamInfoString)
{
    IS_SESSION_INFO sessionInfo;
    memset(&sessionInfo, 0, sizeof(sessionInfo));

    sessionInfo.cbSizeOfStruct = sizeof(sessionInfo);

    strncpy(sessionInfo.strName, streamName.toLocal8Bit(), std::max(streamName.size(), 20));
    strncpy(sessionInfo.strDescription, streamDescription.toLocal8Bit(), std::max(streamDescription.size(), 20));
    strncpy(sessionInfo.strInfo, streamInfoString.toLocal8Bit(), std::max(streamInfoString.size(), 20));

    int ret = is_Stream(IS_STREAM_ADD_SESSION, &sessionInfo, sizeof(sessionInfo));
    throwOnError(ret, QString("Adding the stream session failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

    m_SessionID = sessionInfo.idSession;
}

void Streamer::removeSession()
{
    if (m_SessionID != 0)
    {
        int ret = is_Stream(IS_STREAM_REMOVE_SESSION, m_SessionID, sizeof(m_SessionID));
        throwOnError(ret, QString("Removing the stream session failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

        m_SessionID = 0;
    }
}

void Streamer::addStream(const QSize& dstFrame, const QSize& srcFrame, const IS_STREAM_PIXELFMT srcPixelFmt, const IS_STREAM_CODEC dstCodec, const quint32 bitrate)
{
    IS_STREAM_VIDEO_CONFIGURATION streamVideo;
    memset(&streamVideo, 0, sizeof(streamVideo));

    streamVideo.cbSizeOfStruct = sizeof(streamVideo);

    /* destination frame size */
    streamVideo.dstHeight = dstFrame.height();
    streamVideo.dstWidth = dstFrame.width();

    /* source frame size */
    streamVideo.srcHeight = srcFrame.height();
    streamVideo.srcWidth = srcFrame.width();

    /* source pixel format */
    streamVideo.srcPixelformat = srcPixelFmt;

    /* destination codec */
    streamVideo.dstCodec = dstCodec;

    /* bitrate */
    streamVideo.bitrate = bitrate;

    IS_STREAM_INFO streamInfo;
    memset(&streamInfo, 0, sizeof(streamInfo));

    streamInfo.cbSizeOfStruct = sizeof(streamInfo);

    streamInfo.idSession = m_SessionID;
    streamInfo.streamType = IS_STREAM_TYPE_VIDEO;

    streamInfo.pStreamData = &streamVideo;

    int ret = is_Stream(IS_STREAM_ADD_STREAM, &streamInfo, sizeof(streamInfo));
    throwOnError(ret, QString("Creating the video stream failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

    m_StreamID = streamInfo.idStream;
}

void Streamer::submitFrame(char *frameBuffer)
{
    IS_STREAM_PAYLOAD_DATA frameData;
    memset(&frameData, 0, sizeof(frameData));

    frameData.idSession = m_SessionID;
    frameData.idStream = m_StreamID;
    frameData.cbSizeOfStruct = sizeof(frameData);
    frameData.pData = reinterpret_cast<unsigned char*>(frameBuffer);

    int ret = is_Stream(IS_STREAM_SUBMIT_DATA, &frameData, sizeof(frameData));
    /* discard error */
   //throwOnError(ret, QString("Submiting the frame failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));
}

IS_SESSION_INFO Streamer::GetInfo()
{
    IS_SESSION_INFO streamInfo;
    memset(&streamInfo, 0, sizeof(streamInfo));

    streamInfo.cbSizeOfStruct = sizeof(IS_STREAM_INFO);
    streamInfo.idSession = m_SessionID;

    int ret = is_Stream(IS_STREAM_GET_SESSION_INFO, &streamInfo, sizeof(streamInfo));
    throwOnError(ret, QString("Receiving the stream information failed with error code: %1\n%2").arg(ret).arg(ErrorMap[ret]));

    return streamInfo;
}

QString Streamer::GetUrl()
{
    return GetInfo().strURL;
}

QString Streamer::GetName()
{
    return GetInfo().strName;
}

QString Streamer::GetDescription()
{
    return GetInfo().strDescription;
}

QString Streamer::GetInformation()
{
    return GetInfo().strInfo;
}
