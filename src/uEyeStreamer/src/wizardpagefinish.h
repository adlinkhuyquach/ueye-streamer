#ifndef WIZARDPAGEFINISH_H
#define WIZARDPAGEFINISH_H

#include <QWizardPage>
#include <QCheckBox>
#include "camerastream.h"

class WizardPageFinish : public QWizardPage
{
    Q_OBJECT
public:
    explicit WizardPageFinish(CameraStreamPtr cameraStream, QWidget *parent = 0);

protected:
    void initializePage();

private:
    CameraStreamPtr m_CameraStream;

signals:

public slots:

};

#endif // WIZARDPAGEFINISH_H
