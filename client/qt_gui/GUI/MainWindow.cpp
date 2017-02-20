#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
#include <boost/filesystem.hpp>
#include <Peer.h>
#include <ClientProtocolTranslator.h>
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

    cm.fileHandlers = this->scanLocalFiles(); // FIXME Pass by reference!!!
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
        for(fs::directory_iterator dirIter(fullPath);dirIter != end_iter;++dirIter)
        {
            LOG(INFO) << dirIter->path().string();
            if (fs::is_regular_file(dirIter->status()) )
            {
                std::string path = dirIter->path().string();
                fileHandlers.emplace_back(std::make_shared<FileHandler>(path));
            }
        }
    }
    return fileHandlers;
}

void MainWindow::insertLocalFiles() {
    ui->downloadingFilesTableWidget->setRowCount(0);
    int row = 0;
    for (auto fileHandler : this->cm.fileHandlers) {
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
    for (auto fileHandler : this->cm.fileHandlers) {
        File &file = *fileHandler.get()->file;
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
    availableFiles = ClientProtocolTranslator::decodeMessage<std::vector<FileInfo>>(response);

    ui->availableFilesTableWidget->setRowCount(0);
    int row = 0;

    for (FileInfo &fileInfo : availableFiles) {

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
//    cm.fileHandlers[row].get()->startDownload(peersWithFile);
}
