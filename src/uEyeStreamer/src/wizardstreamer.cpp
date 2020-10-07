#include "wizardstreamer.h"

WizardStreamer::WizardStreamer(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizard(parent), pageIntro(0), pageCameraSelect(0), pageCamera(0), pageStream(0), pageFinish(0), m_CameraStream(cameraStream)
{
    initWidget();
}

void WizardStreamer::initWidget()
{
    setWindowTitle(tr("Stream creation wizard"));
    setWindowIcon(QIcon(":/res/icons/captureStart.png"));

    if (!m_CameraStream->GetStreamer().IsInitialzed())
    {
        pageIntro = new WizardPageIntro(m_CameraStream, this);
        pageIntro->setCommitPage(true);
        addPage(pageIntro);
    }

    pageCameraSelect = new WizardPageCameraSelect(m_CameraStream, this);
    pageCameraSelect->setCommitPage(true);
    addPage(pageCameraSelect);

    pageCamera = new WizardPageCamera(m_CameraStream, this);
    pageCamera->setCommitPage(true);
    addPage(pageCamera);

    pageStream = new WizardPageStream(m_CameraStream, this);
    pageStream->setCommitPage(true);
    addPage(pageStream);

    pageFinish = new WizardPageFinish(m_CameraStream, this);
    pageFinish->setCommitPage(true);
    addPage(pageFinish);
}

