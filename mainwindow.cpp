#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QElapsedTimer>
#include <QMessageBox>
#include <fstream>
#include <cinttypes>

#ifdef _WIN32
    #define BASEDIR "C:\\"
#elif __unix__
    #define BASEDIR "/"
#else
    define BASEDIR ""
#endif

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->assertOutputDir();

    // Load monospace font
    QFontDatabase::addApplicationFont("./Inconsolata-Regular.ttf");
    QFontDatabase::addApplicationFont("./Inconsolata-Bold.ttf");

    // Apply font to output log
    QTextDocument *doc = this->ui->output->document();
    QFont font = doc->defaultFont();
    font.setFamily("Inconsolata");
    doc->setDefaultFont(font);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::assertOutputDir()
{
    QFileInfo f(this->ui->inputOutFile->text());
    bool writable = f.isDir() && f.isWritable();

    if(writable)
    {
        this->ui->inputWrite->setEnabled(true);
        this->ui->outputError->setVisible(false);
    }
    else
    {
        this->ui->inputWrite->setEnabled(false);
        this->ui->outputError->setVisible(true);
    }
}

void MainWindow::on_inputBrowse_clicked()
{
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select location"), BASEDIR, QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
    this->ui->inputOutFile->setText(dir);

}

void MainWindow::on_inputOutFile_textChanged(const QString &arg1)
{
       this->assertOutputDir();
}

void MainWindow::on_inputWrite_clicked()
{
    this->ui->centralWidget->setDisabled(true);

    this->ui->output->appendPlainText("--------------------------------------------------");
    this->ui->output->appendPlainText(QString("Starting benchmark: ") + QString::number(this->ui->inputCount->value()) + QString(" chunks at ") + QString::number(this->ui->inputSize->value()) + " MiB each");
    this->ui->output->appendPlainText("--------------------------------------------------");

    std::vector<qint64> writeTimes;
    QElapsedTimer       timer;
    qint64              time        = 0;
    std::string         outfile     = this->ui->inputOutFile->text().toStdString() + "/tmp";
    uint64_t            sizeBytes   = double((this->ui->inputSize->value() * 1024.0) * 1024.0);

    // Allocate chunk data
    char * chunk = new char[sizeBytes];
    this->ui->output->appendPlainText(QString("Allocated chunkdata (actual size is ") + QString::number(sizeBytes) + QString(" bytes)"));
    qApp->processEvents();

    // Write benchmark
    {
        // Open stream and write first byte
        timer.start();
        std::fstream out(outfile.c_str(), std::fstream::out|std::fstream::binary|std::fstream::trunc);
        out.write("\0", 1);
        out.flush();
        out.sync();
        time = timer.nsecsElapsed();

        // Notify and return if we failed
        if(out.fail())
        {
            this->ui->output->appendPlainText("ERROR: Failed to open file for writing!");
            return;
        }
        else
        {
            this->ui->output->appendPlainText(QString::number(double(time) / double(1000000.0)) + QString(" ms \t First byte written"));
        }
        qApp->processEvents();

        // Write chunks
        for(int i = 0; i < this->ui->inputCount->value(); i++)
        {
            out.flush();
            out.sync();

            timer.restart();
            out.write(chunk, sizeBytes);
            out.flush();
            out.sync();
            time = timer.elapsed();

            if(out.fail())
            {
                this->ui->output->appendPlainText("ERROR: Write fail on chunk #" + QString::number(i));
                return;
            }
            else
            {
                double mibs = ((double(sizeBytes)/1024.0)/1024.0) / (double(time) * 0.001);
                this->ui->output->appendPlainText(QString::number(time) + QString(" ms \t Chunk #") + QString::number(i) + QString(" written @ ") + QString::number(mibs, 'f', 2) + QString(" MiB/s"));
            }
            qApp->processEvents();
        }

        // Close writer
        out.close();
    }

    // Delete chunkdata
    this->ui->output->appendPlainText("Deleting chunkdata");
    delete[] chunk;
    qApp->processEvents();

    // Cleanup
    this->ui->output->appendPlainText("Cleaning up");
    QFile f(outfile.c_str());
    if(!f.remove())
    {
        this->ui->output->appendPlainText(QString("Could not remove output file, please remove it manually: ") + QString(outfile.c_str()));
    }
    qApp->processEvents();
    this->ui->output->appendPlainText("");
    this->ui->output->appendPlainText("Ready.");

    this->ui->centralWidget->setDisabled(false);
}

void MainWindow::on_inputSaveLog_clicked()
{
    static QString logFile = QString(BASEDIR) + "hddbench-output.txt";
    logFile = QFileDialog::getSaveFileName(this, tr("Save log"), logFile, tr("Log files (*.txt *.log)"));
    if(logFile.size() == 0)
    {
        return;
    }

    std::fstream f(logFile.toStdString().c_str(), std::fstream::out|std::fstream::trunc);
    if(!f)
    {
       QMessageBox::information(this, "Error", "Failed to open log file for writing");
       f.close();
       return;
    }

    f << this->ui->output->toPlainText().toStdString().c_str();

    if(f.fail())
    {
        QMessageBox::information(this, "Error", "Couldn't write log to file");
    }
    else
    {
        QMessageBox::information(this, "Success", "Log file saved!");
    }
    f.close();
}
