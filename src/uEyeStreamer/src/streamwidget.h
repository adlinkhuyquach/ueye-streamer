#ifndef STREAMWIDGET_H
#define STREAMWIDGET_H

#include <QFrame>
#include <QPushButton>
#include <QEvent>

#include "camerastream.h"

class StreamWidget : public QFrame
{
    Q_OBJECT
public:
    StreamWidget(CameraStreamPtr cameraStream, QWidget *parent = 0);
    CameraStreamPtr GetCameraStream();

protected:
    void enterEvent(QEvent *event);
    void leaveEvent(QEvent *event);

protected slots:
    void onCaptureStartStop();
    void onRemoveSession();

signals:
    void sessionRemoved(StreamWidget *widget);

private:
    CameraStreamPtr m_CameraStream;

    QPushButton* buttonCaptureStop;
    QPushButton* buttonRemove;
};

#endif // STREAMWIDGET_H
