#include "autobots_toutiao.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "QColor"
#include "html_parse_utils.h"
#include <QJsonDocument>
#include "code_dlg.h"
#include "vlidatecodeonline.h"
#include <QDateTime>
#include <QUuid>
#include <time.h>
#include "sina_encrypt.h"

QString GetTimeStr()
{
	qint64 n = QDateTime::currentMSecsSinceEpoch();

	return QString::number(n);
}

QString GetUuid()
{
	QString uuid = QUuid::createUuid().toString();
	uuid.remove('{');
	uuid.remove('}');
	return uuid;
}

QString GetDeviceType()
{
	QStringList type;
	type << "iphone 5" << "iphone 6" << "iphone 5s" << "iphone 6s" << "iphone 7"
		<< "iphone 6 plus" << "iphone 6s plus" << "iphone 7 plus";
	size_t n = type.size();
	int m = rand() % n;
	return type[m];
}

QString GetDid()
{
	srand(time(NULL));
	double i = (double)rand() / RAND_MAX;
	qlonglong n = 3111111111 + 888888888 * i;
	return QString::number(n);
}

const int TIMEOUT = 20 * 1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
	: control_status(true), QMainWindow(parent),
	m_account_order(0),m_device_order(0),
	m_client_id("394e2173327e4ead8302dc27f4ae8879"), network(NULL)
{
	ui.setupUi(this);

	initialize();

	connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

	connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

	connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

	connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

	connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

	connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

	connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));
}

autobots_toutiao::~autobots_toutiao()
{
	if (network != NULL)
	{
		network->deleteLater();
		network = NULL;
	}
}

void autobots_toutiao::onStart()
{
	// 检查输入
	if (!CheckInput())
	{
		return;
	}

	// 更新界面输入
	UpdateData();

	//   if (m_account_list.size() < m_comment_list.size())
	//   {
	//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("用户账户太少")); 
	//     return;
	//   }

	control_status = true;

	// 循环发帖
	for (int i = 0; i < m_comment_list.size(); ++i)
	{
		if (!control_status)
		{
			break;
		}

		if (m_account_order >= m_account_list.size())
		{
			ui.lineEdit_msg->setText(QStringLiteral("账号不够用了"));
			return;
		}

		// 尝试登陆
		bool login_status = false;
		while (m_account_order < m_account_list.size())
		{
			if (network != NULL)
			{
				network->deleteLater();
				network = NULL;
			}
			network = new toutiao_network;
// 			if (!RequestForRenren())
// 			{
// 				ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
// 				cookie->deleteLater();
// 				continue;
// 			}
			if (!RequestForSina())
			{
				ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
				continue;
			}

			AccountParam ac = m_account_list.at(m_account_order);

			if (!AuthorBySina(ac._id, ac._password))
			{
				ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255, 0, 0, 180));
				m_account_order++;
				continue;
			}
			else
			{
				ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0, 255, 0, 180));
				m_account_order++;
				login_status = true;
				break;
			}
		}

		if (!login_status)
		{
			ui.lineEdit_msg->setText(QStringLiteral("账号登陆不成功，无法继续"));
			return;
		}

		QElapsedTimer t2;
		t2.start();
		while (t2.elapsed()<1000)
			QCoreApplication::processEvents();

		if (DoPostFatie(m_comment_list[i]))
		{
			m_comment_item_list[i]->setCheckState(Qt::Checked);
			//m_comment_item_list[i]->setBackgroundColor(QColor(0,255,0, 180));
		}
		else
		{
			continue;
		}

		t2.restart();
		while (t2.elapsed()<1000)
			QCoreApplication::processEvents();

		//Logout();
		m_device_order++;

		if (m_device_order >= m_devices_list.size())
		{
			m_device_order = 0;
		}

		QString msg;
		msg.setNum(i + 1);

		ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("次完成"));

		t2.restart();
		while (t2.elapsed()< m_interval * 1000)  // 时间间隔
		{
			ui.lineEdit_msg->setText(QString::number(t2.elapsed()));
			QCoreApplication::processEvents();
		}
	}

	ui.lineEdit_msg->setText(QStringLiteral("已完成"));
}

