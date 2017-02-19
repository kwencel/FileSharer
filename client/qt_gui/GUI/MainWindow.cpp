#include <SerializationHelper.h>
#include <easylogging++.h>
#include <include/ConnectionManager.h>
#include <FileInfo.h>
#include <boost/filesystem.hpp>
#include <Peer.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHeaderView *header = ui->downloadingFilesTableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    header = ui->availableFilesTableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    this->scanLocalFiles();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::scanLocalFiles() {
    namespace fs = boost::filesystem;
    fs::path fullPath(fs::current_path());
    fullPath /= "files";
    fs::directory_iterator end_iter;

    if (fs::exists(fullPath) && fs::is_directory(fullPath)) {
        for(fs::directory_iterator dirIter(fullPath);dirIter != end_iter;++dirIter)
        {
            LOG(INFO) << dirIter->path().string();
            if (fs::is_regular_file(dirIter->status()) )
            {
                localFiles.emplace_back(File(dirIter->path().string()));
            }
        }
    }
}

void MainWindow::getAvailableFilesButtonClicked() {

}

void MainWindow::informTrackerButtonClicked() {

}
