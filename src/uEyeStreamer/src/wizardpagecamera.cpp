#include "wizardpagecamera.h"
#include <QMessageBox>
#include <QVBoxLayout>
#include <QFileDialog>
#include <QLabel>

WizardPageCamera::WizardPageCamera(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizardPage(parent), m_CameraStream(cameraStream), comboBoxPixelFormat(0), comboBoxImageFormat(0), spinBoxFramerate(0)
{
    bParameterFileUsed = false;

    initWidget();
    initConnection();
}

void WizardPageCamera::initWidget()
{
    setTitle(tr("Camera configuration"));
    setSubTitle(tr("Please configure your uEye camera."));

    QVBoxLayout *layoutV = new QVBoxLayout(this);
    setLayout(layoutV);

    groupStreamConfig = new QGroupBox(tr("Camera configuration"), this);
    layoutV->addWidget(groupStreamConfig);

    QGridLayout *layoutGroupStreamConfig = new QGridLayout(groupStreamConfig);
    groupStreamConfig->setLayout(layoutGroupStreamConfig);

    comboBoxPixelFormat = new QComboBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Pixel format:")), 0, 0, 1, 1);
    layoutGroupStreamConfig->addWidget(comboBoxPixelFormat, 0, 1, 1, 1);

    comboBoxImageFormat = new QComboBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Image format:")), 1, 0, 1, 1);
    layoutGroupStreamConfig->addWidget(comboBoxImageFormat, 1, 1, 1, 1);

    spinBoxFramerate = new QDoubleSpinBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Framerate:")), 2, 0, 1, 1);
    layoutGroupStreamConfig->addWidget(spinBoxFramerate, 2, 1, 1, 1);

    buttonLoadParameterFile = new QPushButton(tr("Load parameter file..."), this);
    layoutGroupStreamConfig->addWidget(buttonLoadParameterFile, 3, 1, 1, 1);
}

void WizardPageCamera::initConnection()
{
    connect(buttonLoadParameterFile, SIGNAL(clicked(bool)), this, SLOT(onLoadParameterFile()));
}

void WizardPageCamera::initializePage()
{
    comboBoxPixelFormat->addItem("BGR8 Packed", Camera::BGR8Packed);
    comboBoxPixelFormat->addItem("BGRA8 Packed", Camera::BGRA8Packed);

    Camera::ImageFormatList imageFormatList = m_CameraStream->GetCamera().GetImageFormatList();

    foreach (const IMAGE_FORMAT_INFO& format, imageFormatList)
    {
        if (format.nSupportedCaptureModes & CAPTMODE_TRIGGER_SOFT_CONTINUOUS ||
            format.nSupportedCaptureModes & CAPTMODE_FREERUN)
        {
            comboBoxImageFormat->addItem(format.strFormatName, format.nFormatID);
        }
    }

    Camera::Range<double> range = m_CameraStream->GetCamera().GetFramerateRange();

    spinBoxFramerate->setRange(range.min, range.max);
    spinBoxFramerate->setValue(m_CameraStream->GetCamera().GetFramerate());
}

bool WizardPageCamera::validatePage()
{
    try
    {
        if (!bParameterFileUsed)
        {
            m_CameraStream->GetCamera().SetColorMode(static_cast<Camera::ColorMode>(comboBoxPixelFormat->itemData(comboBoxPixelFormat->currentIndex()).toInt()));
            m_CameraStream->GetCamera().SetImageFormat(comboBoxImageFormat->itemData(comboBoxImageFormat->currentIndex()).toInt());
            m_CameraStream->GetCamera().SetFramerate(spinBoxFramerate->value());
        }

        m_CameraStream->GetCamera().AllocBuffer();
    }
    catch(std::exception& e)
    {
        QMessageBox::warning(this, "Information", e.what());
        return false;
    }

    return true;
}

void WizardPageCamera::onLoadParameterFile()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Open file"), QString(), tr("Parameter file (*.ini)"));
    if (!file.isEmpty())
    {
        try
        {
            Camera& cam = m_CameraStream->GetCamera();
            cam.LoadParameter(file.toStdString());

            /* now check settings */

            /* invalid color mode */
            if (cam.GetColorMode() != Camera::BGR8Packed &&
                cam.GetColorMode() != Camera::BGRA8Packed &&
                cam.GetColorMode() != Camera::RGB8Packed &&
                cam.GetColorMode() != Camera::RGBA8Packed)
            {
                QMessageBox::warning(this, "Information", "Parameter file could not be used for streaming. Invalid color mode!");
                return;
            }

            /* check aoi pos */
            if (cam.IsAbsoluteAoi())
            {
                QMessageBox::warning(this, "Information", "Parameter file could not be used for streaming. Absolute aoi position is enabled!");
                return;
            }

            comboBoxPixelFormat->setEnabled(false);
            comboBoxImageFormat->setEnabled(false);
            spinBoxFramerate->setEnabled(false);

            bParameterFileUsed = true;
        }
        catch(std::exception& e)
        {
            QMessageBox::warning(this, "Information", e.what());
        }
    }
}
