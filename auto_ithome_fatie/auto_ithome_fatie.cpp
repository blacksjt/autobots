#include "auto_ithome_fatie.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "QColor"
#include "html_parse_utils.h"
#include <QJsonDocument>
#include "code_dlg.h"
#include "sina_encrypt.h"
#include "vlidatecodeonline.h"
#include "dailconnector.h"

const int TIMEOUT = 20*1000;

QString GetTimeStr()
{
  qint64 n = QDateTime::currentMSecsSinceEpoch();

  return QString::number(n);
}

auto_ithome_fatie::auto_ithome_fatie(QWidget *parent)
    : QMainWindow(parent),
    m_account_order(0),m_comment_order(0),m_code_online(true)
{
    ui.setupUi(this);

    initialize();

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_develiver, SIGNAL(clicked()), this, SLOT(onDeveliver()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));
}

auto_ithome_fatie::~auto_ithome_fatie()
{

}

void auto_ithome_fatie::onStart()
{
  // 检查输入
  if (!CheckInput())
  {
    return;
  }

  //ui.pushButton_start->setEnabled(false);

  // 更新界面输入
  UpdateData();

  if (m_auto_change_ip)
  {
    
    AutoFatie();
  }
  else
  {
    ui.pushButton_develiver->setEnabled(true);
  }

  //ui.pushButton_start->setEnabled(true);
}

void auto_ithome_fatie::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool auto_ithome_fatie::DoWork()
{
//   if (m_comment_order >= m_comment_list.size())
//   {
//     UpdateData();
//   }
// 
//   QNetworkCookieJar* cookie = new QNetworkCookieJar();
// 
//   network.GetManager().setCookieJar(cookie);
// 
//   // 尝试登陆
//   bool login_status = false;
//   while (m_account_order < m_account_list.size())
//   {
//     // 获取CSRF TOKEN
// 
//     if (!GetContent())
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
//       break;;
//     }
// 
//     if (!RequestForLogin())
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
//       continue;
//     }
// 
//     AccountParam ac = m_account_list.at(m_account_order);
// 
//     if (!AuthorBySina(ac._id, ac._password))
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
//       //m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
//       ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255,0,0, 180));
//       m_account_order++;
//       continue;
//     }
//     else
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
//       m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
//       ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0,255,0, 180));
//       m_account_order++;
//       login_status = true;
//       break;
//     }
//   }
// 
//   if (!login_status)
//   {
//     ui.lineEdit_msg->setText(QStringLiteral("账号登陆不成功，无法继续"));
//     return false;
//   }
// 
//   QString msg;
//   msg.setNum(m_comment_order+1);
// 
//   QElapsedTimer t2;
//   t2.start();
//   while(t2.elapsed()<1000 )  
//     QCoreApplication::processEvents();
// 
//   bool b = DoPostFatie(m_comment_list[m_comment_order]);
// 
//   if (!b)
//   {
//     // 多尝试一次
//     b = DoPostFatie(m_comment_list[m_comment_order]);
//   }
// 
//   if (b)
//   {
//     m_comment_item_list[m_comment_order]->setCheckState(Qt::Checked);
//     ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,已完成"));
//     m_comment_order++;
//   }
//   else
//   {
//     ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,失败"));
//     m_comment_order++;
//   }
// 
//   t2.restart();
//   while(t2.elapsed()<1000 )  
//     QCoreApplication::processEvents();
// 
//   Logout();

  //cookie->deleteLater();

  return true;
}


