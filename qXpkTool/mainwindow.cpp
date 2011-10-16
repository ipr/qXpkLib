#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QTextEdit>
#include <QTextCodec>
#include <QDateTime>

#include <QTreeWidgetItem>

#include "qxpklib.h"



MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    m_lastPath(),
    m_pXpkLib(nullptr)
{
    ui->setupUi(this);
	m_szBaseTitle = windowTitle();
	connect(this, SIGNAL(FileSelection(QString)), this, SLOT(onFileSelected(QString)));
	
	m_pXpkLib = new QXpkLib(this);
	connect(m_pXpkLib, SIGNAL(message(QString)), this, SLOT(onMessage(QString)));
	connect(m_pXpkLib, SIGNAL(warning(QString)), this, SLOT(onWarning(QString)));
	connect(m_pXpkLib, SIGNAL(error(QString)), this, SLOT(onError(QString)));

	QStringList treeHeaders;
	treeHeaders << "Name" 
			<< "Unpacked size" 
			<< "Packed size" 
			<< "Time" 
			<< "Date" 
			<< "Packing" 
			<< "Comment";
	ui->treeWidget->setColumnCount(treeHeaders.size());
	ui->treeWidget->setHeaderLabels(treeHeaders);

	
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

void MainWindow::onFileSelected(QString szFile)
{
	ClearAll();

	m_pXpkLib->setInputFile(szFile);

	setWindowTitle(m_szBaseTitle + " - " + szArchiveFile);
	m_szCurrentArchive = szArchiveFile;

	QXpkLib::CArchiveInfo info;
	if (m_pXpkLib->xpkInfo(info) == false)
	{
		// should show error message from signal instead
		//ui->statusBar->showMessage("Error retrieving information");
		return;
	}

	QString szMessage;
	szMessage.append(" Total files in archive: ").append(QString::number(info.m_fileList.count()))
			.append(" Total unpacked size: ").append(QString::number(info.m_archiveInfo.m_ulUnpackedSize))
			.append(" Archive file size: ").append(QString::number(info.m_archiveInfo.m_ulFileSize))
			.append(" Packing: ").append(info.m_archiveInfo.m_packing);
	ui->statusBar->showMessage(szMessage);
	
	auto it = info.m_archiveInfo.begin();
	auto itEnd = info.m_archiveInfo.end();
	while (it != itEnd)
	{
		QXpkLib::CEntryInfo &Entry = (*it);
		
		// empty name?
		/*
		if (Entry.m_fileName.length() < 1)
		{
			++it;
			continue;
		}
		*/

		QString szPath;
		QString szFile;
		
		if (SplitPathFileName(Entry.m_fileName, szPath, szFile) == false)
		{
			// no path
			szFile = Entry.m_fileName;
			szPath = "/";
		}
		
		// get top-level item of path
		QTreeWidgetItem *pTopItem = m_PathToItem.value(szPath, nullptr);
		if (pTopItem == nullptr)
		{
			pTopItem = new QTreeWidgetItem((QTreeWidgetItem*)0);
			pTopItem->setText(0, szPath);
			ui->treeWidget->addTopLevelItem(pTopItem);
			m_PathToItem.insert(szPath, pTopItem);
		}

		// add sub-item of file
		QTreeWidgetItem *pSubItem = new QTreeWidgetItem(pTopItem);
		pSubItem->setText(0, szFile);
		pSubItem->setText(1, QString::number(Entry.m_ulUnpackedSize)); // always given
		pSubItem->setText(2, QString::number(Entry.m_ulPackedSize)); // always given
		
		QTime Time(Entry.m_Stamp.time());
		pSubItem->setText(3, Time.toString("hh:mm:ss"));
		
		QDate Date(Entry.m_Stamp.date());
		pSubItem->setText(4, Date.toString("dd-MM-yyyy"));
		
		// packing information
		pSubItem->setText(5, Entry.m_packing);
		
		// file-related comment (if any stored)
		pSubItem->setText(6, Entry.m_szComment);


		pTopItem->addChild(pSubItem);
		++it;
	}
	
	ui->treeWidget->expandAll();
	ui->treeWidget->resizeColumnToContents(0);
	ui->treeWidget->sortByColumn(0);
}


void MainWindow::on_actionFile_triggered()
{
	QString szFile = QFileDialog::getOpenFileName(this, tr("Open file"), m_lastPath);
	if (szFile != NULL)
	{
		// keep path
		//
		// note: fix path-separator if still msdos-style,
		// it's not needed since 1999 for fucks sake..
		//
        szFile.replace('\\', "/"); // fix path-separator
        m_lastPath = szFile.left(szFile.lastIndexOf('/'));
	
		emit FileSelection(szFile);
	}
}


void MainWindow::on_actionDecrunch_triggered()
{
	// if is multi-file archive
	// -> select dest folder dialog
	/*
    QString szDestPath = QFileDialog::getExistingDirectory(this, tr("Select path to extract to.."), szCurrentFilePath);
    if (szDestPath == NULL)
    {
		return;
    }
    */
	
	// if is single file
	// -> select dest file dialog
	
	

	if (m_pXpkLib->xpkUnpack() == true)
	{
		ui->statusBar->showMessage("Extract completed", 10000);
	}
}

void MainWindow::on_actionTest_triggered()
{
	if (m_pXpkLib->xpkTest() == true)
	{
		ui->statusBar->showMessage("Test completed successfully", 10000);
	}
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

bool MainWindow::SplitPathFileName(const QString &szName, QString &szPath, QString &szFile)
{
	QString szFilePath = szName;
	szFilePath.replace('\\', "/");
	int iPos = szFilePath.lastIndexOf('/');
	if (iPos == -1)
	{
		// no path
		return false;
	}

	szFile = szFilePath.right(szFilePath.length() - iPos -1);
	if (szFilePath.at(0) != '/')
	{
		szPath = "/";
	}
	szPath += szFilePath.left(iPos);
	
	// split done
	return true;
}

void MainWindow::ClearAll()
{
	setWindowTitle(m_szBaseTitle);
	
	m_szCurrentArchive.clear();
	m_PathToItem.clear();
	ui->treeWidget->clear();
}
