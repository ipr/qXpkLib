#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMap>

namespace Ui {
    class MainWindow;
}

class QXpkLib;
class QTreeWidgetItem;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
	void FileSelection(QString szFile);

protected slots:

	void onMessage(QString szData);
	void onWarning(QString szData);
	void onError(QString szData);
	
private slots:
	void onFileSelected(QString szFile);
	
	void on_actionFile_triggered();
	
	void on_actionDecrunch_triggered();
	
	void on_actionTest_triggered();
	void on_actionAbout_triggered();
	
private:
    Ui::MainWindow *ui;
    QString m_lastPath;

	QString m_szBaseTitle;
	QString m_szCurrentArchive;
	
	QXpkLib *m_pXpkLib;
	
	// keep some archive-info here
	bool m_bIsMultiFileArc;
	bool m_bIsMultiVolArc;
	
	// key: path (without filename)
	// value: top-level item
	//
	QMap<QString, QTreeWidgetItem*> m_PathToItem;
	
	//QString GetPath(const QString &szName);
	bool SplitPathFileName(const QString &szName, QString &szPath, QString &szFile);
	
	void ClearAll();
};

#endif // MAINWINDOW_H
