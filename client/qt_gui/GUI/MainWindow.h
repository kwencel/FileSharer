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

private:
        std::vector<FileHandler> scanLocalFiles();
        void insertLocalFiles();
        std::vector<FileInfo> getLocalFileInfos();

        Ui::MainWindow *ui;
        ConnectionManager &cm = ConnectionManager::getInstance(CLIENT_BIND_IP, CLIENT_BIND_PORT);
};

#endif // MAINWINDOW_H
