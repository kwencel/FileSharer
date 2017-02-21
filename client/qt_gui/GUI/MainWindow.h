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
        /**
         * Slot function for requesting available files signatures from tracker
         */
        void getAvailableFilesButtonClicked();

        /**
         * Slot function for informing tracker about local files
         */
        void informTrackerButtonClicked();
        /**
         * Slot function for staring download of a file on double click
         * @param row Double clicked row
         * @param column Double clicked column
         */
        void trackerFileRowDoubleClicked(int row, int column);

        /**
         * Slot function for updating progress of file download in GUI
         * @param fileHandler Pointer to FileHandler requesting GUI update
         */
        void updateFileDownloadProgress(FileHandler* fileHandler);

private:
        /**
         * Scans the files folder for files and creates FileHandlers for them
         * @return Vector of FileHandler for each found file
         */
        std::vector<std::shared_ptr<FileHandler>> scanLocalFiles();
        /**
         * Inserts local files into LocalFiles QTableWidget
         */
        void insertLocalFiles();
        /**
         * Checks how much of a file is downloaded
         * @param fileInfo FileInfo for a particular file
         * @return String representing file completeness ("0" to "100")
         */
        std::string isFileLocalAndDownloaded(FileInfo fileInfo);
        /**
         * Creates a vector of FileInfo for local files
         * @return Vector of FileInfo for local files
         */
        std::vector<FileInfo> getLocalFileInfos();

        Ui::MainWindow *ui;
        ConnectionManager &cm = ConnectionManager::getInstance();
        std::vector<FileInfo> availableFiles;
};

#endif // MAINWINDOW_H
