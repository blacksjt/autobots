#include "toutiao_mobile_dz.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "code_dlg.h"
#include "vlidatecodeonline.h"
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

const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 1*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),network(NULL),
    m_client_id("394e2173327e4ead8302dc27f4ae8879"), m_device_order(0)
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

    connect(ui.pushButton_compute_num, SIGNAL(clicked()), this, SLOT(onComputeNumber()));

    connect(ui.pushButton_count, SIGNAL(clicked()), this, SLOT(onCountAll()));
}

autobots_toutiao::~autobots_toutiao()
{

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

  //bool res = GetDongtaiIDMap();
  //ui.lineEdit_msg->setText(QStringLiteral("获取动态ID"));
  
  control_status = true;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    if (!control_status || m_comment_list.isEmpty())
    {
      break;
    }

	if (network == NULL)
	{
		network->deleteLater();
		network = NULL;
	}

	network = new toutiao_network;

    //if (!RequestForRenren())
    //{
    //  ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
    //  continue;
    //}

    AccountParam ac = m_account_list.at(i);

    if (!LoginTTByMobile(ac._id, ac._password))
    {
      ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
      ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(255,0,0, 180));
      continue;
    }

    ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
    ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(0,255,0, 180));

    bool do_up = DoAction();

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<500 )  
      QCoreApplication::processEvents();

//     if (m_current_count >= m_base_count)
//     {
//       // 已经达到基本数,每隔一个随机数再减掉一个
//       // 随机产生一个基本数
//       m_comment_list.pop_back();
//       //m_comment_item_list.pop_back();
//       int n = GetRandom();
//       m_base_count += n;
//     }
	m_device_order++;

	if (m_device_order >= m_devices_list.size())
	{
		m_device_order = 0;
	}

    t2.restart();
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

  }

  ui.lineEdit_msg->setText(QStringLiteral("已完成"));
}

