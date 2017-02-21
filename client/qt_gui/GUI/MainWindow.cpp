#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
#include <boost/filesystem.hpp>
#include <Peer.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <TrackerHandler.h>

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QHeaderView *header = ui->downloadingFilesTableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    header = ui->availableFilesTableWidget->horizontalHeader();
    header->setSectionResizeMode(QHeaderView::Stretch);

    cm.setFileHandlers(this->scanLocalFiles());
    insertLocalFiles();
    cm.listenLoop();
    cm.processIncomingConnections();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<std::shared_ptr<FileHandler>> MainWindow::scanLocalFiles() {
    namespace fs = boost::filesystem;
    fs::path fullPath(fs::current_path());
    fullPath /= "files";
    fs::directory_iterator end_iter;

    std::vector<std::shared_ptr<FileHandler>> fileHandlers;

    if (fs::exists(fullPath) && fs::is_directory(fullPath)) {
        for (fs::directory_iterator dirIter(fullPath); dirIter != end_iter; ++dirIter) {
            std::string filePath = dirIter->path().string();
            if (filePath.substr(filePath.length() - 5) == ".meta") {
                LOG(INFO) << "Skipping .meta file " + filePath;
                continue;
            }
            LOG(INFO) << filePath;
            if (fs::is_regular_file(dirIter->status()) ) {
                std::string path = dirIter->path().filename().string();
                fileHandlers.emplace_back(std::make_shared<FileHandler>(path));
            }
        }
    }
    return fileHandlers;
}

void MainWindow::insertLocalFiles() {
    ui->downloadingFilesTableWidget->setRowCount(0);
    int row = 0;
    for (auto fileHandler : this->cm.getFileHandlers()) {
        File &file = *fileHandler.get()->file;
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
    for (auto fileHandler : this->cm.getFileHandlers()) {
        File &file = *fileHandler.get()->file;
        fileInfos.emplace_back(file.getFileInfo());
    }
    return fileInfos;
}


void MainWindow::informTrackerButtonClicked() {
    std::vector<FileInfo> localFileInfoVector = this->getLocalFileInfos();
    std::string response = TrackerHandler::registerToTracker(cm.getOwnIP(), cm.getOwnPort(), localFileInfoVector);
}

void MainWindow::getAvailableFilesButtonClicked() {
    availableFiles = TrackerHandler::getAvailableFiles();
    ui->availableFilesTableWidget->setRowCount(0);
    int row = 0;

    for (FileInfo &fileInfo : availableFiles) {
        ui->availableFilesTableWidget->insertRow(row);
        QTableWidgetItem *name = new QTableWidgetItem(QString::fromStdString(fileInfo.getName()));
        QTableWidgetItem *hash = new QTableWidgetItem(QString::fromStdString(fileInfo.getHash()));
        QTableWidgetItem *fileSize = new QTableWidgetItem(QString::fromStdString(std::to_string(fileInfo.getSize())));
        QTableWidgetItem *progress = new QTableWidgetItem(QString::fromStdString("0"));
        ui->availableFilesTableWidget->setItem(row, 0, name);
        ui->availableFilesTableWidget->setItem(row, 1, hash);
        ui->availableFilesTableWidget->setItem(row, 2, fileSize);
        ui->availableFilesTableWidget->setItem(row, 3, progress);
        if (isFileLocalAndDownloaded(fileInfo)) {
            QFont newFont = ui->availableFilesTableWidget->item(row, 0)->font();
            newFont.setBold(true);
            ui->availableFilesTableWidget->item(row, 0)->setFont(newFont);
            ui->availableFilesTableWidget->item(row, 1)->setFont(newFont);
            ui->availableFilesTableWidget->item(row, 2)->setFont(newFont);
            ui->availableFilesTableWidget->item(row, 3)->setFont(newFont);
        }
        ++row;
    }
}

void MainWindow::trackerFileRowDoubleClicked(int row, int column) {
//    LOG(INFO) << "Row: " + std::to_string(row) + ", column: " + std::to_string(column) + " clicked";
//    char header = PROTOCOL_HEADER_PEERS_WITH_FILE;
//    std::string hash  = cm.fileHandlers[row].get()->file.get()->getHash();
//    std::string message = ClientProtocolTranslator::generateMessage<std::string>(header, hash);
//    Connection tracker(TRACKER_BIND_IP, TRACKER_BIND_PORT);
//    tracker.write(message);
//    std::string first9bytes = tracker.read(9);
//    header = ProtocolUtils::decodeHeader(first9bytes.substr(0,1));
//    uint64_t size = ProtocolUtils::decodeSize(first9bytes.substr(1,8));
//    std::string encodedPeersWithFile = tracker.read(size);
//    std::vector<PeerFile> peersWithFile = ClientProtocolTranslator::decodeMessage<std::vector<PeerFile>>(encodedPeersWithFile);
    FileInfo fileInfo = availableFiles[row];
    std::shared_ptr<FileHandler> newFileHandler = std::make_shared<FileHandler>(fileInfo);
    cm.addFileHandler(newFileHandler);
    connect(newFileHandler.get(), SIGNAL(updateFileHandlerProgress(FileHandler*)), this, SLOT(updateFileDownloadProgress(FileHandler*)));
    newFileHandler->beginDownload();
}

void MainWindow::updateFileDownloadProgress(FileHandler *fileHandler) {
    //float progress = fileHandler->file->getDownloadedChunksAmount()/fileHandler->file->getChunksAmount();
    float progress = 0;
    std::string hash = fileHandler->file->getHash();

    for (unsigned int i = 0; i < 0; ++i) {
        std::string rowHash = ui->downloadingFilesTableWidget->item(i, 1)->text().toStdString();
        if (rowHash == hash) {
            ui->downloadingFilesTableWidget->item(i, 3)->setText(QString::fromStdString(std::to_string(progress)));
            break;
        }
    }
}

bool MainWindow::isFileLocalAndDownloaded(FileInfo fileInfo) {
    for(auto fileSharedPtr : cm.getFileHandlers()) {
        if (fileSharedPtr.get()->file->getHash() == fileInfo.getHash() && fileSharedPtr.get()->file->isDownloaded()) {
            return true;
        }
    }
    return false;
}
