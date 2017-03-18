#include "toutiao_mobile.h"
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
#include "../utility/utility_common.h"

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

QString Getiid()
{
	srand(time(NULL));
	double i = (double)rand() / RAND_MAX;
	qlonglong n = 7111111111 + 888888888 * i;
	return QString::number(n);
}

QString GetOpenUdid()
{
	QString uuid1 = QUuid::createUuid().toString();
	QString uuid2 = QUuid::createUuid().toString();
	uuid1 += uuid2;
	uuid1.remove('{');
	uuid1.remove('}');
	uuid1.remove('-');
	QString temp = uuid1.left(40);
	return temp;
}


const int TIMEOUT = 20 * 1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
	: control_status(true), QMainWindow(parent),
	m_account_order(0),m_device_order(0),
	m_client_id("394e2173327e4ead8302dc27f4ae8879")
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

}

void autobots_toutiao::onStart()
{
	// �������
	if (!CheckInput())
	{
		return;
	}

	// ���½�������
	UpdateData();

	//   if (m_account_list.size() < m_comment_list.size())
	//   {
	//     QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("�û��˻�̫��")); 
	//     return;
	//   }

	control_status = true;

	// ѭ������
	for (int i = 0; i < m_comment_list.size(); ++i)
	{
		if (!control_status)
		{
			break;
		}

		if (m_account_order >= m_account_list.size())
		{
			ui.lineEdit_msg->setText(QStringLiteral("�˺Ų�������"));
			return;
		}

		// ���Ե�½
		bool login_status = false;
		while (m_account_order < m_account_list.size())
		{
			network.GetManager().clearAccessCache();

			QNetworkCookieJar* cookie = new QNetworkCookieJar();

			network.GetManager().setCookieJar(cookie);

			//if (!RequestForRenren())
			//{
			//	ui.lineEdit_msg->setText(QStringLiteral("����ʧ��..."));
			//	cookie->deleteLater();
			//	continue;
			//}

			AccountParam ac = m_account_list.at(m_account_order);

			if (!LoginTTByMobile(ac._id, ac._password))
			{
				ui.lineEdit_msg->setText(QStringLiteral("��½ʧ��..."));
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255, 0, 0, 180));
				m_account_order++;
				continue;
			}
			else
			{
				ui.lineEdit_msg->setText(QStringLiteral("��½�ɹ�"));
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0, 255, 0, 180));
				m_account_order++;
				login_status = true;
				break;
			}
		}

		if (!login_status)
		{
			ui.lineEdit_msg->setText(QStringLiteral("�˺ŵ�½���ɹ����޷�����"));
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

		ui.lineEdit_msg->setText(QStringLiteral("��") + msg + QStringLiteral("�����"));

		t2.restart();
		while (t2.elapsed()< m_interval * 1000)  // ʱ����
		{
			ui.lineEdit_msg->setText(QString::number(t2.elapsed()));
			QCoreApplication::processEvents();
		}
	}

	ui.lineEdit_msg->setText(QStringLiteral("�����"));
}

void autobots_toutiao::onPause()
{
	control_status = false;
	ui.lineEdit_msg->setText(QStringLiteral("��ֹͣ"));
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
	//header_list.push_back(HttpParamItem("Referer", m_url));
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

	QNetworkReply* reply = network.PostRequest_ssl(url1, header_list, post_data);

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


void autobots_toutiao::CodeCheckForRenren()
{

}

bool autobots_toutiao::LoginTTByMobile(const QString& name, const QString& password)
{
	////1.������֤��
	//QString vcode, code_sign;
	//bool need_code = NeedValidateCode(name, vcode, code_sign);
	//QString str_need_code = need_code ? "true" : "";
	QString account_en = Utility::encodeTT(name);
	QString pwd_en = Utility::encodeTT(password);

	if (account_en.isEmpty() || pwd_en.isEmpty())
	{
		return false;
	}
	QString temp = "mobile=" + account_en + "&password=" + pwd_en;
	int body_len = temp.length();
	QString body_len_str = QString::number(body_len);

	QString str_url1 = QString("https://security.snssdk.com/user/mobile/login/v2/?mix_mode=1&version_code=6.0.1&app_name=news_article&vid=%1&device_id=%2&channel=App%20Store&resolution=750*1334&aid=13&ab_version=112577,101786,101533,113093,110341,112639,112692,112070,113114,106784,112630,97143,113119,111341,101558,112868,112815,112532,105610,105822,112578,111798,110795,98039,105475&ab_feature=z2&ab_group=z2&openudid=%3&live_sdk_version=1.6.5&idfv=%4&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=%5&ab_client=a1,f2,f7,e1&device_type=iPhone%206&idfa=%6")
		.arg(m_devices_list[m_device_order]._uuid)
		.arg(m_devices_list[m_device_order]._did)
		.arg(m_devices_list[m_device_order]._openudid)
		.arg(m_devices_list[m_device_order]._uuid)
		.arg(m_devices_list[m_device_order]._iid)
		.arg(m_devices_list[m_device_order]._idfa);
	QUrl url1(str_url1);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-Hans-CN;q=1"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
	header_list.push_back(HttpParamItem("Host", "security.snssdk.com"));
	header_list.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	header_list.push_back(HttpParamItem("tt-request-time", GetTimeStr()));
	header_list.push_back(HttpParamItem("Content-Length", body_len_str));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("mobile", account_en));
	post_data.push_back(HttpParamItem("password", pwd_en));

	QNetworkReply* reply = network.PostRequest_ssl(url1, header_list, post_data);

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
		QString msg = reply->errorString();
		reply->deleteLater();	
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n!=200)
	{
		return false;
	}

	bool res = false;

	QByteArray data = reply->readAll();
	res = GetLoginRst(data);

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}


