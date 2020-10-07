#ifndef WIZARDPAGESTREAM_H
#define WIZARDPAGESTREAM_H

#include <QWizardPage>
#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QSpinBox>

#include "camerastream.h"

class WizardPageStream : public QWizardPage
{
    Q_OBJECT
public:
    explicit WizardPageStream(CameraStreamPtr cameraStream, QWidget *parent = 0);

protected:
    void initWidget();
    void initializePage();
    bool validatePage();

private:
    QLineEdit* lineEditStreamName;
    QLineEdit* lineEditStreamDescription;
    QLineEdit* lineEditStreamInformation;

    QComboBox* comboBoxCodec;
    QComboBox* comboBoxdstWidth;
    QComboBox* comboBoxdstHeight;

    QSpinBox* spinBitrate;

    CameraStreamPtr m_CameraStream;

    IS_STREAM_PIXELFMT cameraFmtToStreamFmt(const Camera::ColorMode colorMode);

signals:

public slots:

};

#endif // WIZARDPAGESTREAM_H