bool auto_ithome_fatie::DoPostFatie(const QString& content)
{
  QString str_url1 = "http://www.ithome.com/ithome/postComment.aspx";

  QUrl url1;
  url1.setUrl(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"*/*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  header_list.push_back(HttpParamItem("DNT", "1"));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
//   qint64 ntime = QDateTime::currentMSecsSinceEpoch();
//   ntime = ntime - ntime%1000;
//   QString time = QString::number(ntime);
//   QString call_back = "iJax" + time;
  post_data.push_back(HttpParamItem("commentContent", content));
  post_data.push_back(HttpParamItem("commentNick", "undefined"));
  post_data.push_back(HttpParamItem("newsid", m_news_id));
  post_data.push_back(HttpParamItem("txtCode", "undefined"));
  post_data.push_back(HttpParamItem("type", "comment"));

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

  QByteArray rp_data = reply->readAll();

  QVariant content_type = reply->header(QNetworkRequest::ContentTypeHeader);

  QString type = content_type.toString();


  return GetFatieStatus(rp_data); 
  
}

bool auto_ithome_fatie::GetContent()
{
  QString str_url_1 = m_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", m_host)); // mhost
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* rp = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while (rp && !rp->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (rp == NULL || (rp->error() != QNetworkReply::NoError) || _timeout)
  {
      QString msg = rp->errorString();
      ui.lineEdit_msg->setText(msg);

      return false;
  }

  QVariant statusCodeV =  rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
  bool res = statusCodeV.toInt() == 200 ? true : false;

  if (rp!=NULL)
    rp->deleteLater();

  return  res;
}

bool auto_ithome_fatie::RequestForLogin()
{
	QString str_url1 = "http://www.ithome.com/ithome/divlogin.aspx";

	QUrl url1(str_url1);

	HttpParamList header_list;
	//header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", m_host));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network.GetRequest(url1, header_list);

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
        ui.lineEdit_msg->setText(msg);
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	bool res = (n == 200 ? true : false);

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}


void auto_ithome_fatie::onAddCommentID()
{
  
  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  ui.listWidget_comment_id->addItem(id);

}

void auto_ithome_fatie::UpdateData()
{
  m_url = ui.lineEdit_url->text();
  m_news_id = ui.lineEdit_news_id->text();
//  m_chanel = ui.lineEdit_news_chanel->text();
  m_host = ui.lineEdit_host->text();
  m_interval = ui.spinBox_interval->value();
  m_code_online = ui.checkBox_CodeOnline->checkState() == Qt::Checked;
  m_auto_change_ip = ui.checkBox_changeip->checkState() == Qt::Checked;
  m_comment_list.clear();
  m_account_list.clear();
  m_comment_item_list.clear();
  m_account_row_list.clear();
  m_comment_order = 0;
  m_account_order = 0;

  int n = ui.listWidget_comment_id->count();

  for (int i = 0 ; i < n; ++i)
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

  for (int i = 0 ; i < n; ++i)
  {
    QTableWidgetItem* item = ui.tableWidget_account_id->item(i, 0);
    if (item->checkState() != Qt::Checked)
    {
      QString name = ui.tableWidget_account_id->item(i, 0)->text();
      QString password = ui.tableWidget_account_id->item(i, 1)->text();
      m_account_list.push_back(AccountParam(name,password));
      m_account_row_list.push_back(ui.tableWidget_account_id->item(i, 0));
    }
  }
}

void auto_ithome_fatie::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
  ui.pushButton_develiver->setEnabled(false);
  ui.checkBox_CodeOnline->setCheckState(Qt::Checked);
  m_client_id = "2504490989";
  m_appkey= "42iQjj";
  m_state = "f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D";
}

void auto_ithome_fatie::onActFromTxt()
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
  m_account_list.clear();
  m_account_order = 0;
  m_account_row_list.clear();

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

void auto_ithome_fatie::Logout()
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString stime = QString::number(time);
  QString str_url_1 = "http://www.ithome.com/ithome/login.aspx/btnLogout_Click";

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list1.push_back(HttpParamItem("DNT", "1"));
  ////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", m_host));
  header_list1.push_back(HttpParamItem("Referer", m_url));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  HttpParamList post_data;
  QNetworkReply* reply_1 = network.PostRequest(url_1, header_list1, post_data);

  QTime _t;
  _t.start();

  while(reply_1 && !reply_1->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= 2000) {
      break;
    }
  }

  reply_1->deleteLater();
}

void auto_ithome_fatie::onActImportComment()
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
  ui.listWidget_comment_id->clear();
  m_comment_item_list.clear();
  m_comment_list.clear();
  m_comment_order = 0;

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    ui.listWidget_comment_id->addItem(new QListWidgetItem(line_str));
  }

  file.close();
}

void auto_ithome_fatie::onActClearComments()
{
  ui.listWidget_comment_id->clear();
  m_comment_list.clear();
  m_comment_item_list.clear();
  m_comment_order = 0;
}

void auto_ithome_fatie::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  m_account_order = 0;
  m_account_list.clear();
  m_account_row_list.clear();
}

bool auto_ithome_fatie::CheckInput()
{
  if (ui.lineEdit_url->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_news_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻频道没有输入")); 
    return false;
  }

//   if (ui.lineEdit_news_chanel->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
//     return false;
//   }

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

void auto_ithome_fatie::FatieSucceed(const QString& id)
{
  m_comment_item_list[m_comment_order]->setCheckState(Qt::Checked);
  //ui.lineEdit_msg->setText(comment + QStringLiteral(":成功"));

  ui.textBrowser_commentid->append(id);
}

bool auto_ithome_fatie::GetFatieStatus(const QByteArray& byte_arr)
{
//   QString temp = byte_arr;
// 
//   int first = temp.indexOf("(");
//   QString temp2= temp.right(temp.length() - first -1);
//   int last = temp2.lastIndexOf(")");
//   temp2 = temp2.left(last);
// 
//   QJsonParseError json_error;
//   QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toUtf8(), &json_error); 
//   if(json_error.error == QJsonParseError::NoError) 
//   {  
//     if(parse_doucment.isObject())  
//     {  
//       QJsonObject obj = parse_doucment.object(); 
//       if(obj.contains("result"))  
//       {  
//         QJsonValue name_value = obj.take("result");
//         if(name_value.isObject())
//         {
//           QJsonObject res_obj = name_value.toObject(); // success
// 
//           if (res_obj.contains("status"))
//           {
//             QJsonObject status_obj = res_obj.take("status").toObject();
//             if (status_obj.contains("code"))
//             {
//               QJsonValue code_value = status_obj.take("code");
//               if (code_value.isDouble())
//               {
//                 int res = code_value.toDouble();
//                 if (res == 0)
//                 {
//                   return true;
//                 }
//               }
//             }
//           }
// 
//         }
//       }
//     }
//   }
	QString result = QString::fromLocal8Bit(byte_arr);
	if (result.contains(QStringLiteral("评论成功")))
	{
		return true;
	}

  return false;
}


int auto_ithome_fatie::GetLoginResult(const QByteArray& data)
  {
	//{"d":"ok:salt"}
	//{"d":"ok:姐多久额哦"}
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("d"))
			{
				QJsonValue val = obj.take("d");
				if (val.isString())
				{
					QString str = val.toString();
					//"ok:salt"
					if (str.contains("ok", Qt::CaseInsensitive))
					{
						return true;
					}
				}
			}
		}
	}
	QString str = data;
	if (str.contains("d\":\"ok"))
	{
		return true;
	}

	return false;
  }

