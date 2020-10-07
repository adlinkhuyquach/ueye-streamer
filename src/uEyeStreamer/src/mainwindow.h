#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QToolBar>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialogButtonBox>
#include <QToolButton>
#include <QMenu>

#include "camerastream.h"
#include "streamwidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:
    void initWidget();
    void initConnection();

protected slots:
    void onAddStream();
    void onRemoveStream(StreamWidget *widget);

    void onAboutStreamer();
    void onAboutQt();
    void onAboutFFmpeg();
    void onAboutLive555();

private:
    QPushButton *buttonQuit;
    QPushButton *buttonAddStream;
    QToolButton *toolButtonAbout;

    QMenu *menuAbout;

    QDialogButtonBox *buttonBox;

    QVBoxLayout* layoutMain;
    QVBoxLayout* layoutSessions;
    QHBoxLayout* layoutButtons;

    QWidget* mainWidget;
};

#endif // MAINWINDOW_H
