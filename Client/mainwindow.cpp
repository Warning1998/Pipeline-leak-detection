#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

///转到录音和播放界面
void MainWindow::on_MainRecoderButton_clicked()
{
    RecoderAndPlayerUI.show();
}
