#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
#include <boost/filesystem.hpp>
#include <Peer.h>
#include <include/ClientProtocolTranslator.h>
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

    cm.fileHandlers = this->scanLocalFiles();
    insertLocalFiles();
    cm.listenLoop();
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

void MainWindow::insertLocalFiles() {
    ui->downloadingFilesTableWidget->setRowCount(0);
    int row = 0;
    for (FileHandler &fileHandler : this->cm.fileHandlers) {
        File &file = *fileHandler.file;
        ui->downloadingFilesTableWidget->insertRow(row);
        QTableWidgetItem *name = new QTableWidgetItem(QString::fromStdString(file.getName()));
        QTableWidgetItem *hash = new QTableWidgetItem(QString::fromStdString(file.getHash()));
        QTableWidgetItem *size = new QTableWidgetItem(QString::fromStdString(std::to_string(file.getRealSize())));
        ui->downloadingFilesTableWidget->setItem(row, 0, name);
        ui->downloadingFilesTableWidget->setItem(row, 1, hash);
        ui->downloadingFilesTableWidget->setItem(row, 2, size);
        ++row;
    }
}

std::vector<FileInfo> MainWindow::getLocalFileInfos() {
    std::vector<FileInfo> fileInfos;
    for (FileHandler &fileHandler : this->cm.fileHandlers) {
        File &file = *fileHandler.file;
        fileInfos.emplace_back(file.getFileInfo());
    }
    return fileInfos;
}


void MainWindow::informTrackerButtonClicked() {
    std::vector<FileInfo> fileInfos = this->getLocalFileInfos();
    Peer localPeer(this->cm.getOwnIP(), this->cm.getOwnPort(), fileInfos);
    Connection conn(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    char header = PROTOCOL_HEADER_REGISTER;
    std::string message = ClientProtocolTranslator::generateMessage<Peer>(header, localPeer);
    conn.write(message);
}

void MainWindow::getAvailableFilesButtonClicked() {
    Connection conn(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    char header = PROTOCOL_HEADER_LIST_FILES;
    std::string message = ClientProtocolTranslator::generateMessage<std::string>(header, "");
    conn.write(message);
    std::string first = conn.read(9);
    header = ProtocolUtils::decodeHeader(first.substr(0, 1));
    uint64_t  size = ProtocolUtils::decodeSize(first.substr(1, 8));
    std::string response = conn.read(size);
    std::vector<FileInfo> fileInfos = ClientProtocolTranslator::decodeMessage<std::vector<FileInfo>>(response);

    ui->availableFilesTableWidget->setRowCount(0);
    int row = 0;

    for (FileInfo &fileInfo : fileInfos) {

        ui->availableFilesTableWidget->insertRow(row);
        QTableWidgetItem *name = new QTableWidgetItem(QString::fromStdString(fileInfo.getName()));
        QTableWidgetItem *hash = new QTableWidgetItem(QString::fromStdString(fileInfo.getHash()));
        QTableWidgetItem *fileSize = new QTableWidgetItem(QString::fromStdString(std::to_string(fileInfo.getSize())));
        ui->availableFilesTableWidget->setItem(row, 0, name);
        ui->availableFilesTableWidget->setItem(row, 1, hash);
        ui->availableFilesTableWidget->setItem(row, 2, fileSize);
        ++row;
    }
}

void MainWindow::trackerFileRowDoubleClicked(int row, int column) {
    LOG(INFO) << "Row: " + std::to_string(row) + ", column: " + std::to_string(column) + " clicked";
}

