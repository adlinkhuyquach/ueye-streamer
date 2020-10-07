#include "wizardpageintro.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QFormLayout>

WizardPageIntro::WizardPageIntro(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizardPage(parent), m_CameraStream(cameraStream)
{
    initWidget();
}

void WizardPageIntro::initWidget()
{
    setTitle(tr("Server configuration"));
    setSubTitle(tr("Please select the port to use for the rtsp server."));

    QFormLayout *mainLayout = new QFormLayout(this);
    setLayout(mainLayout);

    spinBoxRtspPort = new QSpinBox(this);
    spinBoxRtspPort->setRange(0, 1 << 16);
    spinBoxRtspPort->setValue(8554);
    mainLayout->addRow("Rtsp port:", spinBoxRtspPort);

    spinBoxRtspPort->setEnabled(!Streamer::IsInitialzed());
}

bool WizardPageIntro::validatePage()
{
    if (!Streamer::IsInitialzed())
    {
        try
        {
            m_CameraStream->GetStreamer().init(spinBoxRtspPort->value());
        }
        catch(std::exception& e)
        {
            QMessageBox::warning(this, "Information", e.what());
            return false;
        }
    }

    return true;
}
