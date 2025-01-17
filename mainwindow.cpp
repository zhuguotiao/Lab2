#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "aboutdialog.h"
#include "searchdialog.h"
#include "replacedialog.h"
#include "qfiledialog.h"

#include "QColorDialog"
#include"QTextStream"
#include"QMessageBox"
#include"QFontDialog"
#include<QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    //初始化文本框状态
    textChanged=false;
    on_actionNew_triggered();


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


    ui->actionCopy->setEnabled(false);
    ui->actionRedo->setEnabled(false);
    ui->actionUndo->setEnabled(false);
    ui->actionCut->setEnabled(false);
    ui->actionPaste->setEnabled(false);

    QPlainTextEdit::LineWrapMode mode=ui->textEdit->lineWrapMode();
    if(mode==QTextEdit::NoWrap){
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(true);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(false);
    }

    ui->actionToolbar->setChecked(true);
    ui->actionStatusBar->setChecked(true);

    connect(ui->actionLineNumber,SIGNAL(triggered(bool)),ui->textEdit,SLOT(showLineNumberArea(bool)));

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
    SearchDialog dlg(this,ui->textEdit);
    dlg.exec();
}


void MainWindow::on_actionReplace_triggered()
{
    ReplaceDialog dlg(this,ui->textEdit);
    dlg.exec();
}

//新建文件
void MainWindow::on_actionNew_triggered()
{
    if(!userEditConfirmed()){
        return;
    }

    filePath="";

    ui->textEdit->clear();
    this->setWindowTitle(tr("新建文本文件-编辑器"));

    textChanged=false;;

}

//打开文件
void MainWindow::on_actionOpen_triggered()
{
    if(!userEditConfirmed()){
        return;
    }
    QString filename=QFileDialog::getOpenFileName(this,"打开文件",".",tr("Text files(*.txt) ;; All(*.*)"));
    QFile file(filename);
    if(!file.open(QFile::ReadOnly | QFile::Text)){
        QMessageBox::warning(this,"..","打开文件失败");
        return;
    }

    filePath=filename;

    QTextStream in(&file);
    QString text=in.readAll();
    ui->textEdit->insertPlainText(text);
    file.close();

    this->setWindowTitle(QFileInfo(filename).absoluteFilePath());

    textChanged=false;
}

//保存文件
void MainWindow::on_actionSave_triggered()
{
    if(filePath==""){
        QString filename=QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));

        QFile file(filename);

        if(!file.open(QFile::WriteOnly | QFile::Text)){
            QMessageBox::warning(this,"..","保存文件失败");
            return;
        }
        file.close();
        filePath=filename;
    }


    QFile file(filePath);
    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"..","保存文件失败");
        return;
    }

    QTextStream out(&file);
    QString text=ui->textEdit->toPlainText();

    out<<text;
    file.flush();
    file.close();

    this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChanged=false;
}

//另存文件
void MainWindow::on_actionSaveAs_triggered()
{
    QString filename=QFileDialog::getSaveFileName(this,"保存文件",".",tr("Text files(*.txt)"));

    QFile file(filename);

    if(!file.open(QFile::WriteOnly | QFile::Text)){
        QMessageBox::warning(this,"..","保存文件失败");
        return;
    }

    filePath=filename;

    QTextStream out(&file);
    QString text=ui->textEdit->toPlainText();

    out<<text;
    file.flush();
    file.close();

     this->setWindowTitle(QFileInfo(filePath).absoluteFilePath());

    textChanged=false;

}


void MainWindow::on_textEdit_textChanged()
{
    if(!textChanged){
        this->setWindowTitle("*"+this->windowTitle());
        textChanged=true;
    }

    statusLabel.setText("length: "+QString::number(ui->textEdit->toPlainText().length())
                        +"  lines:   "+QString::number(ui->textEdit->document()->lineCount()));


}