void autobots_toutiao::onAddCommentID()
{

	QString id = ui.lineEdit_comment_id->text();

	ui.lineEdit_comment_id->setText("");

	ui.listWidget_comment_id->addItem(id);

}

void autobots_toutiao::UpdateData()
{
	//m_url = ui.lineEdit_url->text();
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
}

void autobots_toutiao::initialDevices()
{
	//for (size_t i = 0; i < 100; i++)
	//{
	//	DeviceParam dev;
	//	dev._uuid = GetUuid();
	//	dev._did = GetDid();
	//	dev._useragent = "News/6.0.1 (iPhone; iOS 9.3.5; Scale/2.00)";
	//	dev._device_type = GetDeviceType();
	//	dev._idfa = GetUuid();
	//	dev._openudid = GetOpenUdid();
	//	dev._iid = Getiid();
	//	m_devices_list.push_back(dev);

	//}
	for (size_t i = 0; i < 100; i++)
	{
		DeviceParam dev;
		dev._uuid = "674FB6B1-60E9-4315-88FC-AAC84BEFAB46";
		dev._did = "3135986566";
		dev._useragent = "Mozilla / 5.0 (iPhone; CPU iPhone OS 9_3_5 like Mac OS X) AppleWebKit / 601.1.46 (KHTML, like Gecko) Mobile / 13G36 NewsArticle / 5.8.3.2 JsSdk / 2.0 NetType / WIFI(News 5.8.3 9.300000)";
		dev._device_type = "iPhone%206";
		dev._idfa = "86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A";
		dev._openudid = "0d919477efbefb99dfe7a02a2df34d9127ecc947";
		dev._iid = "7730017535";
		m_devices_list.push_back(dev);

	}
}

void autobots_toutiao::onActFromTxt()
{
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��ѡ���ļ�·��"), "", QStringLiteral("Txt(*.txt)"));
	if (path.isEmpty())  //�ж��û��Ƿ�ѡ�����ļ�
	{
		//emitLogMessage(QStringLiteral("ȡ�����ļ�"));
		return;
	}

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, "��ʾ", "�޷����ļ�");
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
	//header_list1.push_back(HttpParamItem("Referer", m_url));
	header_list1.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));

	QNetworkReply* reply_1 = network.GetRequest(url_1, header_list1);

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
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��ѡ���ļ�·��"), "", QStringLiteral("Txt(*.txt)"));
	if (path.isEmpty())  //�ж��û��Ƿ�ѡ�����ļ�
	{
		//emitLogMessage(QStringLiteral("ȡ�����ļ�"));
		return;
	}

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, "��ʾ", "�޷����ļ�");
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
	//if (ui.lineEdit_url->text().isEmpty())
	//{
	//	QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("URLû������"));
	//	return false;
	//}

	if (ui.lineEdit_page_id->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("����IDû������"));
		return false;
	}

	if (ui.listWidget_comment_id->count() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("û�����۵�ID"));
		return false;
	}

	if (ui.tableWidget_account_id->rowCount() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("�뵼���˺�"));
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
	ui.lineEdit_msg->setText(comment + QStringLiteral(":�ɹ�"));

	ui.textBrowser_commentid->append(id);
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
						// �����ɹ�
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
	//network.GetManager().setCookieJar(new QNetworkCookieJar(this));

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
	post_data.push_back(HttpParamItem("post_form_id", m_post_id));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("scope", "status_update photo_upload create_album"));
	post_data.push_back(HttpParamItem("secure", "true"));
	post_data.push_back(HttpParamItem("state", "renren_sns__0____toutiao____2__0__24"));
	post_data.push_back(HttpParamItem("username", name));

	QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

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

	// ��ȡ��֤��

	QUrl url2("http://icode.renren.com/getcode.do?t=openplatform");
	HttpParamList header_list2;
	header_list2.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list2.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list2.push_back(HttpParamItem("Host", "icode.renren.com"));
	header_list2.push_back(HttpParamItem("User-Agent", m_devices_list[m_device_order]._useragent));
	header_list2.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply2 = network.GetRequest(url2, header_list2);

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
		// ������֤
		//bool result = image_.save("e:\\1.jpg");
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		int res = obj->GetRecResults(data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
	}
	else
	{
		// �ֶ���֤
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

bool autobots_toutiao::GetLoginRst(const QByteArray& data)
{
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
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
					if (str_value.contains("success"))
					{
						return true;
					}
				}
			}
		}
	}

	return false;
}