#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <FileHandler.h>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    private slots:
        void getAvailableFilesButtonClicked();
        void informTrackerButtonClicked();

private:
        void scanLocalFiles();
        void insertLocalFiles();
        Ui::MainWindow *ui;
        std::vector<FileHandler> localFileHandlers;
};

#endif // MAINWINDOW_H