void autobots_toutiao::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool autobots_toutiao::DoAction()
{
  //http://is.snssdk.com/2/data/comment_action/?version_code=5.9.8&app_name=news_article&vid=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&device_id=3135986566&channel=App%20Store&resolution=750*1334&aid=13&ab_version=101786,101533,103923,104704,104785,97143,31211,104086,104716,101558,103799,94041,102203,92441,104236,104005,103173,103217,103586,104712,104552,98041,103773,101405,103631,103435&ab_feature=z2&ab_group=z2&openudid=0d919477efbefb99dfe7a02a2df34d9127ecc947&live_sdk_version=1.6.5&idfv=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=7730017535&ab_client=a1,f2,f7,e1&device_type=iPhone%206&idfa=86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A
  QString str_url1 = QString("http://is.snssdk.com/2/data/comment_action/?version_code=5.9.8&app_name=news_article&vid=%1&device_id=%2&channel=App%20Store&resolution=750*1334&aid=13&ab_version=101786,101533,103923,104704,104785,97143,31211,104086,104716,101558,103799,94041,102203,92441,104236,104005,103173,103217,103586,104712,104552,98041,103773,101405,103631,103435&ab_feature=z2&ab_group=z2&openudid=%3&live_sdk_version=1.6.5&idfv=%4&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=%5&ab_client=a1,f2,f7,e1&device_type=%6&idfa=%7")
	  .arg(m_devices_list[m_device_order]._uuid)
	  .arg(m_devices_list[m_device_order]._did)
	  .arg(m_devices_list[m_device_order]._openudid)
	  .arg(m_devices_list[m_device_order]._uuid)
	  .arg(m_devices_list[m_device_order]._iid)
	  .arg(m_devices_list[m_device_order]._device_type)
	  .arg(m_devices_list[m_device_order]._idfa);

  QUrl url1;
  url1.setUrl(str_url1);

  QList<int> remove_list;

  for(int i = 0; i <m_comment_list.size(); ++i)
  {
     QString str_id = m_comment_list[i]->text(0);
// 	QString str_d_id;
// 	if (m_id_dongtaiid.find(str_id) != m_id_dongtaiid.end())
// 	{
// 		str_d_id = m_id_dongtaiid[str_id];
// 	}
// 	else
// 	{
// 		continue;
// 	}

	 HttpParamList header_list;
	 header_list.push_back(HttpParamItem("Accept", "*/*"));
	 header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	 header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	 header_list.push_back(HttpParamItem("Accept-Language", "zh-Hans-CN;q=1"));
	 header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
	 header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	 header_list.push_back(HttpParamItem("tt-request-time", GetTimeStr()));
	 header_list.push_back(HttpParamItem("Host", "is.snssdk.com"));
	 //header_list.push_back(HttpParamItem("Referer", m_url));
	 header_list.push_back(HttpParamItem("User-Agent", "News/5.9.8 (iPhone; iOS 9.3.5; Scale/2.00)"));
	 //header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));

    HttpParamList post_data;
    post_data.push_back(HttpParamItem("action", "digg"));
	post_data.push_back(HttpParamItem("aggr_type", "1"));
	post_data.push_back(HttpParamItem("device_id", m_devices_list[m_device_order]._did));
	post_data.push_back(HttpParamItem("comment_id", str_id));
    post_data.push_back(HttpParamItem("group_id", m_group_id));
	post_data.push_back(HttpParamItem("item_id", m_item_id));
	post_data.push_back(HttpParamItem("share", "0"));

    QNetworkReply* reply = network->PostRequest(url1, header_list, post_data);

    QTime _t;
    _t.start();

    while(reply && !reply->isFinished())
    {
      QCoreApplication::processEvents();
      if (_t.elapsed() >= TIMEOUT1) {
        break;
      }
    }

    if (reply == NULL || (reply->error() != QNetworkReply::NoError) )
    {
      continue;
    }

    QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

    int n = statusCodeV.toInt();

    QByteArray data = reply->readAll();

    int count = 0;
    bool res = UpdateDiggCount(data, count);

    if (res)
    {
		if (count != 0)
		{
			QString ct = QString::number(count);
			m_comment_list[i]->setText(1, ct);
		}
		else
		{
			ui.lineEdit_msg->setText("bye bye");
		}

      int max = m_comment_list[i]->text(2).toInt();

      if (count >= max)
      {
        remove_list.push_back(i);
      }  
    }

	int interval = ((i+1) % 5 == 0) ? 15000 : 2000;

	QTime _t2;
	_t2.start();
	while (_t2.elapsed() < interval)
	{
		QCoreApplication::processEvents();
	}

  }
  
  for (int j = 0; j < remove_list.size(); ++j)
  {
    m_comment_list.removeAt(remove_list[j]);
  }

  return true;
}

void autobots_toutiao::onAddCommentID()
{
  
  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  AddToTree(id);
}

void autobots_toutiao::UpdateData()
{
  //m_url = ui.lineEdit_url->text();
  m_group_id = ui.lineEdit_page_id->text();
  m_item_id = ui.lineEdit_item_id->text();
//  m_base_count = ui.spinBox_min_count->value();
  m_comment_list.clear();
  m_account_list.clear();
  m_code_online = true;

  int n = ui.treeWidget_comment_id->topLevelItemCount();

  for (int i = 0 ; i < n; ++i)
  {
    //QString text = ui.treeWidget_comment_id->topLevelItem(i)->text(0);
    m_comment_list.push_back(ui.treeWidget_comment_id->topLevelItem(i));
    //m_comment_item_list.push_back();
  }

  n = ui.tableWidget_account_id->rowCount();

  for (int i = 0 ; i < n; ++i)
  {
    QString name = ui.tableWidget_account_id->item(i, 0)->text();
    QString password = ui.tableWidget_account_id->item(i, 1)->text();
    m_account_list.push_back(AccountParam(name,password));
    //m_account_row_list.push_back(i);
  }

}

void autobots_toutiao::initialize()
{
  initialDevices();
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
//  m_current_count = 0;
}

