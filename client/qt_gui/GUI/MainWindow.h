#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <FileHandler.h>
#include <ConnectionManager.h>

namespace Ui {
    class MainWindow;
}

class ConnectionManager;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    private slots:
        void getAvailableFilesButtonClicked();
        void informTrackerButtonClicked();
        void trackerFileRowDoubleClicked(int row, int column);
        void updateFileDownloadProgress(FileHandler* fileHandler);

private:
        std::vector<std::shared_ptr<FileHandler>> scanLocalFiles();
        void insertLocalFiles();
        std::string isFileLocalAndDownloaded(FileInfo fileInfo);
        std::vector<FileInfo> getLocalFileInfos();

        Ui::MainWindow *ui;
        ConnectionManager &cm = ConnectionManager::getInstance();
        std::vector<FileInfo> availableFiles;
};

#endif // MAINWINDOW_H
