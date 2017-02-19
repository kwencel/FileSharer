#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
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

//    cm.fileHandlers = this->scanLocalFiles();
//    cm.listenLoop();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<FileHandler> MainWindow::scanLocalFiles() {
    namespace fs = boost::filesystem;
    fs::path fullPath(fs::current_path());
    fullPath /= "files";
    fs::directory_iterator end_iter;

    std::vector<FileHandler> fileHandlers;

    if (fs::exists(fullPath) && fs::is_directory(fullPath)) {
        for(fs::directory_iterator dirIter(fullPath);dirIter != end_iter;++dirIter)
        {
            LOG(INFO) << dirIter->path().string();
            if (fs::is_regular_file(dirIter->status()) )
            {
                std::string path = dirIter->path().string();
                fileHandlers.emplace_back(path);
            }
        }
    }
    return fileHandlers;
}

//void MainWindow::insertLocalFiles() {
//    ui->downloadingFilesTableWidget->setRowCount(0);
//    int row = 0;
//    for (FileHandler &fileHandler : this->cm.fileHandlers) {
//        File &file = *fileHandler.file;
//        ui->downloadingFilesTableWidget->insertRow(row);
//        QTableWidgetItem *name = new QTableWidgetItem(QString::fromStdString(file.getName()));
//        QTableWidgetItem *hash = new QTableWidgetItem(QString::fromStdString(file.getHash()));
//        QTableWidgetItem *size = new QTableWidgetItem(QString::fromStdString(std::to_string(file.getRealSize())));
//        ui->downloadingFilesTableWidget->setItem(row, 0, name);
//        ui->downloadingFilesTableWidget->setItem(row, 1, hash);
//        ui->downloadingFilesTableWidget->setItem(row, 2, size);
//        ++row;
//    }
//}

//std::vector<FileInfo> MainWindow::getLocalFileInfos() {
//    std::vector<FileInfo> fileInfos;
//    for (FileHandler &fileHandler : this->cm.fileHandlers) {
//        File &file = *fileHandler.file;
//        fileInfos.emplace_back(FileInfo(file.getName(), file.getHash(), file.getDownloadedChunks()));
//    }
//    return fileInfos;
//}


void MainWindow::informTrackerButtonClicked() {
    //std::vector<FileInfo> fileInfos = this->getLocalFileInfos();
    Connection conn(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    char header = PROTOCOL_HEADER_REGISTER;
    //std::string message = ClientProtocolTranslator::generateMessage<>()
}

void MainWindow::getAvailableFilesButtonClicked() {

}

