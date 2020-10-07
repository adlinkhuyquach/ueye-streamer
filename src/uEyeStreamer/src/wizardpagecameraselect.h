#ifndef WIZARDPAGECAMERASELECT_H
#define WIZARDPAGECAMERASELECT_H

#include <QWizardPage>
#include <QLabel>
#include <QTableView>

#include "camera.h"
#include "cameralistmodel.h"
#include "camerastream.h"

class WizardPageCameraSelect : public QWizardPage
{
    Q_OBJECT
public:
    explicit WizardPageCameraSelect(CameraStreamPtr cameraStream, QWidget *parent = 0);

protected:
    void initWidget();
    void initConnection();
    bool validatePage();

private:
    QTableView *cameraListView;
    CameraListModel *cameraListModel;

    CameraStreamPtr m_CameraStream;

private slots:
    void onCameraListClicked(QModelIndex index);

signals:

public slots:

};

#endif // WIZARDPAGECAMERASELECT_H