void auto_ithome_fatie::onDeveliver()
{
  control_status = true;

  if (m_account_order >= m_account_list.size())
  {
    ui.lineEdit_msg->setText(QStringLiteral("账号不够用了"));
    return;
  }
  ui.pushButton_develiver->setEnabled(false);
  DoWork();
  ui.pushButton_develiver->setEnabled(true);
}

void auto_ithome_fatie::AutoFatie()
{
  DailConnector connector("VPN","ycc1","111");

  for (int i = 0; i < m_comment_list.size(); ++i)
  {
    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

    network.GetManager().setCookieJar(cookie);

    EmitMsgStatusBar(QStringLiteral("拨号中..."));
    QString dial_msg;
    while(!connector.ReConnect(dial_msg))
    {
      EmitMsgStatusBar(QStringLiteral("拨号中失败：")+ dial_msg );
      WaitforSeconds(3);
      EmitMsgStatusBar(QStringLiteral("拨号中..."));
    }

    EmitMsgStatusBar(QStringLiteral("拨号成功..."));

    int ncount = 0;
    while (!GetContent() && ncount < 15)
    {
      connector.ReConnect(dial_msg);
      cookie->deleteLater();
      cookie = new QNetworkCookieJar(this);
      network.GetManager().setCookieJar(cookie);

      WaitforSeconds(3);
      ncount++;
    }

    if (ncount >= 15)
    {
      ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
      EmitMsgStatusBar(QStringLiteral("网站连接失败...,请检查网络连接"));
      continue;;
    }

    // 尝试登陆
    bool login_status = false;
    while (m_account_order < m_account_list.size())
    {
      if (!RequestForLogin())
      {
        ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
        cookie->deleteLater();
        cookie = new QNetworkCookieJar(this);
        network.GetManager().setCookieJar(cookie);
        continue;
      }

      AccountParam ac = m_account_list.at(m_account_order);

      if (!Login(ac._id, ac._password))
      {
        ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
        //m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
        ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255,0,0, 180));
        m_account_order++;
        continue;
      }
      else
      {
        ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
        m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
        ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0,255,0, 180));
        m_account_order++;
        login_status = true;
        break;
      }
    }

    if (!login_status)
    {
      ui.lineEdit_msg->setText(QStringLiteral("1、网络无连接 \n 2、号登陆不成功，无法继续"));
      return;
    }

    QString msg;
    msg.setNum(m_comment_order+1);

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    if (DoPostFatie(m_comment_list[m_comment_order]))
    {
      m_comment_item_list[m_comment_order]->setCheckState(Qt::Checked);
      ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,已完成"));
      m_comment_order++;
    }
    else
    {
      ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,失败"));
      m_comment_order++;
    }

    WaitforSeconds(m_interval);

    Logout();

    cookie->deleteLater();
  }
  
  connector.DisConnect();
  EmitMsgStatusBar(QStringLiteral("已完成"));
}

void auto_ithome_fatie::EmitMsgStatusBar(const QString& msg)
{
  ui.statusBar->showMessage(msg);
}

void auto_ithome_fatie::WaitforSeconds(int nseconds)
{
  QTime t2;
  t2.restart();
  while(t2.elapsed() < nseconds*1000 )  
    QCoreApplication::processEvents();
}

bool auto_ithome_fatie::Login(const QString& name, const QString& password)
{
	QString str_url1 = "http://www.ithome.com/ithome/login.aspx/btnLogin_Click";
	QUrl url1(str_url1);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept", "application/json, text/javascript, */*; q=0.01"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Content-Type", "application/json; charset=utf-8"));
	header_list.push_back(HttpParamItem("Host", m_host));
	header_list.push_back(HttpParamItem("Referer", m_url));
	//Referer	
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QJsonObject obj;

	obj.insert("username", name);
	obj.insert("password", password);

	QJsonDocument doc1(obj);
	QString temp2 = doc1.toJson();

	QNetworkReply* reply = network.PostRequest_json(url1, header_list, doc1);

	//QNetworkReply* reply = network.PostRequest_ssl(url1, header_list,post_data);

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

	if (n == 200)
	{
		QByteArray data = reply->readAll();
		res = GetLoginResult(data);
	}

	if (reply != NULL)
	{
		reply->deleteLater();
	}

	return res;
}
