#ifndef WIZARDPAGECAMERA_H
#define WIZARDPAGECAMERA_H

#include <QWizardPage>
#include <QComboBox>
#include <QGroupBox>
#include <QDoubleSpinBox>
#include <QPushButton>

#include "camerastream.h"

class WizardPageCamera : public QWizardPage
{
    Q_OBJECT
public:
    explicit WizardPageCamera(CameraStreamPtr cameraStream, QWidget *parent = 0);

protected:
    void initWidget();
    void initConnection();

    void initializePage();
    bool validatePage();

private:
    QGroupBox *groupStreamConfig;
    CameraStreamPtr m_CameraStream;

    QComboBox* comboBoxPixelFormat;
    QComboBox* comboBoxImageFormat;

    QDoubleSpinBox *spinBoxFramerate;
    QPushButton *buttonLoadParameterFile;

    bool bParameterFileUsed;

protected slots:
    void onLoadParameterFile();

};

#endif // WIZARDPAGECAMERA_H
