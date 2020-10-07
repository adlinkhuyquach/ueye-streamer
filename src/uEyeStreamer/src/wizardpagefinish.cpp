#include "wizardpagefinish.h"
#include <QVBoxLayout>


WizardPageFinish::WizardPageFinish(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizardPage(parent), m_CameraStream(cameraStream)
{

}

void WizardPageFinish::initializePage()
{
   setSubTitle(tr("Your stream has been successfuly created!\n\nYour stream could be accessed at ") + m_CameraStream->GetStreamer().GetUrl());
}
