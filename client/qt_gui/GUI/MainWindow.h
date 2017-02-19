#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <File.h>

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
        Ui::MainWindow *ui;
        std::vector<File> localFiles;
};

#endif // MAINWINDOW_H
