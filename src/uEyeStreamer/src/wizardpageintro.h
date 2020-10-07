#ifndef WIZARDPAGEINTRO_H
#define WIZARDPAGEINTRO_H

#include <QWizardPage>
#include <QSpinBox>
#include <QLabel>

#include "camerastream.h"

class WizardPageIntro : public QWizardPage
{
    Q_OBJECT
public:
    explicit WizardPageIntro(CameraStreamPtr cameraStream, QWidget *parent = 0);

protected:
    void initWidget();

private:
    CameraStreamPtr m_CameraStream;
    QSpinBox *spinBoxRtspPort;

private slots:
    bool validatePage();

signals:

public slots:

};

#endif // WIZARDPAGEINTRO_H
