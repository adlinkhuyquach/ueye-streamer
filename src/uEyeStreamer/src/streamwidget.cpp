#include "streamwidget.h"

#include <QMessageBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QDebug>

StreamWidget::StreamWidget(CameraStreamPtr cameraStream, QWidget *parent) :
    QFrame(parent), m_CameraStream(cameraStream), buttonCaptureStop(0), buttonRemove(0)
{
    setFrameShape(QFrame::Panel);
    setFrameShadow(QFrame::Raised);
    setLineWidth(2);
    setFixedHeight(90);

    QHBoxLayout* mainLayout = new QHBoxLayout(this);
    setLayout(mainLayout);

    QString stringInfo;
    stringInfo = "<b>" + cameraStream->GetStreamer().GetName() + "</b>" + "<br>" + cameraStream->GetStreamer().GetDescription()
            + "<br>" + cameraStream->GetStreamer().GetUrl();

    QLabel* labelStreamUrl = new QLabel(stringInfo, this);
    labelStreamUrl->setTextFormat(Qt::RichText);
    labelStreamUrl->setTextInteractionFlags(Qt::TextBrowserInteraction);
    labelStreamUrl->setOpenExternalLinks(true);
    labelStreamUrl->setWordWrap(true);
    mainLayout->addWidget(labelStreamUrl);

    buttonCaptureStop = new QPushButton(QIcon(":/res/icons/captureStart.png"), "", this);
    buttonCaptureStop->setFlat(true);
    buttonCaptureStop->setFixedSize(48, 48);
    mainLayout->addWidget(buttonCaptureStop);

    connect(buttonCaptureStop, SIGNAL(released()), this, SLOT(onCaptureStartStop()));

    buttonRemove = new QPushButton(QIcon(":/res/icons/streamRemove.png"), "", this);
    buttonRemove->setFlat(true);
    buttonRemove->setFixedSize(48, 48);
    mainLayout->addWidget(buttonRemove);

    connect(buttonRemove, SIGNAL(released()), this, SLOT(onRemoveSession()));
}

void StreamWidget::onCaptureStartStop()
{
    try
    {
        if (m_CameraStream->GetCamera().IsCapturing())
        {
            m_CameraStream->GetCamera().StopVideo();
            buttonCaptureStop->setIcon(QIcon(":/res/icons/captureStart.png"));
        }
        else
        {
            m_CameraStream->GetCamera().CaptureVideo();
            buttonCaptureStop->setIcon(QIcon(":/res/icons/captureStop.png"));
        }
    }
    catch(std::exception& e)
    {
        QMessageBox::warning(this, "Information", e.what());
    }
}

void StreamWidget::onRemoveSession()
{
    try
    {
        m_CameraStream->Stop();

        m_CameraStream->GetCamera().StopVideo();
        m_CameraStream->GetStreamer().removeSession();
    }
    catch(std::exception& e)
    {
        qWarning() << e.what();
    }

    emit sessionRemoved(this);
}

CameraStreamPtr StreamWidget::GetCameraStream()
{
    return m_CameraStream;
}

void StreamWidget::enterEvent(QEvent*)
{
    setLineWidth(1);
}

void StreamWidget::leaveEvent(QEvent*)
{
    setLineWidth(2);
}
