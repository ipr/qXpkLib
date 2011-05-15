#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCodec>
#include <QDateTime>


#include "qxpklib.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_pXpkLib(nullptr)
{
    ui->setupUi(this);
	connect(this, SIGNAL(FileSelection(QString)), this, SLOT(onFileSelected(QString)));
	
	m_pXpkLib = new QXpkLib(this);
	connect(m_pXpkLib, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
	connect(m_pXpkLib, SIGNAL(warning(QString)), this, SLOT(onWarning(QString)));
	connect(m_pXpkLib, SIGNAL(error(QString)), this, SLOT(onError(QString)));
	
	// if file given in command line
	QStringList vCmdLine = QApplication::arguments();
	if (vCmdLine.size() > 1)
	{
		emit FileSelection(vCmdLine[1]);
	}
}

MainWindow::~MainWindow()
{
	delete m_pXpkLib;
	
    delete ui;
}


void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"));
	if (szFile != NULL)
	{
		emit FileSelection(szFile);
	}
}

void MainWindow::onFileSelected(QString szFile)
{
	m_pXpkLib->setInputFile(szFile);
	
	if (m_pXpkLib->xpkUnpack() == true)
	{
		ui->statusBar->showMessage(QString("Unpack done: ").append(szFile));
	}
}

void MainWindow::onMessage(QString szData)
{
	ui->statusBar->showMessage(QString("Message: ").append(szData));
}

void MainWindow::onWarning(QString szData)
{
	ui->statusBar->showMessage(QString("Warning: ").append(szData));
}

void MainWindow::onError(QString szData)
{
	ui->statusBar->showMessage(QString("Error: ").append(szData));
}

void MainWindow::on_actionAbout_triggered()
{
	QTextEdit *pTxt = new QTextEdit(this);
	pTxt->setWindowFlags(Qt::Window); //or Qt::Tool, Qt::Dialog if you like
	pTxt->setReadOnly(true);
	pTxt->append("XpkTool by Ilkka Prusi 2011");
	pTxt->append("");
	pTxt->append("This program is free to use and distribute. No warranties of any kind.");
	pTxt->append("Program uses Qt 4.7.2 under LGPL v. 2.1");
	pTxt->append("");
	pTxt->append("Keyboard shortcuts:");
	pTxt->append("");
	pTxt->append("F = open file");
	pTxt->append("Esc = close");
	pTxt->append("? = about (this dialog)");
	pTxt->append("");
	pTxt->show();
}