void autobots_toutiao::onPause()
{
	control_status = false;
	ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool autobots_toutiao::DoPostFatie(const QString& content)
{

	//https://is.snssdk.com/2/data/post_message/?version_code=5.8.3&app_name=news_article&vid=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&device_id=3135986566&channel=App%20Store&resolution=750*1334&aid=13&ab_version=83098,79288,87751,87331,85045,86854,86884,87032,86738,31650,87244,82679,87835,87830,87494,87036,87629&ab_feature=z2&ab_group=z2&openudid=0d919477efbefb99dfe7a02a2df34d9127ecc947&live_sdk_version=1.3.0&idfv=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=6088961915&ab_client=a1,f2,f7,e1&device_type=iPhone%206&idfa=86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A
	QString str_url1 = QString("https://is.snssdk.com/2/data/post_message/?version_code=5.8.3&app_name=news_article&vid=%1&device_id=%2&channel=App%20Store&resolution=750*1334&aid=13&ab_version=83098,79288,87751,87331,85045,86854,86884,87032,86738,31650,87244,82679,87835,87830,87494,87036,87629&ab_feature=z2&ab_group=z2&openudid=0d919477efbefb99dfe7a02a2df34d9127ecc947&live_sdk_version=1.3.0&idfv=%3&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=6088961915&ab_client=a1,f2,f7,e1&device_type=%4&idfa=86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A")
		.arg(m_devices_list[m_device_order]._uuid).arg(m_devices_list[m_device_order]._did).arg(m_devices_list[m_device_order]._uuid).arg(m_devices_list[m_device_order]._device_type);

	QUrl url1;
	url1.setUrl(str_url1);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Proxy - Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
	header_list.push_back(HttpParamItem("Host", "is.snssdk.com"));
	header_list.push_back(HttpParamItem("Referer", m_url));
	header_list.push_back(HttpParamItem("tt-request-time", GetTimeStr()));
	//header_list.push_back(HttpParamItem("User-Agent", "News/5.8.3 (iPhone; iOS 9.3.5; Scale/2.00)"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("aggr_type", "1"));
	post_data.push_back(HttpParamItem("comment_duration", "20745"));
	post_data.push_back(HttpParamItem("group_id", m_group_id));
	post_data.push_back(HttpParamItem("is_comment", "1"));
	post_data.push_back(HttpParamItem("item_id", m_news_id));
	post_data.push_back(HttpParamItem("platform", ""));
	post_data.push_back(HttpParamItem("read_pct", "4"));
	post_data.push_back(HttpParamItem("staytime_ms", "5225.455"));
	post_data.push_back(HttpParamItem("text", content));
	post_data.push_back(HttpParamItem("zz", "0"));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
		int n = statusCodeV.toInt();
		QString ss = reply->errorString();
		ui.lineEdit_msg->setText(ss);
		reply->deleteLater();
		return false;
	}

	QByteArray rp_data = reply->readAll();

	QVariant content_type = reply->header(QNetworkRequest::ContentTypeHeader);

	QString type = content_type.toString();

	if (type.contains("json"))
	{
		bool res = GetFatieStatus(rp_data);
		if (!res)
		{
			QString msg = rp_data;
			ui.lineEdit_msg->setText(msg);
		}

		return res;
	}
	else
	{
		return false;
	}

}

// bool autobots_toutiao::GetContent()
// {
// 	QString str_url_1 = m_url;
// 
// 	QUrl url_1(str_url_1);
// 	url_1.setUrl(str_url_1);
// 	HttpParamList header_list1;
// 	header_list1.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
// 	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
// 	//  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
// 	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
// 	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
// 
// 	header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
// 	//header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
// 	header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
// 	//  header_list1.push_back(HttpParamItem("Referer", "http://www.toutiao.com/"));
// 	header_list1.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (iPhone; CPU iPhone OS 9_3_5 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Mobile/13G36 NewsArticle/5.8.3.2 JsSdk/2.0 NetType/WIFI (News 5.8.3 9.300000)"));
// 
// 	QNetworkReply* rp = network->GetRequest(url_1, header_list1);
// 
// 	QTime _t;
// 	_t.start();
// 
// 	bool _timeout = false;
// 
// 	while (rp && !rp->isFinished())
// 	{
// 		QCoreApplication::processEvents();
// 		if (_t.elapsed() >= TIMEOUT) {
// 			_timeout = true;
// 			break;
// 		}
// 	}
// 
// 	QVariant statusCodeV = rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);
// 	int n = statusCodeV.toInt();
// 
// 	if (rp == NULL || (rp->error() != QNetworkReply::NoError) || _timeout)
// 	{
// 		QVariant statusCodeV = rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);
// 		int n = statusCodeV.toInt();
// 
// 		QString ss = rp->errorString();
// 		ui.lineEdit_msg->setText(ss);
// 		rp->deleteLater();
// 		return false;
// 	}
// 
// 	bool res = GetCsrfToken(rp->readAll());
// 
// 	if (rp != NULL)
// 		rp->deleteLater();
// 
// 	return  res;
// }