void autobots_toutiao::initialDevices()
{
	//for (size_t i = 0; i < 100; i++)
	//{
	//	DeviceParam dev;
	//	dev._uuid = GetUuid();
	//	dev._did = GetDid();
	//	dev._useragent = "Mozilla / 5.0 (iPhone; CPU iPhone OS 9_3_5 like Mac OS X) AppleWebKit / 601.1.46 (KHTML, like Gecko) Mobile / 13G36 NewsArticle / 5.8.3.2 JsSdk / 2.0 NetType / WIFI(News 5.8.3 9.300000)";
	//	dev._device_type = GetDeviceType();
	//	dev._idfa = GetUuid();
	//	dev._openudid = GetOpenUdid();
	//	dev._iid = Getiid();
	//	m_devices_list.push_back(dev);
	//	
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
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL, "提示", "无法打开文件");  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  //m_account_row_list.clear();
  m_account_list.clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();

    QStringList str_list = line_str.split("----");

    if (str_list.size() >= 2)
    {
      ui.tableWidget_account_id->setRowCount(ui.tableWidget_account_id->rowCount()+1);
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
	header_list1.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

	//header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
	////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
	header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
	//header_list1.push_back(HttpParamItem("Referer", m_url));
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
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.treeWidget_comment_id->clear();
  //m_comment_item_list.clear();
  m_comment_list.clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    if (!line_str.isEmpty())
    {
      AddToTree(line_str);
    }
  }

  file.close();
}

void autobots_toutiao::onActClearComments()
{
  ui.treeWidget_comment_id->clear();
  m_comment_list.clear();
}

void autobots_toutiao::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  m_account_list.clear();
}

bool autobots_toutiao::CheckInput()
{
  //if (ui.lineEdit_url->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
//     return false;
//   }

  if (ui.lineEdit_page_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
    return false;
  }

  if (ui.treeWidget_comment_id->topLevelItemCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
    return false;
  }

  if (ui.tableWidget_account_id->rowCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请导入账号")); 
    return false;
  }
  
  for (int i = 0; i < ui.treeWidget_comment_id->topLevelItemCount(); ++i)
  {
    // 第一行不空，第三行空，则未输入有效目标数
    if (!ui.treeWidget_comment_id->topLevelItem(i)->text(0).isEmpty() &&
      ui.treeWidget_comment_id->topLevelItem(i)->text(2).isEmpty())
    {
      QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请输入目标数量，或自动生成目标数量")); 
      return false;
    }
  }

  return true;
}

void autobots_toutiao::AddToTree(const QString& text)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget_comment_id);
  item->setText(0,text);
  item->setFlags(item->flags() | Qt::ItemIsEditable);  
  ui.treeWidget_comment_id->addTopLevelItem(item);
}

