#ifndef STREAMER_H
#define STREAMER_H

#include <QString>
#include <QObject>
#include <QSize>

#include <ueye_stream.h>

class Streamer : public QObject
{
    Q_OBJECT

public:
    Streamer(QObject *parent = 0);
    ~Streamer();

    void addSession(const QString& streamName, const QString& streamDescription, const QString& streamInfoString);
    void removeSession();
    void addStream(const QSize& dstFrame, const QSize& srcFrame, const IS_STREAM_PIXELFMT srcPixelFmt, const IS_STREAM_CODEC dstCodec, const quint32 bitrate);

    static void init(int port);
    static void exit();

    static bool IsInitialzed();

    QString GetUrl();
    QString GetName();
    QString GetDescription();
    QString GetInformation();

    IS_SESSION_INFO GetInfo();

public slots:
    void submitFrame(char *frameBuffer);

private:
    IS_STREAM_ID m_SessionID;
    IS_STREAM_ID m_StreamID;

    static bool m_Initialized;
};

#endif // STREAMER_H