bool autobots_toutiao::RequestForSina()
{
	QString str_url1 = QString("http://isub.snssdk.com/2/auth/login/v2/?platform=sina_weibo&version_code=6.0.3&uid_type=14&app_name=news_article&vid=%1&device_id=%2&channel=App%20Store&resolution=750*1334&aid=13&ab_version=117915%2C112577%2C117600%2C101786%2C101533%2C117843%2C110341%2C113607%2C115065%2C113115%2C106784%2C113608%2C101558%2C105475%2C117715%2C105610%2C115771%2C105826%2C112578%2C115966%2C115570%2C116615%2C31642%2C116158%2C114338&ab_feature=z2&ab_group=z2&openudid=0d919477efbefb99dfe7a02a2df34d9127ecc947&live_sdk_version=1.6.5&idfv=%3&ac=WIFI&os_version=10.3&ssmix=a&device_platform=iphone&iid=9029545108&ab_client=a1%2Cf2%2Cf7%2Ce1&device_type=%4&idfa=86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A")
		.arg(m_devices_list[m_device_order]._uuid).arg(m_devices_list[m_device_order]._did).arg(m_devices_list[m_device_order]._uuid).arg(m_devices_list[m_device_order]._device_type);

	QUrl url1(str_url1);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "isub.snssdk.com"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(url1, header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectSSL(str);
	}
	else
	{
		return false;
	}

}

void autobots_toutiao::CodeCheckForRenren()
{

}

bool autobots_toutiao::AuthorBySina(const QString& name, const QString& password)
{
	//1.检验验证码
	QString vcode, code_sign;
	SinaData sina_data;
	if (!PreLoginSina(name, sina_data, vcode, code_sign))
	{
		ui.lineEdit_msg->setText(QStringLiteral("获取验证码失败"));
		return false;
	}

	if (sina_data._showpin && vcode.isEmpty())
	{
		return false;
	}

	// 2.登陆
	if (!LoginSina(sina_data, name, password, vcode, code_sign))
	{
		ui.lineEdit_msg->setText(QStringLiteral("登录失败"));
		return false;
	}

	// 3 授权
	if (!AuthorizeSina(sina_data))
	{
		// 再次尝试
		if (!AuthorizeSina(sina_data))
		{
			ui.lineEdit_msg->setText(QStringLiteral("授权失败"));
			return false;
		}
	}
	return true;
}


void autobots_toutiao::onAddCommentID()
{

	QString id = ui.lineEdit_comment_id->text();

	ui.lineEdit_comment_id->setText("");

	ui.listWidget_comment_id->addItem(id);

}

void autobots_toutiao::UpdateData()
{
	m_url = ui.lineEdit_url->text();
	m_news_id = ui.lineEdit_page_id->text();
	m_group_id = ui.lineEdit_comment_group->text();
	m_comment_list.clear();
	m_account_list.clear();
	m_comment_item_list.clear();
	m_account_row_list.clear();
	m_code_online = ui.checkBox_code_online->checkState() == Qt::Checked;
	m_interval = ui.spinBox_interval->value();

	int n = ui.listWidget_comment_id->count();

	for (int i = 0; i < n; ++i)
	{
		QListWidgetItem* item = ui.listWidget_comment_id->item(i);

		if (item->checkState() != Qt::Checked)
		{
			QString text = item->text();
			m_comment_list.push_back(text);
			m_comment_item_list.push_back(item);
		}
	}

	n = ui.tableWidget_account_id->rowCount();

	for (int i = 0; i < n; ++i)
	{
		QString name = ui.tableWidget_account_id->item(i, 0)->text();
		QString password = ui.tableWidget_account_id->item(i, 1)->text();
		m_account_list.push_back(AccountParam(name, password));
		m_account_row_list.push_back(i);
	}

}

void autobots_toutiao::initialize()
{
	initialDevices();
	ui.tableWidget_account_id->setColumnWidth(0, 200);
	ui.tableWidget_account_id->setColumnWidth(1, 150);

	m_client_id = "2504490989";
	m_appkey = "42iQjj";
	m_state = "f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D";

	m_code_online = true;
}

void autobots_toutiao::initialDevices()
{
	for (size_t i = 0; i < 10; i++)
	{
		DeviceParam dev;
		dev._uuid = GetUuid();
		dev._did = GetDid();
		dev._useragent = "Mozilla / 5.0 (iPhone; CPU iPhone OS 9_3_5 like Mac OS X) AppleWebKit / 601.1.46 (KHTML, like Gecko) Mobile / 13G36 NewsArticle / 5.8.3.2 JsSdk / 2.0 NetType / WIFI(News 5.8.3 9.300000)";
		dev._device_type = GetDeviceType();
		m_devices_list.push_back(dev);
	}
}

void autobots_toutiao::onActFromTxt()
{
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)"));
	if (path.isEmpty())  //判断用户是否选择了文件
	{
		//emitLogMessage(QStringLiteral("取消打开文件"));
		return;
	}

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, "提示", "无法打开文件");
		return;
	}

	QTextStream text_stream(&file);
	//QString str = text_stream.readAll();
	int n_row = 0;
	ui.tableWidget_account_id->clearContents();
	ui.tableWidget_account_id->setRowCount(0);
	m_account_list.clear();
	m_account_order = 0;
	m_account_row_list.clear();

	while (!text_stream.atEnd())
	{
		QString line_str = text_stream.readLine();

		QStringList str_list = line_str.split("----");

		if (str_list.size() >= 2)
		{
			ui.tableWidget_account_id->setRowCount(ui.tableWidget_account_id->rowCount() + 1);
			QString name = str_list[0];
			QString password = str_list[1];
			QTableWidgetItem* item = new QTableWidgetItem(name);
			ui.tableWidget_account_id->setItem(n_row, 0, item);
			item = new QTableWidgetItem(password);
			ui.tableWidget_account_id->setItem(n_row, 1, item);
			n_row++;
		}
	}

	file.close();
}

