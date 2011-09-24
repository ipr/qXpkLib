#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

namespace Ui {
    class MainWindow;
}

class QXpkLib;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
	void FileSelection(QString szFile);

	
private slots:
	void on_actionFile_triggered();
	void onFileSelected(QString szFile);
	
	void onMessage(QString szData);
	void onWarning(QString szData);
	void onError(QString szData);
	
	void on_actionAbout_triggered();
	
	void on_actionDecrunch_triggered();
	
	void on_actionTest_triggered();
	
private:
    Ui::MainWindow *ui;
    QString m_lastPath;
	
	QXpkLib *m_pXpkLib;
};

#endif // MAINWINDOW_H
