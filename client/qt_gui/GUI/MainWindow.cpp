#include <SerializationHelper.h>
#include <easylogging++.h>
#include <ConnectionManager.h>
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
                File newFile = File(dirIter->path().string());
                //localFileHandlers.emplace_back(FileHandler(&newFile));
            }
        }
    }
    this->insertLocalFiles();
}

void MainWindow::insertLocalFiles() {
    ui->downloadingFilesTableWidget->setRowCount(0);
    int row = 0;
    for (FileHandler &fileHandler : this->localFileHandlers) {
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

void MainWindow::informTrackerButtonClicked() {
    /*std::shared_ptr<Connection> conn = ConnectionManager::getInstance().requestConnection(TRACKER_BIND_IP, TRACKER_BIND_PORT);
    char header = PROTOCOL_HEADER_REGISTER;
    FileInfo fi("nazwa", "hash", std::vector<bool>{1,1,1,0,0,0});
    Peer p("127.0.0.1", std::vector<FileInfo>{fi});
    LOG(INFO) << "Sending header: " + std::to_string(header);
    std::string serialized = SerializationHelper::serialize<Peer>(header, p);
    conn.get()->send(serialized);
    std::cout << conn.get()->receive();*/
}

void MainWindow::getAvailableFilesButtonClicked() {

}
