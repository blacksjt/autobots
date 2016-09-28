#ifndef AUTO_DIRVERS_FATIE_H
#define AUTO_DIRVERS_FATIE_H

#include <QtWidgets/QMainWindow>
#include "ui_auto_dirvers_fatie.h"
#include "network.h"
#include "driver_work.h"

struct AccountParam
{
public:
	AccountParam(QString name, QString password)
	{
		_id = name;
		_password = password;
	}

public:
	QString _id;
	QString _password;
};

typedef QList<AccountParam> AccountList;

class auto_dirvers_fatie : public QMainWindow
{
	Q_OBJECT

public:
	auto_dirvers_fatie(QWidget *parent = 0);
	~auto_dirvers_fatie();
	void initializeConncetions();

private slots:
	void onStart();
	void onStop();
	void onActImportComment();
	void onActClearComments();
	void onActFromTxt();
	void onActClearAccounts();
	void onInputUrl1(const QString & text);
	void onInputUrl2(const QString & text);
	void onInputUrl3(const QString & text);
	void onChanged(const QString& text);
private:
	void emitMsgBar(const QString& msg); // 输出信息到状态栏
	bool updateData(); // 提取界面数据到
	void initializeControls();

	void AutoFatie();
	void WaitforSeconds(int nseconds);
	bool GetContent();

	bool GetFatieStatus(const QByteArray& rp_data);
	bool getCode(QString& vcode, QString& code_sign);
	bool Login(const QString& _id, const QString& _password, int depth);
	void logout();
	bool isAllFinished();
	QString GetMatchedText(const QString& text);
	void intialVPN();
	void SaveIniSettings();
private:
	QList<DriverWork> m_work_list;

	Ui::auto_dirvers_fatieClass ui;
	toutiao_network network;
	bool m_control_stop;

	int m_account_order;
	AccountList m_account_list;
	QList<QTableWidgetItem*> m_account_row_list;

	int m_interval;
};

#endif // AUTO_DIRVERS_FATIE_H