void autobots_toutiao::Logout()
{
	QString str_url_1 = "http://web.toutiao.com/auth/logout/";

	QUrl url_1(str_url_1);

	HttpParamList header_list1;
	header_list1.push_back(HttpParamItem("Accept", "application/json, text/javascript, */*; q=0.01"));
	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

	//header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
	////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
	header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
	header_list1.push_back(HttpParamItem("Referer", m_url));
	header_list1.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply_1 = network->GetRequest(url_1, header_list1);

	QTime _t;
	_t.start();

	while (reply_1 && !reply_1->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= 2000) {
			break;
		}
	}
}

void autobots_toutiao::onActImportComment()
{
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)"));
	if (path.isEmpty())  //判断用户是否选择了文件
	{
		//emitLogMessage(QStringLiteral("取消打开文件"));
		return;
	}

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, "提示", "无法打开文件");
		return;
	}

	QTextStream text_stream(&file);
	//QString str = text_stream.readAll();
	int n_row = 0;
	ui.listWidget_comment_id->clear();
	m_comment_item_list.clear();
	m_comment_list.clear();

	while (!text_stream.atEnd())
	{
		QString line_str = text_stream.readLine();
		ui.listWidget_comment_id->addItem(new QListWidgetItem(line_str));
	}

	file.close();
}

void autobots_toutiao::onActClearComments()
{
	ui.listWidget_comment_id->clear();
	m_comment_list.clear();
	m_comment_item_list.clear();
}

void autobots_toutiao::onActClearAccounts()
{
	ui.tableWidget_account_id->clearContents();
	ui.tableWidget_account_id->setRowCount(0);
	m_account_order = 0;
	m_account_list.clear();
	m_account_row_list.clear();
}

bool autobots_toutiao::CheckInput()
{
	if (ui.lineEdit_url->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入"));
		return false;
	}

	if (ui.lineEdit_page_id->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入"));
		return false;
	}

	if (ui.listWidget_comment_id->count() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID"));
		return false;
	}

	if (ui.tableWidget_account_id->rowCount() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请导入账号"));
		return false;
	}

	return true;
}

void autobots_toutiao::FatieSucceed(const QString& comment, const QString& id)
{
	//   QList<QListWidgetItem*> items = ui.listWidget_comment_id->findItems(comment, Qt::MatchExactly);
	// 
	//   if (items.size() > 0 && items[0] != NULL)
	//   {
	//     items[0]->setBackgroundColor(QColor(0,255,0, 180));
	//   }
	ui.lineEdit_msg->setText(comment + QStringLiteral(":成功"));

	ui.textBrowser_commentid->append(id);
}

bool autobots_toutiao::GetCsrfToken(const QByteArray& rp_data)
{
	HtmlParseUtils html_parse(rp_data.data(), rp_data.length());

	// "查找csrftoken"
	GumboNode*  node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");

	if (node != NULL)
	{
		std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
		m_csrf_token = QString::fromStdString(csrf_token);
		return true;
	}

	return false;
}

