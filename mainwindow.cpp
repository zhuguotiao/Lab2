#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    //显示长度和行号
    statusLabel.setMaximumWidth(150);
    statusLabel.setText("length: "+QString::number(0)+"  lines:   "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusLabel);

    //显示行列
    statusCursorLabel.setMaximumWidth(150);
    statusCursorLabel.setText("Ln: "+QString::number(0)+"  Col:   "+QString::number(1));
    ui->statusbar->addPermanentWidget(&statusCursorLabel);

    //设置作者
    QLabel *author=new QLabel(ui->statusbar);
    author->setText(tr("陈德城"));
    ui->statusbar->addPermanentWidget(author);



}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_actionAbout_triggered()
{
    AboutDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionFind_triggered()
{
    SearchDialog dlg;
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg;
    dlg.exec();
}