bool MainWindow::userEditConfirmed()
{



    if(textChanged){
         QString path=filePath!=""?filePath:"无标题.txt";
        QMessageBox msg(this);
        msg.setIcon(QMessageBox::Question);
        msg.setWindowTitle("");
        msg.setWindowFlag(Qt::Drawer);
        msg.setText(QString("是否将更改保存到\n")+"\""+path+"\"?");
        msg.setStandardButtons(QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        //判断用户的选择
        int r=msg.exec();
        switch (r) {
        case QMessageBox::Yes:
            on_actionSave_triggered();
            break;
        case QMessageBox::No:
            textChanged=false;
            break;
        case QMessageBox::Cancel:
            break;
        }

    }

    return true;
}

//恢复操作
void MainWindow::on_actionUndo_triggered()
{
    ui->textEdit->undo();
}

//剪切操作
void MainWindow::on_actionCut_triggered()
{
    ui->textEdit->cut();
    ui->actionPaste->setEnabled(true);
}

//复制操作
void MainWindow::on_actionCopy_triggered()
{
    ui->textEdit->copy();
    ui->actionPaste->setEnabled(true);
}

//粘贴操作
void MainWindow::on_actionPaste_triggered()
{
    ui->textEdit->paste();
}

//撤销操作
void MainWindow::on_actionRedo_triggered()
{
    ui->textEdit->redo();
}

//判断是否恢复
void MainWindow::on_textEdit_undoAvailable(bool b)
{
    ui->actionUndo->setEnabled(b);
}
//判断是否执行复制和剪切
void MainWindow::on_textEdit_copyAvailable(bool b)
{
    ui->actionCopy->setEnabled(b);
    ui->actionCut->setEnabled(b);

}

//判断是否执行撤销
void MainWindow::on_textEdit_redoAvailable(bool b)
{
    ui->actionRedo->setEnabled(b);
}





void MainWindow::on_actionFontBackgroundColor_triggered()
{
    // 打开颜色选择对话框
        QColor color = QColorDialog::getColor(Qt::white, this, "选择背景颜色");

        // 如果选择的颜色有效
        if (color.isValid()) {
            // 获取当前光标
            QTextCursor cursor = ui->textEdit->textCursor();

            // 如果没有选中任何文本，选择整个文本
            if (cursor.selectedText().isEmpty()) {
                cursor.select(QTextCursor::Document);
            }

            // 设置背景颜色
            QTextCharFormat format;
            format.setBackground(color);
            cursor.mergeCharFormat(format);

            // 更新文本编辑器的内容
            ui->textEdit->setTextCursor(cursor);
        }
}


void MainWindow::on_actionBackgroundColor_triggered()
{
    QColor color=QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit {background-color:%1}").arg(color.name()));
    }
}


void MainWindow::on_actionFontColor_triggered()
{
    QColor color=QColorDialog::getColor(Qt::black,this,"选择颜色");
    if(color.isValid()){
        ui->textEdit->setStyleSheet(QString("QPlainTextEdit {color:%1}").arg(color.name()));
    }
}




void MainWindow::on_actionWrap_triggered()
{
    QPlainTextEdit::LineWrapMode mode=ui->textEdit->lineWrapMode();
    if(mode==QTextEdit::NoWrap){
        ui->textEdit->setLineWrapMode(QPlainTextEdit::WidgetWidth);
        ui->actionWrap->setChecked(true);
    }else{
        ui->textEdit->setLineWrapMode(QPlainTextEdit::NoWrap);
        ui->actionWrap->setChecked(false);
    }
}




void MainWindow::on_actionFont_triggered()
{
    bool ok=false;
    QFont font=QFontDialog::getFont(&ok,this);
    if(ok){
        ui->textEdit->setFont(font);
    }
}


void MainWindow::on_actionToolbar_triggered()
{
    bool visible=ui->toolBar->isVisible();
    ui->toolBar->setVisible(!visible);
    ui->actionToolbar->setChecked(!visible);
}


void MainWindow::on_actionStatusBar_triggered()
{
    bool visible=ui->statusbar->isVisible();
    ui->statusbar->setVisible(!visible);
    ui->actionStatusBar->setChecked(!visible);
}


void MainWindow::on_actionSelectAll_triggered()
{
    ui->textEdit->selectAll();
}


void MainWindow::on_action_Exit_triggered()
{
    if(userEditConfirmed()){
        exit(0);
    }
}


void MainWindow::on_textEdit_cursorPositionChanged()
{
    int pos=ui->textEdit->textCursor().position();
    int col=0;
    int ln=0;
    int flg=-1;
    QString text=ui->textEdit->toPlainText();
    for(int i=0;i<pos;i++){
        if(text[i]=='\n'){
            ln++;
            flg=i;
        }
    }
    flg++;
    col=pos-flg;
    statusCursorLabel.setText("Ln: "+QString::number(ln)+"  Col:   "+QString::number(col));

}







