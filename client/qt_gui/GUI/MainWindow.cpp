#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
#include <boost/filesystem.hpp>
#include <Peer.h>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <TrackerHandler.h>
#include <QMessageBox>
#include <CustomExceptions.h>

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
    try {
        cm.listenLoop();
    } catch (std::runtime_error &e) {
        std::string s(e.what());
        s = "Critical error! " + s;
        QMessageBox::warning(
                this,
                tr("Error"),
                QString::fromStdString(s));
    }
    cm.processIncomingConnections();
    this->informTrackerButtonClicked();
    this->getAvailableFilesButtonClicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

std::vector<std::shared_ptr<FileHandler>> MainWindow::scanLocalFiles() {
    namespace fs = boost::filesystem;
    fs::path fullPath(fs::current_path());
    fullPath /= FILES_PATH_PREFIX;
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
    try {
        std::vector<FileInfo> localFileInfoVector = this->getLocalFileInfos();
        std::string response = TrackerHandler::registerToTracker(cm.getOwnIP(), cm.getOwnPort(), localFileInfoVector);
    } catch (ConnectionError &e) {
        std::string s(e.what());
        s = "Cannot register to tracker. " + s;
        QMessageBox::warning(
                this,
                tr("Error"),
                QString::fromStdString(s));
    }
}

void MainWindow::getAvailableFilesButtonClicked() {
    try {
        availableFiles = TrackerHandler::getAvailableFiles();
        ui->availableFilesTableWidget->setRowCount(0);
        int row = 0;

        for (FileInfo &fileInfo : availableFiles) {
            ui->availableFilesTableWidget->insertRow(row);
            QTableWidgetItem *name = new QTableWidgetItem(QString::fromStdString(fileInfo.getName()));
            QTableWidgetItem *hash = new QTableWidgetItem(QString::fromStdString(fileInfo.getHash()));
            QTableWidgetItem *fileSize = new QTableWidgetItem(
                    QString::fromStdString(std::to_string(fileInfo.getSize())));
            QTableWidgetItem *progress = new QTableWidgetItem(QString::fromStdString("100"));
            QTableWidgetItem *nPeers = new QTableWidgetItem(QString::fromStdString(std::to_string(fileInfo.getNumberOfPeers())));
            ui->availableFilesTableWidget->setItem(row, 0, name);
            ui->availableFilesTableWidget->setItem(row, 1, hash);
            ui->availableFilesTableWidget->setItem(row, 2, fileSize);
            ui->availableFilesTableWidget->setItem(row, 4, nPeers);
            std::string progressValue = "100";
            if ((progressValue = isFileLocalAndDownloaded(fileInfo)) != "100") {
                QFont newFont = ui->availableFilesTableWidget->item(row, 0)->font();
                newFont.setBold(true);
                ui->availableFilesTableWidget->item(row, 0)->setFont(newFont);
                ui->availableFilesTableWidget->item(row, 1)->setFont(newFont);
                ui->availableFilesTableWidget->item(row, 2)->setFont(newFont);
                ui->availableFilesTableWidget->item(row, 4)->setFont(newFont);
                progress->setFont(newFont);
            }
            progress->setText(QString::fromStdString(progressValue));
            ui->availableFilesTableWidget->setItem(row, 3, progress);
            ++row;
        }
    } catch (ConnectionError &e) {
        std::string s(e.what());
        s = "Cannot check tracker for available files. " + s;
        QMessageBox::warning(
                this,
                tr("Error"),
                QString::fromStdString(s));
    }
}

void MainWindow::trackerFileRowDoubleClicked(int row, int column) {
    if (isFileLocalAndDownloaded(availableFiles[row]) != "100" && !cm.isFileBeingDownloaded(availableFiles[row])) {
        FileInfo fileInfo = availableFiles[row];
        try {
            std::shared_ptr<FileHandler> newFileHandler = std::make_shared<FileHandler>(fileInfo);
            cm.addFileHandler(newFileHandler);
            cm.addToDownloadingFiles(newFileHandler);
            connect(newFileHandler.get(), SIGNAL(updateFileHandlerProgress(FileHandler * )), this,
                    SLOT(updateFileDownloadProgress(FileHandler * )));
            newFileHandler->beginDownload();
        } catch (std::exception &e) {
            LOG(ERROR) << "Could not download file. Reason: " << e.what();
        }
    }
    else {
        LOG(INFO) << "File already downloaded";
    }
}

void MainWindow::updateFileDownloadProgress(FileHandler *fileHandler) {
    float progress = 0;
    std::string hash = fileHandler->file->getHash();

    File *file = fileHandler->file.get();
    progress = (float) file->getDownloadedChunksAmount() / file->getChunksAmount();
    progress *= 100;

    std::stringstream stream;
    stream << std::fixed << std::setprecision(0) << progress;
    std::string stringProgress = stream.str();
    QString qStringProgress = QString::fromStdString(stringProgress);

    for (unsigned int i = 0; i < (unsigned int) ui->availableFilesTableWidget->rowCount(); ++i) {
        std::string rowHash = ui->availableFilesTableWidget->item(i, 1)->text().toStdString();
        if (rowHash == hash) {
            ui->availableFilesTableWidget->item(i, 3)->setText(qStringProgress);
            if (stringProgress == "100") {
                this->informTrackerButtonClicked();
                QFont newFont = ui->availableFilesTableWidget->item(i,3)->font();
                newFont.setBold(false);
                this->cm.removeFromDownloadingFiles(fileHandler);
                for (unsigned int j = 0; j < (unsigned int) ui->availableFilesTableWidget->columnCount(); ++j) {
                    ui->availableFilesTableWidget->item(i, j)->setFont(newFont);
                }
            }
            break;
        }
    }
}

std::string MainWindow::isFileLocalAndDownloaded(FileInfo fileInfo) {
    float progress = 0.0f;
    std::string result = "0";
    for (auto fileSharedPtr : cm.getFileHandlers()) {
        if (fileSharedPtr.get()->file->getHash() == fileInfo.getHash()) {
            File *file = fileSharedPtr.get()->file.get();
            progress = (float) file->getDownloadedChunksAmount() / file->getChunksAmount();
            progress *= 100;
            std::stringstream stream;
            stream << std::fixed << std::setprecision(0) << progress;
            result = stream.str();
        }
    }
    return result;
}

