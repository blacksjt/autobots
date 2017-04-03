#ifndef AUTOBOTS_TOUTIAO_H
#define AUTOBOTS_TOUTIAO_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_autobots_toutiao.h"
#include "toutiao_network.h"

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

struct DeviceParam
{
public:
	DeviceParam(QString uuid, QString did, QString agent, QString device_type)
	{
		_uuid = uuid;
		_did = did;
		_useragent = agent;
		_device_type = device_type;
	}

	DeviceParam()
	{

	}
public:
	QString _uuid;
	QString _did;
	QString _useragent;
	QString _device_type;
	QString _idfa;
	QString _openudid;
	QString _iid;
};

typedef QList<DeviceParam> DeviceParamList;


class autobots_toutiao : public QMainWindow
{
	Q_OBJECT

public:
	autobots_toutiao(QWidget *parent = 0);
	~autobots_toutiao();

private:

	private slots :
		void onStart();
	void onPause();
	void onAddCommentID();
	void onActFromTxt();
	void onActImportComment();
	void onActClearComments();
	void onActClearAccounts();
private:
	bool DoPostFatie(const QString& content);

	bool RequestForRenren();
	void CodeCheckForRenren();
	bool LoginTTByMobile(const QString& name, const QString& password);
	bool LoginTTByMobile(const QString& name, const QString& password, const QString& captcha);
	void UpdateData();
	void initialize();
	void initialDevices();
	void Logout();
	bool CheckInput();
	bool GetFatieStatus(const QByteArray& byte_arr);
	void FatieSucceed(const QString& comment, const QString& id);
	bool NeedValidateCode(const QString& name, QString& vcode, QString& code_sign);

	void WaitforSeconds(int nseconds);

	// 0�ɹ�  -1 ��֤������ -2 ��������
	int GetLoginRst(const QByteArray& data, QString& captcha);

private:
	Ui::autobots_toutiaoClass ui;
	toutiao_network* network;
	bool control_status;
	QString m_client_id;
	QString m_post_id;
	QString m_csrf_token;
	QString m_state_id;
	bool m_code_online;
private:
	//QString m_url;
	QString m_news_id;
	QString m_group_id;
	int m_interval;
	QStringList m_comment_list;
	QList<QListWidgetItem*> m_comment_item_list;
	AccountList m_account_list;
	QList<int> m_account_row_list;
	int m_account_order;

	DeviceParamList m_devices_list;
	size_t m_device_order;
};

#endif // AUTOBOTS_TOUTIAO_H
