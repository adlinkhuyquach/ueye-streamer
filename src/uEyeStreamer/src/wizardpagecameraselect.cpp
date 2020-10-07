#include "wizardpagecameraselect.h"
#include <QHeaderView>
#include <QMessageBox>
#include <QVBoxLayout>

WizardPageCameraSelect::WizardPageCameraSelect(CameraStreamPtr cameraStream, QWidget *parent) :
    QWizardPage(parent), cameraListView(0), cameraListModel(0), m_CameraStream(cameraStream)
{
    initWidget();
    initConnection();
}

void WizardPageCameraSelect::initWidget()
{
    setTitle(tr("Camera selection"));
    setSubTitle(tr("Please select the uEye camera which will be used for streaming."));

    cameraListView = new QTableView(this);
    cameraListView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    cameraListView->setSortingEnabled(true);
    cameraListView->setSelectionBehavior(QTableView::SelectRows);
    cameraListView->setSelectionMode(QTableView::SingleSelection);
    cameraListView->setAlternatingRowColors(true);

    cameraListView->verticalHeader()->setDefaultSectionSize(cameraListView->verticalHeader()->minimumSectionSize());
    cameraListView->verticalHeader()->hide();

    cameraListView->horizontalHeader()->setStretchLastSection(true);
    cameraListView->horizontalHeader()->setHighlightSections(false);

    cameraListModel = new CameraListModel(cameraListView);
    cameraListView->setModel(cameraListModel);
    cameraListView->resizeColumnsToContents();
    cameraListView->selectRow(0);

    QVBoxLayout *layout = new QVBoxLayout(this);
    setLayout(layout);

    layout->addWidget(cameraListView); 
}

void WizardPageCameraSelect::initConnection()
{
    connect(cameraListView, SIGNAL(clicked(QModelIndex)), this, SLOT(onCameraListClicked(QModelIndex)));
}

void WizardPageCameraSelect::onCameraListClicked(QModelIndex index)
{
    bool isInUse = cameraListModel->index(index.row(), CameraListModel::Available).data().toString() == "No";
}

bool WizardPageCameraSelect::validatePage()
{
    QModelIndex index = cameraListView->currentIndex();

    if (!index.isValid())
    {
        return false;
    }

    try
    {
        m_CameraStream->GetCamera().Init(cameraListModel->index(index.row(), CameraListModel::DevID).data().toInt());
    }
    catch(std::exception& e)
    {
        QMessageBox::warning(this, "Information", e.what());
        return false;
    }

    return true;
}
