///////////////////////////////////
//
// Library entry-point/interface.
//

#ifndef QXPKLIB_H
#define QXPKLIB_H

#include "qXpkLib_global.h"

#include <QObject>
#include <QByteArray>


//// fwd. decl.


class CXpkMaster;
//class CxpkProgress; // TODO: see about what to do..
struct XpkProgress;


class QXPKLIBSHARED_EXPORT QXpkLib : public QObject
{
	Q_OBJECT

private:
	CXpkMaster *m_pMaster;
	
	// current pack/unpack related item
	//TagItem *m_pItem;
	
	// status (display also or just internal?)
	XpkProgress *m_pProgress;
	
protected:
	void PrepareMaster();
	
public:
    QXpkLib(QObject *parent = 0);
	virtual ~QXpkLib();

	// pack/unpack to/from given input&output
	bool xpkPack();
	bool xpkUnpack();
	
signals:
	void message(QString);
	void warning(QString);
	void error(QString);

	//void status(type?);
	
public slots:
	
	// set filename and path
	void setInputFile(QString &szFile);
	void setOutputFile(QString &szFile);
	
	// if user wants some buffer-to-buffer handling?
	void setInputBuffer(QByteArray *pArray);
	void setOutputBuffer(QByteArray *pArray);

	
};

#endif // QXPKLIB_H
