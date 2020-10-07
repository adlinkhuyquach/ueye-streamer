#include "wizardpagestream.h"
#include <QVBoxLayout>
#include <QFormLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QMessageBox>

namespace
{
    static std::map<Camera::ColorMode, IS_STREAM_PIXELFMT> g_MapLutColorModePixelFmt;
}

WizardPageStream::WizardPageStream(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizardPage(parent), m_CameraStream(cameraStream)
{
    g_MapLutColorModePixelFmt[Camera::BGR8Packed] = IS_STREAM_FMT_BGR24;
    g_MapLutColorModePixelFmt[Camera::RGB8Packed] = IS_STREAM_FMT_RGB24;

    g_MapLutColorModePixelFmt[Camera::BGRA8Packed] = IS_STREAM_FMT_BGRA;
    g_MapLutColorModePixelFmt[Camera::RGBA8Packed] = IS_STREAM_FMT_RGBA;

    initWidget();
}

void WizardPageStream::initWidget()
{
    setTitle(tr("Stream configuration"));
    setSubTitle(tr("Please configure your stream session."));

    QVBoxLayout *layoutV = new QVBoxLayout(this);
    setLayout(layoutV);

    QGroupBox *groupStreamInfo = new QGroupBox(tr("Rtsp stream"), this);
    layoutV->addWidget(groupStreamInfo);

    QFormLayout *layoutGroupStreamInfo = new QFormLayout(groupStreamInfo);
    groupStreamInfo->setLayout(layoutGroupStreamInfo);

    lineEditStreamName = new QLineEdit("mystream", groupStreamInfo);
    lineEditStreamName->setValidator(new QRegExpValidator( QRegExp("[A-Za-z0-9_]{0,255}"), this ));
    layoutGroupStreamInfo->addRow(tr("Stream name:"), lineEditStreamName);

    lineEditStreamDescription = new QLineEdit("descr", groupStreamInfo);
    lineEditStreamDescription->setValidator(new QRegExpValidator( QRegExp("[A-Za-z0-9_]{0,255}"), this ));
    layoutGroupStreamInfo->addRow(tr("Stream description"), lineEditStreamDescription);

    lineEditStreamInformation = new QLineEdit("info", groupStreamInfo);
    lineEditStreamInformation->setValidator(new QRegExpValidator( QRegExp("[A-Za-z0-9_]{0,255}"), this ));
    layoutGroupStreamInfo->addRow(tr("Stream information"), lineEditStreamInformation);

    QGroupBox *groupStreamConfig = new QGroupBox(tr("Configuration"), this);
    layoutV->addWidget(groupStreamConfig);

    QGridLayout *layoutGroupStreamConfig = new QGridLayout(groupStreamConfig);
    groupStreamConfig->setLayout(layoutGroupStreamConfig);

    comboBoxCodec = new QComboBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Codec:")), 0, 0, 1, 1);
    layoutGroupStreamConfig->addWidget(comboBoxCodec, 0, 1, 1, 1);

    spinBitrate = new QSpinBox(this);
    spinBitrate->setRange(0, INT_MAX);
    spinBitrate->setValue(5000000);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Bitrate:")), 0, 2, 1, 1);
    layoutGroupStreamConfig->addWidget(spinBitrate, 0, 3, 1, 1);

    comboBoxdstWidth = new QComboBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Width:")), 1, 0, 1, 1);
    layoutGroupStreamConfig->addWidget(comboBoxdstWidth, 1, 1, 1, 1);

    comboBoxdstHeight = new QComboBox(this);
    layoutGroupStreamConfig->addWidget(new QLabel(tr("Height:")), 1, 2, 1, 1);
    layoutGroupStreamConfig->addWidget(comboBoxdstHeight, 1, 3, 1, 1);

    comboBoxCodec->addItem("MJPEG", IS_STREAM_CODEC_MJPEG);
    comboBoxCodec->addItem("H264", IS_STREAM_CODEC_H264);
}

void WizardPageStream::initializePage()
{
    comboBoxdstWidth->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().width() / 1));
    comboBoxdstHeight->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().height() / 1));

    comboBoxdstWidth->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().width() / 2));
    comboBoxdstHeight->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().height() / 2));

    comboBoxdstWidth->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().width() / 4));
    comboBoxdstHeight->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().height() / 4));

    comboBoxdstWidth->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().width() / 8));
    comboBoxdstHeight->addItem(QString::number(m_CameraStream->GetCamera().GetAoi().height() / 8));
}

bool WizardPageStream::validatePage()
{
    try
    {
        m_CameraStream->GetStreamer().addSession(lineEditStreamName->text(), lineEditStreamDescription->text(), lineEditStreamInformation->text());
        m_CameraStream->GetStreamer().addStream(QSize(comboBoxdstWidth->currentText().toInt(), comboBoxdstHeight->currentText().toInt()),
                             m_CameraStream->GetCamera().GetAoi().size(), cameraFmtToStreamFmt(m_CameraStream->GetCamera().GetColorMode()),
                             static_cast<IS_STREAM_CODEC>(comboBoxCodec->itemData(comboBoxCodec->currentIndex()).toInt()), spinBitrate->value());
    }
    catch(std::exception& e)
    {
        m_CameraStream->GetStreamer().removeSession();
        QMessageBox::warning(this, "Information", e.what());
        return false;
    }

    return true;
}

IS_STREAM_PIXELFMT WizardPageStream::cameraFmtToStreamFmt(const Camera::ColorMode colorMode)
{
    return g_MapLutColorModePixelFmt[colorMode];
}