bool autobots_toutiao::ProcessRedirectSSL(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Proxy-Connection", "keep-alive"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	if (!str.contains("https://", Qt::CaseInsensitive))
	{
		return false;
	}

	QNetworkReply* reply = network->GetRequest_ssl(QUrl(str), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	bool res = false;

	if (n != 200)
	{
		res = false;
	}
	else
	{
		QByteArray data = reply->readAll();

		res = GetPostId(data);
	}

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}

bool autobots_toutiao::ProcessRedirectGet(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply->readAll();

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return GetPostId(data);

}

bool autobots_toutiao::GetPostId(const QByteArray& arr)
{
	HtmlParseUtils html_parse(arr.data(), arr.length());

	// "查找post_from_id"
	GumboNode* node = html_parse.FirstElementNode("name", "client_id");

	if (node != NULL)
	{
		std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString str = QString::fromStdString(post_from_id);

		m_client_id = str;
	}

	// "查找client_id"
	node = html_parse.FirstElementNode("name", "client_id");

	if (node != NULL)
	{
		std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

		m_client_id = QString::fromStdString(client_id);
	}

	// "查找state"
	node = html_parse.FirstElementNode("name", "state");

	if (node != NULL)
	{
		std::string state_id = HtmlParseUtils::GetAttributeValue(node, "value");

		m_state = QString::fromStdString(state_id);
	}

	if (m_client_id.isEmpty() || m_client_id.isEmpty())
	{
		return false;
	}

	return true;
}

// 0 正常，-1 未知错误， -2 验证码错误
bool autobots_toutiao::ProcessRedirectLoginGet(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
	header_list.push_back(HttpParamItem("Host", "api.snssdk.com"));
	header_list.push_back(HttpParamItem("Connection", "keep-alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN,zh;q=0.8"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		if (reply != NULL)
		{
			QString t = reply->errorString();

			int n = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

			t = reply->readAll();
			//reply->deleteLater();
			return ProcessRedirectLoginGetTemp(str);
		}
		else
		{
			return false;
		}
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();
	bool res = false;
	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();
		QString r = reply->readAll();
		QString str2 = red_url.toString();

		if (str2.contains("session_key="))
		{
			res = true;
		}
		else
		{
			res = ProcessRedirectLoginGet2(str2);
		}

		
	}
	else if (n == 302)
	{
		QString str2 = reply->readAll();
		if (str2.contains(QStringLiteral("验证码错误")))
		{
			res = false;
		}
	}
	else if (n == 500)
	{
		// 找不到页面
		res = ProcessRedirectLoginGetTemp(str);
	}
	else
	{
		QString t = reply->readAll();
	}

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}

bool autobots_toutiao::ProcessRedirectLoginGet2(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply->readAll();

	QString temp_s = data;
	if (temp_s.contains("\"message\": \"success\""))
	{
		return true;
	}

	return false;
}

// 0 正常，-1 未知错误， -2 验证码错误
int autobots_toutiao::ProcessRedirectLoginGetTemp(const QString& str)
{
	WaitforSeconds(2);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
	header_list.push_back(HttpParamItem("Host", "api.snssdk.com"));
	header_list.push_back(HttpParamItem("Referer", "http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
	//header_list.push_back(HttpParamItem("",""));
	header_list.push_back(HttpParamItem("Connection", "keep-alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN,zh;q=0.8"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || _timeout)
	{

		reply->deleteLater();
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int n = statusCodeV.toInt();

	if ((reply->error() != QNetworkReply::NoError) && n != 500)
	{
		QString t = reply->errorString();
		return false;
	}

	bool res = false;
	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str2 = red_url.toString();

		res = ProcessRedirectLoginGet2(str2);
	}
	else if (n == 302)
	{
		QString str2 = reply->readAll();
		if (str2.contains(QStringLiteral("验证码错误")))
		{
			res = false;
		}
	}
	else if (n == 500)
	{
		// 找不到页面
		res = ProcessRedirectLoginGetTemp2(str);
	}

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}

int autobots_toutiao::ProcessRedirectLoginGetTemp2(const QString& str)
{
	//http://api.snssdk.com/auth/login_success/?code=vM988uwAJr91qLN7Yh4tKCxw4alqNDDz&state=renren_sns__0____toutiao____2__0__24
	int pos1 = str.indexOf("code=");
	int pos2 = str.indexOf("&", pos1);
	QString t = str.mid(pos1, pos2 - pos1);
	QString s_url = "http://toutiao.com/auth/connected/?state=renren_sns__0____toutiao____2__0__24&" + t;

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
	header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
	//header_list.push_back(HttpParamItem("Referer","http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
	//header_list.push_back(HttpParamItem("",""));
	header_list.push_back(HttpParamItem("Connection", "keep-alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN,zh;q=0.8"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply = network->GetRequest(QUrl(s_url), header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || _timeout)
	{

		reply->deleteLater();
		return -1;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	int n = statusCodeV.toInt();

	if ((reply->error() != QNetworkReply::NoError) && n != 500)
	{
		QString t2 = reply->errorString();
		return -1;
	}

	int res = -1;

	if (n != 200)
	{
		return -1;
	}

	QByteArray data = reply->readAll();

	QString temp_s = data;
	if (temp_s.contains("window.opener.user.connected")
		|| temp_s.contains("window.parent.user.connected"))
	{
		return 0;
	}


	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}


bool autobots_toutiao::GetFatieStatus(const QByteArray& byte_arr)
{
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_arr, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("message"))
			{
				QJsonValue name_value = obj.take("message");
				if (name_value.isString())
				{
					QString str_value = name_value.toString(); // success
					if (str_value == "success")
					{
						// 发帖成功
						if (obj.contains("data"))
						{
							QString comment_str;
							QString comment_id;
							QJsonValue comment_obj = obj.take("data");
							if (comment_obj.isObject())
							{
								QJsonObject obj2 = comment_obj.toObject();
								if (obj2.contains("text"))
								{
									QJsonValue text_value = obj2.take("text");
									if (text_value.isString())
									{
										comment_str = text_value.toString();
									}
								}
								if (obj2.contains("id"))
								{
									QJsonValue text_value = obj2.take("id");
									if (text_value.isDouble())
									{
										double id = text_value.toDouble();
										comment_id = QString::number(id, 'f', 0);
									}
								}
							}

							if (!comment_id.isEmpty())
							{
								FatieSucceed(comment_str, comment_id);
								return true;
							}
						}
					}
				}
			}
		}
	}

	return false;
}

bool autobots_toutiao::NeedValidateCode(const QString& name, QString& vcode, QString& code_sign)
{
	QString str_url1 = "http://graph.renren.com/icode/check";

	QUrl url1(str_url1);
	QString str_temp = "http://graph.renren.com/oauth/grant?client_id=" + m_client_id + "&redirect_uri=http://api.snssdk.com/auth/login_success/&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000";
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	header_list.push_back(HttpParamItem("Referer", str_temp));
	//network->GetManager().setCookieJar(new QNetworkCookieJar(this));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("authFeed", "true"));
	post_data.push_back(HttpParamItem("authorizeOrigin", "00000"));
	post_data.push_back(HttpParamItem("client_id", m_client_id));
	post_data.push_back(HttpParamItem("display", "page"));
	post_data.push_back(HttpParamItem("follow", "true"));
	post_data.push_back(HttpParamItem("icode", ""));
	post_data.push_back(HttpParamItem("isNeedIcode", ""));
	post_data.push_back(HttpParamItem("login_type", "false"));
	post_data.push_back(HttpParamItem("password", ""));
	post_data.push_back(HttpParamItem("porigin", "80100"));
	//post_data.push_back(HttpParamItem("post_form_id", m_post_id));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("scope", "status_update photo_upload create_album"));
	post_data.push_back(HttpParamItem("secure", "true"));
	post_data.push_back(HttpParamItem("state", "renren_sns__0____toutiao____2__0__24"));
	post_data.push_back(HttpParamItem("username", name));

	QNetworkReply* reply = network->PostRequest(url1, header_list, post_data);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QString str = reply->readAll();

	reply->deleteLater();

	if (str.toInt() == 0)
	{
		return false;
	}

	// 获取验证码

	QUrl url2("http://icode.renren.com/getcode.do?t=openplatform");
	HttpParamList header_list2;
	header_list2.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list2.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list2.push_back(HttpParamItem("Host", "icode.renren.com"));
	header_list2.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	header_list2.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply2 = network->GetRequest(url2, header_list2);

	_t.restart();

	_timeout = false;

	while (reply2 && !reply2->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply2 == NULL || (reply2->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply2->readAll();

	QImage image_ = QImage::fromData(data);//(data.data(),100,40,QImage::Format_RGB32);

	if (m_code_online)
	{
		// 在线验证
		//bool result = image_.save("e:\\1.jpg");
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		int res = obj->GetRecResults(data, "bestsalt", "hh610520;;", "bestsalt", vcode, code_sign);
	}
	else
	{
		// 手动验证
		ValidateDlg dlg(this, image_);
		if (dlg.exec() == QDialog::Accepted)
		{
			vcode = dlg.getCode();
		}
	}

	return true;
}

void autobots_toutiao::WaitforSeconds(int nseconds)
{
	QTime t2;
	t2.restart();
	while (t2.elapsed() < nseconds * 1000)
		QCoreApplication::processEvents();
}

bool autobots_toutiao::PreLoginSina(const QString& name, SinaData& data,
	QString& vcode, QString& code_sign)
{
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QString str_time = QString::number(time);

	// 获取用户名
	sina_encrypt* encryptor = sina_encrypt::GetInstance();
	QString encrypted_name = encryptor->EncryptName(name);

	QString str_url1 = QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.18)&_=%2").arg(encrypted_name, str_time);
	QUrl url1(str_url1);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://service.zol.com.cn/user/api/sina/callback.php&response_type=code").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	//header_list.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply = network->GetRequest_ssl(url1, header_list);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray str = reply->readAll();

	bool res = GetPreLoginResult(str, data);

	reply->deleteLater();

	if (!res)
	{
		return false;
	}

	if (!data._showpin) // 不需要验证码
	{
		return true;
	}

	int n_rand = rand() % 9;
	double d = 0.987654321235647 / n_rand;
	d = d * 100000000;

	QString str_r = QString::number(d, 'f', 0);

	// 获取验证码
	//https://login.sina.com.cn/cgi/pin.php?r=54763124&s=0&p=gz-db24c75a1d5b6a51bea82087efb1a2ce12a5
	QString str_url2 = QString("https://login.sina.com.cn/cgi/pin.php?r=%1&s=0&p=%2").arg(str_r, data._pcid);
	QUrl url2(str_url2);
	HttpParamList header_list2;
	header_list2.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list2.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list2.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list2.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	//header_list2.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply2 = network->GetRequest_ssl(url2, header_list2);

	_t.restart();

	_timeout = false;

	while (reply2 && !reply2->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply2 == NULL || (reply2->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	if (n != 200)
	{
		return false;
	}

	QByteArray rp_data = reply2->readAll();

	//   QVariant var = reply2->header(QNetworkRequest::ContentTypeHeader);
	// 
	//   QString type = var.toString();

	QImage image_ = QImage::fromData(rp_data);
	if (m_code_online)
	{
		// 在线验证
		//bool result = image_.save("e:\\1.jpg");
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		int res = obj->GetRecResults(rp_data, "bestsalt", "hh610520;;", "bestsalt", vcode, code_sign);
		//obj->ReportError("bestsalt", code_sign); 
	}
	else
	{
		// 手动验证
		ValidateDlg dlg(this, image_);
		if (dlg.exec() == QDialog::Accepted)
		{
			vcode = dlg.getCode();
		}
	}

	return true;
}

bool autobots_toutiao::GetPreLoginResult(const QByteArray& str, SinaData& data)
{
	//sinaSSOController.preloginCallBack({"retcode":0,"servertime":1451282884,
	//"pcid":"gz-db24c75a1d5b6a51bea82087efb1a2ce12a5","nonce":"J2Z37I",
	//"pubkey":"EB2A38.....",
	//"rsakv":"1330428213","is_openlock":0,"showpin":1,"exectime":10})
	QString temp = str;

	int first = temp.indexOf("(");
	QString temp2 = temp.right(temp.length() - first - 1);
	int last = temp2.lastIndexOf(")");
	temp2 = temp2.left(last);

	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toLocal8Bit(), &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("retcode"))
			{
				QJsonValue name_value = obj.take("retcode");
				int d = name_value.toDouble();
				if (d != 0)
				{
					return false;
				}
			}
			if (obj.contains("servertime"))
			{
				QJsonValue name_value = obj.take("servertime");
				double d = name_value.toDouble();
				data._servertime = QString::number(d, 'f', 0);

				if (data._servertime.isEmpty())
				{
					return false;
				}

			}
			if (obj.contains("nonce"))
			{
				QJsonValue name_value = obj.take("nonce");
				data._nonce = name_value.toString();
				if (data._nonce.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("pubkey"))
			{
				QJsonValue name_value = obj.take("pubkey");
				data._pubkey = name_value.toString();
				if (data._pubkey.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("rsakv"))
			{
				QJsonValue name_value = obj.take("rsakv");
				data._rsakv = name_value.toString();
				if (data._rsakv.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("pcid"))
			{
				QJsonValue name_value = obj.take("pcid");
				data._pcid = name_value.toString();
			}
			if (obj.contains("showpin"))
			{
				QJsonValue name_value = obj.take("showpin");
				int i = name_value.toDouble();
				data._showpin = (i == 0) ? false : true;
			}

			return true;
		}
	}

	return false;
}

bool autobots_toutiao::LoginSina(SinaData& sina_data, const
	QString& name, const QString& password, const QString& vcode, const QString& code_sign)
{
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QString str_time = QString::number(time);

	// 获取用户名
	sina_encrypt* encryptor = sina_encrypt::GetInstance();
	QString encrypted_name = encryptor->EncryptName(name);
	QString encrypted_pwd = encryptor->EncryptPassword(password, sina_data._servertime, sina_data._nonce, sina_data._pubkey);

	QString str_login_url = QString("https://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.18)&_=%1").arg(str_time);

	QUrl url1(str_login_url);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=mobile&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Referer",str_temp));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	header_list.push_back(HttpParamItem("Origin", "https://api.weibo.com"));

	HttpParamList post_data;
	//post_data.push_back(HttpParamItem("action", "login"));
	post_data.push_back(HttpParamItem("entry", "openapi"));
	post_data.push_back(HttpParamItem("gateway", "1"));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("savestate", "0"));
	post_data.push_back(HttpParamItem("useticket", "1"));
	post_data.push_back(HttpParamItem("pagerefer", ""));
	post_data.push_back(HttpParamItem("pcid", sina_data._pcid));
	post_data.push_back(HttpParamItem("ct", "1800"));
	post_data.push_back(HttpParamItem("s", "1"));
	post_data.push_back(HttpParamItem("vsnf", "1"));
	post_data.push_back(HttpParamItem("vsnval", ""));
	post_data.push_back(HttpParamItem("door", vcode));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("su", encrypted_name));
	post_data.push_back(HttpParamItem("service", "miniblog"));
	post_data.push_back(HttpParamItem("servertime", sina_data._servertime));
	post_data.push_back(HttpParamItem("nonce", sina_data._nonce));
	post_data.push_back(HttpParamItem("pwencode", "rsa2"));
	post_data.push_back(HttpParamItem("rsakv", sina_data._rsakv));
	post_data.push_back(HttpParamItem("sp", encrypted_pwd));
	post_data.push_back(HttpParamItem("sr", "375*667"));
	post_data.push_back(HttpParamItem("encoding", "UTF-8"));
	post_data.push_back(HttpParamItem("prelt", "1235"));
	post_data.push_back(HttpParamItem("cdult", "423"));
	post_data.push_back(HttpParamItem("domain", "weibo.com"));
	post_data.push_back(HttpParamItem("returntype", "TEXT"));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray rp_data = reply->readAll();

	bool res = GetLoginResult(rp_data, sina_data,code_sign);

	reply->deleteLater();

	return res;
}

bool autobots_toutiao::GetLoginResult(const QByteArray& str, SinaData& data, const QString& code_sign)
{

	//QString temp = str;

	//int first = temp.indexOf("(");
	//QString temp2= temp.right(temp.length() - first -1);
	//int last = temp2.lastIndexOf(")");
	//temp2 = temp2.left(last);

	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(str, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("retcode"))
			{
				QJsonValue name_value = obj.take("retcode");
				QString d = name_value.toString();
				int nres = d.toInt();
				if (nres == 2070)
				{
					// 验证码错误
					VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
					obj->ReportError("bestsalt", code_sign);
					return false;
				}
				else if (nres != 0)
				{
					return false;
				}
			}
			if (obj.contains("ticket"))
			{
				QJsonValue name_value = obj.take("ticket");
				data._ticket = name_value.toString();
				if (data._ticket.isEmpty())
				{
					return false;
				}
			}
			return true;
		}
	}

	return false;
}

bool autobots_toutiao::AuthorizeSina(const SinaData& sina_data)
{
	QString str_url_author = "https://api.weibo.com/oauth2/authorize";

	QString str_temp = QString("https://api.weibo.com/2/oauth2/authorize?client_id=%1&response_type=code&display=mobile&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&from=&with_cookie=").arg(m_client_id);

	QUrl url1(str_url_author);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Referer",str_temp));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("display", "mobile"));
	post_data.push_back(HttpParamItem("action", "login"));
	post_data.push_back(HttpParamItem("ticket", sina_data._ticket));
	post_data.push_back(HttpParamItem("scope", ""));
	post_data.push_back(HttpParamItem("isLoginSina", ""));
	post_data.push_back(HttpParamItem("withOfficalFlag", "0"));
	post_data.push_back(HttpParamItem("quick_auth", "false"));
	post_data.push_back(HttpParamItem("withOfficalAccount", ""));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("regCallback", str_temp));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("client_id", m_client_id));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("state", m_state));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("offcialMobile", "null"));
	post_data.push_back(HttpParamItem("verifyToken", "null"));
	post_data.push_back(HttpParamItem("version", ""));
	post_data.push_back(HttpParamItem("sso_type", ""));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectLoginGet(str);
	}
	else if (n == 200)
	{
		QByteArray data = reply->readAll();
		QString t = data;
		GetUserId(data);
		return AuthorizeSina2(sina_data);
	}
	else
	{
		QByteArray data = reply->readAll();
		QString t = data;
		return false;
	}
}

