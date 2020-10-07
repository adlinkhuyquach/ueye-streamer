#include "mainwindow.h"

#include <QMessageBox>
#include <QInputDialog>
#include <QMessageBox>
#include <QDebug>

#include "streamwidget.h"
#include "wizardstreamer.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(320, 240);
    initWidget();
    initConnection();

    setWindowIcon(QIcon(":/res/icons/captureStart.png"));

    onAddStream();
}

MainWindow::~MainWindow()
{

}

void MainWindow::initWidget()
{
    mainWidget = new QWidget(this);

    layoutMain = new QVBoxLayout(mainWidget);
    mainWidget->setLayout(layoutMain);

    layoutSessions = new QVBoxLayout;
    layoutSessions->setContentsMargins(2, 8, 2, 8);
    layoutSessions->insertStretch(0, 1);

    layoutMain->addLayout(layoutSessions);

    layoutButtons = new QHBoxLayout;
    layoutMain->addLayout(layoutButtons);

    buttonBox = new QDialogButtonBox(Qt::Horizontal);

    buttonAddStream = new QPushButton(tr("Add"), this);
    buttonBox->addButton(buttonAddStream, QDialogButtonBox::ActionRole);

    buttonQuit = new QPushButton(tr("Quit"), this);
    buttonBox->addButton(buttonQuit, QDialogButtonBox::ActionRole);

    menuAbout = new QMenu(this);
    menuAbout->addAction("About Qt", this, SLOT(onAboutQt()));
    menuAbout->addAction("About FFmpeg", this, SLOT(onAboutFFmpeg()));
    menuAbout->addAction("About LIVE555", this, SLOT(onAboutLive555()));

    toolButtonAbout = new QToolButton(this);
    toolButtonAbout->setIcon(QIcon(":/res/icons/information.png"));

    toolButtonAbout->setPopupMode(QToolButton::MenuButtonPopup);
    toolButtonAbout->setMenu(menuAbout);
    buttonBox->addButton(toolButtonAbout, QDialogButtonBox::ActionRole);

    layoutButtons->addWidget(buttonBox);
    setCentralWidget(mainWidget);
}

void MainWindow::initConnection()
{
    connect(buttonQuit, SIGNAL(clicked(bool)), this, SLOT(close()));
    connect(buttonAddStream, SIGNAL(clicked(bool)), this, SLOT(onAddStream()));
    connect(toolButtonAbout, SIGNAL(clicked(bool)), this, SLOT(onAboutStreamer()));
}

void MainWindow::onAddStream()
{
    try
    {
        CameraStreamPtr cameraStream(new CameraStream);

        WizardStreamer wizardStreamer(cameraStream);
        if (wizardStreamer.exec() == QDialog::Accepted)
        {
            StreamWidget *widgetStreamer = new StreamWidget(cameraStream, mainWidget);
            connect(widgetStreamer, SIGNAL(sessionRemoved(StreamWidget*)), this, SLOT(onRemoveStream(StreamWidget*)));

            layoutSessions->insertWidget(0, widgetStreamer);
            layoutSessions->setAlignment(widgetStreamer, Qt::AlignTop);
        }
        else
        {
            if (layoutSessions->isEmpty())
            {
                Streamer::exit();
            }
        }
    }
    catch(std::exception& e)
    {
        QMessageBox::warning(this, "Information", e.what());
    }
}

void MainWindow::onRemoveStream(StreamWidget *widget)
{
    layoutSessions->removeWidget(widget);
    delete widget;

    if (layoutSessions->isEmpty())
    {
        try
        {
            Streamer::exit();
        }
        catch(std::exception& e)
        {
            qWarning() << e.what();
        }
    }
}

void MainWindow::onAboutStreamer()
{
    QMessageBox::about(this, "about uEyeStreamer", "uEye Streamer application\n"
                       "Copyright (C) 2016\n"
                       "IDS Imaging Development Systems GmbH\n\n"
                       "This software uses libraries from the FFmpeg project under the LGPLv2.1\n"
                       "This software uses libraries from the LIVE555 project under the LGPLv2.1\n"
                       "This software uses libraries from the Qt toolkit under the LGPLv3.0\n");
}

void MainWindow::onAboutQt()
{
    QMessageBox::aboutQt(this);
}

void MainWindow::onAboutFFmpeg()
{
    QMessageBox msg;
    msg.setText("FFmpeg is licensed under the GNU Lesser General Public License (LGPL) version 2.1.\n"
                "http://www.ffmpeg.org/\n\n"
                "You may use, distribute and copy the FFmpeg library under the terms of "
                "GNU Lesser General Public License version 2.1. "
                "This license makes reference to the version 3 of the GNU General "
                "Public License, which you can find in the LICENSE.GPLv3 file.");
    msg.exec();
}

void MainWindow::onAboutLive555()
{
    QMessageBox msg;
    msg.setText("LIVE555 Streaming Media is licensed under the GNU LGPL.\n"
                "http://www.live555.com/liveMedia/\n\n"
                "You may use, distribute and copy the Live555 library under the terms of "
                "GNU Lesser General Public License version 2.1. "
                "This license makes reference to the version 3 of the GNU General "
                "Public License, which you can find in the LICENSE.GPLv3 file.");
    msg.exec();
}