bool autobots_toutiao::UpdateDiggCount(const QByteArray& data, int& count_)
{
	bool ret = false;
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    { 
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("message"))  
      {  
        QJsonValue name_value = obj.take("message");
        if(name_value.isString())
        {
          QString msg = name_value.toString();

          if (msg == "success" )
          {
			ret = true;
          }
        }

      }

	  if (obj.contains("digg_count"))
	  {
		  QJsonValue count_value = obj.take("digg_count");
		  count_ = count_value.toDouble();
	  }

	  //if (obj.contains("data"))
	  //{
		 // QJsonValue data_value = obj.take("data");

		 // QJsonObject data_ojb = data_value.toObject();

		 // double comment_id = 0;
		 // if (data_ojb.contains("digg_count"))
		 // {
			//  QJsonValue count_value = data_ojb.take("digg_count");
			//  count_ = count_value.toDouble();	  
		 // }
	  //}
    }
  }

  return ret;
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
	post_data.push_back(HttpParamItem("post_form_id", m_post_id));
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
		int res = obj->GetRecResults(data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
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

int autobots_toutiao::GetRandom()
{
  // [m n]之间的随机数
  int m = 1, n = 8;
  return rand()%(n-m+1)+m;
}

void autobots_toutiao::onComputeNumber()
{
  int n = ui.spinBox_min_count->value();
  
  int count = ui.treeWidget_comment_id->topLevelItemCount();

  for (int i = count-1 ; i >= 0; --i)
  {
    QString n_str  = QString::number(n);
    ui.treeWidget_comment_id->topLevelItem(i)->setText(2,n_str);
    n = n + GetRandom();
  }
}

void autobots_toutiao::WaitforSeconds(int nseconds)
{
  QTime t2;
  t2.restart();
  while(t2.elapsed() < nseconds*1000 )  
    QCoreApplication::processEvents();
}


void autobots_toutiao::onCountAll()
{
  int count = ui.treeWidget_comment_id->topLevelItemCount();
  int all = 0;
  for (int i = count-1 ; i >= 0; --i)
  {
    //QString n_str  = QString::number(n);
    QString str = ui.treeWidget_comment_id->topLevelItem(i)->text(1);

    if (str.isEmpty())
    {
      continue;
    }

    int n = str.toInt();

    if ( n < 0 || n > 99999)
    {
      //EmitMsgStatusBar(QStringLiteral("数字有错误"));
      continue;
    }
    else
    {
      all += n;
    }  
  }

  ui.lineEdit_countall->setText(QString::number(all));

}

bool autobots_toutiao::GetDongtaiIDMap()
{
	m_id_dongtaiid.clear();

	bool has_more = true;
	int offset = 0;
	while (has_more)
	{
		has_more = false;
		GetIDList(has_more, offset);
		offset += 500;
	}

	return m_id_dongtaiid.isEmpty() ? false : true;
}

bool autobots_toutiao::ExactComments(const QByteArray& rp_data, bool& has_more)
{
	bool res = false;
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(rp_data, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();

			// 查找data节点
			if (obj.contains("data"))
			{
				QJsonObject data_obj = obj.take("data").toObject();

				// 查找comments节点
				if (data_obj.contains("comments"))
				{
					QJsonArray comments_arry = data_obj.take("comments").toArray();
					for (int i = 1; i < comments_arry.size(); ++i)
					{
						QJsonObject comment_obj = comments_arry.at(i).toObject();
						if (comment_obj.contains("id") && comment_obj.contains("dongtai_id"))
						{
							double d1 = comment_obj.take("id").toDouble();
							QString id = QString::number(d1, 'f', 0);
							QString d_id = QString::number(comment_obj.take("dongtai_id").toDouble(), 'f', 0);
							m_id_dongtaiid.insert(id, d_id);
						}
					}

				}

				if (data_obj.contains("has_more"))
				{
					has_more = data_obj.take("has_more").toBool();
				}				
			}
		}
	}

	return res;
}

void autobots_toutiao::GetIDList(bool& has_more, int offset)
{
	//http://www.toutiao.com/api/comment/list/?group_id=6333409505736392961&item_id=6333409505912553985&offset=5&count=10
	QString str_url = QString("http://www.toutiao.com/api/comment/list/?group_id=%1&item_id=%2&offset=%3&count=500").arg(
		m_group_id, m_item_id, QString::number(offset));

	QNetworkAccessManager manager;
	QNetworkRequest req;
	req.setUrl(str_url);
	req.setRawHeader("Cache-Control", "no-cache");
	req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
	req.setRawHeader("Connection", "Keep-Alive");
	//req.setRawHeader("Referer", m_url.toUtf8().data());
	req.setRawHeader("Accept-Language", "zh-CN");
	req.setRawHeader("Host", "www.toutiao.com");
	req.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

	QNetworkReply* reply = manager.get(req);

	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	QString msg;
	if (reply->error() != QNetworkReply::NoError)
	{
		msg = reply->errorString();
		ui.lineEdit_msg->setText(msg);
		reply->deleteLater();
		return;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		ui.lineEdit_msg->setText(QStringLiteral("获取动态ID失败"));
		reply->deleteLater();
		return;
	}

	QByteArray rp_data = reply->readAll();

	bool res = ExactComments(rp_data, has_more);

	reply->deleteLater();

	//return res;
}


bool autobots_toutiao::LoginTTByMobile(const QString& name, const QString& password)
{
	////1.检验验证码
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
		QString msg = reply->errorString();
		reply->deleteLater();
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
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