bool autobots_toutiao::AuthorizeSina2(const SinaData& sina_data)
{
	QString str_url_author = "https://api.weibo.com/oauth2/authorize";

	QUrl url1(str_url_author);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
	header_list.push_back(HttpParamItem("Referer", str_temp));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("action", "authorize"));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("client_id", m_client_id));
	post_data.push_back(HttpParamItem("display", "mobile"));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("isLoginSina", ""));
	post_data.push_back(HttpParamItem("passwd", ""));
	post_data.push_back(HttpParamItem("quick_auth", "null"));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("regCallback", str_temp));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("scope", ""));
	post_data.push_back(HttpParamItem("state", m_state));
	//post_data.push_back(HttpParamItem("switchLogin","0"));
	post_data.push_back(HttpParamItem("ticket", sina_data._ticket));
	post_data.push_back(HttpParamItem("uid", m_uid));
	post_data.push_back(HttpParamItem("url", "https://api.weibo.com/oauth2/authorize"));
	post_data.push_back(HttpParamItem("verifyToken", m_verifyToken));
	post_data.push_back(HttpParamItem("visible", "0"));
	post_data.push_back(HttpParamItem("withOfficalAccount", "null"));
	post_data.push_back(HttpParamItem("withOfficalFlag", "0"));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (reply && !reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= TIMEOUT) {
			_timeout = true;
			break;
		}
	}

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectLoginGet(str);
	}
	else
	{
		QByteArray data = reply->readAll();
		QString t = data;
		return false;
	}
}

bool autobots_toutiao::GetUserId(const QByteArray& arr)
{
	// name="client_id" value="2504490989"
	// name="appkey62" value="42iQjj"
	// name="state" value="f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D"

	HtmlParseUtils html_parse(arr.data(), arr.length());

	// "查找uid"
	GumboNode* node = html_parse.FirstElementNode("name", "uid");
	QString temp1;
	if (node != NULL)
	{
		std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString str = QString::fromStdString(post_from_id);

		if (!str.isEmpty())
		{
			m_uid = str;
		}
	}

	// "查找verifyToken"
	node = html_parse.FirstElementNode("name", "verifyToken");
	if (node != NULL)
	{
		std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString temp = QString::fromStdString(client_id);

		if (!temp.isEmpty())
		{
			m_verifyToken = temp;
		}
	}

	return true;
}
