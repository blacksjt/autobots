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
};

typedef QList<DeviceParam> DeviceParamList;

struct SinaData
{
public:
	QString _servertime;
	QString _pcid;
	QString _nonce;
	QString _pubkey;
	QString _rsakv;
	QString _ticket;
	bool    _showpin;
};

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
	//void onPostIdReday(QString);
	void onAddCommentID();
	//void onCsrfTokenReday(QString str);
	//void onLoginsucceed(QString id);
	void onActFromTxt();
	void onActImportComment();
	void onActClearComments();
	void onActClearAccounts();
	//void onClientIdReday(QString _id);
	//void onFatieSucceed(QString comment, double id);
private:
	bool DoPostFatie(const QString& content);
	//bool GetContent();
	bool RequestForSina();
	void CodeCheckForRenren();
	bool AuthorBySina(const QString& name, const QString& password);
	void UpdateData();
	void initialize();
	void initialDevices();
	void Logout();
	bool CheckInput();
	bool GetCsrfToken(const QByteArray& arr);
	bool ProcessRedirectSSL(const QString& str);
	bool ProcessRedirectGet(const QString& str);
	bool GetPostId(const QByteArray& arr);
	bool ProcessRedirectLoginGet(const QString& str);
	bool ProcessRedirectLoginGet2(const QString& str);
	int ProcessRedirectLoginGetTemp(const QString& str);
	int ProcessRedirectLoginGetTemp2(const QString& str);
	bool GetFatieStatus(const QByteArray& byte_arr);
	void FatieSucceed(const QString& comment, const QString& id);
	bool NeedValidateCode(const QString& name, QString& vcode, QString& code_sign);

	void WaitforSeconds(int nseconds);

	bool PreLoginSina(const QString& name, SinaData& data, QString& vcode, QString& code_sign);
	bool GetPreLoginResult(const QByteArray& str, SinaData& data);
	bool GetLoginResult(const QByteArray& str, SinaData& data, const QString& code_sign);
	bool LoginSina(SinaData& sina_data, const QString& name,
		const QString& password, const QString& vcode, const QString& code_sign);
	bool AuthorizeSina(const SinaData& sina_data);
	bool AuthorizeSina2(const SinaData & sina_data);
	bool GetUserId(const QByteArray& arr);
	
private:
	Ui::autobots_toutiaoClass ui;
	toutiao_network* network;
	bool control_status;
	QString m_csrf_token;
	bool m_code_online;

	QString m_client_id;
	QString m_appkey;
	QString m_state;
	QString m_verifyToken;
	QString	m_uid;
private:
	QString m_url;
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
