#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

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
    void on_inputBrowse_clicked();

    void on_inputOutFile_textChanged(const QString &arg1);

    void on_inputWrite_clicked();

    void on_inputSaveLog_clicked();

private:
    void assertOutputDir();

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
