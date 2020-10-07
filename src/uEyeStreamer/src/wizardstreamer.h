#ifndef WIZARDSTREAMER_H
#define WIZARDSTREAMER_H

#include <QWizard>

#include "wizardpageintro.h"
#include "wizardpagecameraselect.h"
#include "wizardpagecamera.h"
#include "wizardpagestream.h"
#include "wizardpagefinish.h"

#include "camerastream.h"

class WizardStreamer : public QWizard
{
    Q_OBJECT
public:
    explicit WizardStreamer(CameraStreamPtr cameraStream, QWidget *parent = 0);

public slots:


protected:
    void initWidget();
    bool validatePage();

private:
    WizardPageIntro *pageIntro;
    WizardPageCameraSelect *pageCameraSelect;
    WizardPageCamera *pageCamera;
    WizardPageStream *pageStream;
    WizardPageFinish *pageFinish;

    CameraStreamPtr m_CameraStream;

signals:

};

#endif // WIZARDSTREAMER_H
