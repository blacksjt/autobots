#include "auto_wangyi_fatie.h"
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
#include "netease_encrypt.h"
#include "vlidatecodeonline.h"

const int TIMEOUT = 20*1000;
QString GetTimeStr()
{
  qint64 n = QDateTime::currentMSecsSinceEpoch();

  return QString::number(n);
}
autobots_toutiao::autobots_toutiao(QWidget *parent)
    : QMainWindow(parent),
    m_account_order(0),m_comment_order(0),m_code_online(true)
    ,m_auto_change_ip(true)
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

	connect(ui.comboBox_adsl, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onChanged(const QString&)));

	intialVPN();
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
  if (m_auto_change_ip)
  {

    AutoFatie();
  }
  else
  {
    ui.pushButton_develiver->setEnabled(true);
  }

}

void autobots_toutiao::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool autobots_toutiao::DoWork()
{
  if (m_comment_order >= m_comment_list.size())
  {
    UpdateData();
  }

  QNetworkCookieJar* cookie = new QNetworkCookieJar();

  network.GetManager().setCookieJar(cookie);

  // 尝试登陆
  bool login_status = false;
  while (m_account_order < m_account_list.size())
  {
    // 获取CSRF TOKEN

    if (!GetContent())
    {
      ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
      break;;
    }

    //if (!RequestForLogin())
    //{
    //  ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
    //  continue;
    //}

    AccountParam ac = m_account_list.at(m_account_order);

    if (!LoginWY(ac._id, ac._password))
    {
      ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
      ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255,0,0, 180));
      m_account_order++;
      continue;
    }
    else
    {
      ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
      ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0,255,0, 180));
      m_account_order++;
      login_status = true;
      break;
    }
  }

  if (!login_status)
  {
    ui.lineEdit_msg->setText(QStringLiteral("账号登陆不成功，无法继续"));
    return false;
  }

  QString msg;
  msg.setNum(m_comment_order);

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

  t2.restart();
  while(t2.elapsed()<1000 )  
    QCoreApplication::processEvents();

  if (login_status)
  {
      Logout(m_account_list.at(m_account_order-1)._id);
  }

  cookie->deleteLater();

  return true;
}


bool autobots_toutiao::DoPostFatie(const QString& content)
{
  //NeedValidate();
  QString token = GetToken();

  if (token.isEmpty())
  {
	  token = GetToken();
  }

  //http://comment.news.163.com/api/v1/products/a2869674571f77b5a0867c3d71db5856/threads/BQLRJGAA00014SEH/comments?ibc=newspc
  QString str_url = QString("http://%1/api/v1/products/a2869674571f77b5a0867c3d71db5856/threads/%2/comments?ibc=newspc").arg(m_host,m_news_id);

  QUrl url1;
  url1.setUrl(str_url);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"*/*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("board", m_chanel));
  post_data.push_back(HttpParamItem("content", content));
  post_data.push_back(HttpParamItem("parentId", ""));
  post_data.push_back(HttpParamItem("ntoken", token));

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
	  QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	  QString msg = reply->errorString();
    return false;
  }
  
  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 201 && n != 200)
  {
    return false;
  }

  QByteArray rp_data = reply->readAll();
  QString t = rp_data;
  return GetFatieStatus(rp_data); 
 
}

bool autobots_toutiao::GetContent()
{
  QString str_url_1 = m_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"*/*"));
  header_list1.push_back(HttpParamItem("Connection","keep-alive"));
  //header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
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

  QVariant statusCodeV =  rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
  bool res = statusCodeV.toInt() == 200 ? true : false;

  QString c = rp->readAll();

  if (rp!=NULL)
    rp->deleteLater();

  return  res;
}

bool autobots_toutiao::GetCookies()
{
    QString str_url_1 = QString("https://dl.reg.163.com/ini?pd=tie&pkid=cGTVPrV&pkht=tie.163.com&nocache=%1").arg(GetTimeStr());

    QUrl url_1(str_url_1);
    url_1.setUrl(str_url_1);
    HttpParamList header_list1;
    header_list1.push_back(HttpParamItem("Accept", "*/*"));
    header_list1.push_back(HttpParamItem("Connection", "keep-alive"));
    //header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
    header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
    header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

    //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
    //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
    header_list1.push_back(HttpParamItem("Host", "dl.reg.163.com")); // mhost
    header_list1.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

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

    QVariant statusCodeV = rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);
    bool res = statusCodeV.toInt() == 200 ? true : false;

    QString c = rp->readAll();

    if (rp != NULL)
        rp->deleteLater();

    return  res;
}


bool autobots_toutiao::LoginWY(const QString& name, const QString& password)
{
  QString tk = GetTk(name);

  if(tk.isEmpty())
  { 
      tk = GetTk(name);
  }

  if (tk.isEmpty())
  {
      return false;
  }

  
  bool r = GetVlidateCode(name);

  if (!r)
  {
      return false;
  }

  QNetworkCookieJar* jar= network.GetManager().cookieJar();
  QList<QNetworkCookie> Cl = jar->cookiesForUrl(QUrl("163.com"));
  QMap<QString,QString> map;
  for each (QNetworkCookie var in Cl)
  {
	  QString name = var.name();
	  QString val = var.value();
	  map.insert(name, val);
  }

  QString str_login_url = "https://dl.reg.163.com/l";
  QUrl url1(str_login_url);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","*/*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
  header_list.push_back(HttpParamItem("Content-Type","application/json"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "dl.reg.163.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));
  header_list.push_back(HttpParamItem("Referer","https://dl.reg.163.com/src/mp-agent-finger.html?WEBZJVersion=1481026394335"));

  QString pw = netease_encrypt::GetInstance()->EncryptPassword(password);
  QJsonObject obj;
  obj.insert("un", name);
  obj.insert("pw", pw);
  obj.insert("pd", "tie");
  obj.insert("l", 1);
  obj.insert("d", 10);
  obj.insert("t", QDateTime::currentMSecsSinceEpoch());
  obj.insert("pkid", "cGTVPrV");
  obj.insert("domains", "");
  obj.insert("tk", tk);
  obj.insert("pwdKeyUp", 0);

  QJsonDocument doc1(obj);
  QString temp2 = doc1.toJson(QJsonDocument::Compact);
  header_list.push_back(HttpParamItem("Content-Length", QString::number(temp2.length())));

  QNetworkReply* reply = network.PostRequest_json_ssl(url1, header_list, doc1);

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
    QString str = reply->errorString();
    reply->deleteLater();
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  QByteArray data = reply->readAll();

  QString res = ParseToken(data, "ret");

  if (n != 200)
  {
    return false;
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
  m_news_id = ui.lineEdit_news_id->text();
  m_chanel = ui.lineEdit_news_chanel->text();
  m_token_url = ui.lineEdit_token_url->text();
  m_host = ui.lineEdit_host->text();
  m_interval = ui.spinBox_interval->value();
  m_code_online = ui.checkBox_CodeOnline->checkState() == Qt::Checked;
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

  SaveIniSettings();

}

void autobots_toutiao::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
  ui.pushButton_develiver->setEnabled(false);
  ui.checkBox_CodeOnline->setCheckState(Qt::Checked);
  m_client_id = "2504490989";
  m_appkey= "42iQjj";
  m_state = "f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D";
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

void autobots_toutiao::Logout(const QString& name)
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString stime = QString::number(time);
  QString str_url_1 = QString("http://reg.163.com/Logout.jsp?username=%1&url=%2").arg(name,m_url);

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  ////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "reg.163.com"));
  header_list1.push_back(HttpParamItem("Referer", m_url));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* reply_1 = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();

  while(reply_1 && !reply_1->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= 2000) {
      break;
    }
  }

  if (reply_1 != NULL)
  {
    reply_1->deleteLater();
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

void autobots_toutiao::onActClearComments()
{
  ui.listWidget_comment_id->clear();
  m_comment_list.clear();
  m_comment_item_list.clear();
  m_comment_order = 0;
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

  if (ui.lineEdit_token_url->text().isEmpty())
  {
	  QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Token URL没有输入"));
	  return false;
  }

  if (ui.lineEdit_news_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻频道没有输入")); 
    return false;
  }

  if (ui.lineEdit_news_chanel->text().isEmpty())
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

  if (ui.comboBox_adsl->currentText().isEmpty())
  {
	  //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带名称"));
	  QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带名称"));
	  return false;
  }

  if (ui.lineEdit_account->text().isEmpty())
  {
	  //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带账号"));
	  QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带账号"));
	  return false;
  }

  if (ui.lineEdit_password->text().isEmpty())
  {
	  //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带密码"));
	  QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带密码"));
	  return false;
  }

  return true;
}

bool autobots_toutiao::GetFatieStatus(const QByteArray& byte_arr)
{
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_arr, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    {  
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("commentId"))  
      {  
        QJsonValue name_value = obj.take("commentId");
        int d = name_value.toDouble();
        if (d == 0)
        {
          return false;
        }
      }
      if(obj.contains("content"))  
      {  
        QJsonValue _data = obj.take("content");
        if (_data.isString())
        {
          QString c = _data.toString();
          if (!c.isEmpty())
          {
            ui.lineEdit_msg->setText(c+QStringLiteral(":成功"));
            return true;
          }
        }
      }
    }
  }

  return false;
}

void autobots_toutiao::onDeveliver()
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

void autobots_toutiao::AutoFatie()
{
	QString adsl = ui.comboBox_adsl->currentText();
	QString account = ui.lineEdit_account->text();
	QString pwd = ui.lineEdit_password->text();

  DailConnector connector(adsl,account,pwd);

  for (int i = 0; i < m_comment_list.size(); ++i)
  {
    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

    network.GetManager().setCookieJar(cookie);

#ifndef _DEBUG
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
      WaitforSeconds(3);
      ncount++;
    }

    if (ncount >= 15)
    {
      ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
      EmitMsgStatusBar(QStringLiteral("网站连接失败...,请检查网络连接"));
      continue;;
    }
#else
    GetContent();
    GetCookies();
#endif

	cookie->insertCookie(QNetworkCookie("JSESSIONID-WYTXZDL", "hFjXm\\HiIB5LHxRu/sa85d6e3vpMuaBZII/Qa7DwrQ+TJbGeKLaaANWmK7xhWq2S8H84G23RbddZArgGZW6Ng887sK5QIl68SNKcK\\BKwJH+BVV2g5faPYeFU35qsmZGnU\\N2mEnYqj843BRKOWk4rj8\\6QooF9nh+0eLxQomX9q7rZl:1481454979717"));
	cookie->insertCookie(QNetworkCookie("_ntes_nnid", "23757ad21b8bbf955a7660e31feef5a7,1481299415884"));
	cookie->insertCookie(QNetworkCookie("_ntes_nuid", "23757ad21b8bbf955a7660e31feef5a7"));
	cookie->insertCookie(QNetworkCookie("_ihtxzdilxldP8_", "30"));
	cookie->insertCookie(QNetworkCookie("Province", "025"));
	cookie->insertCookie(QNetworkCookie("City", "0512"));
	cookie->insertCookie(QNetworkCookie("vjlast", "1482226924.1482226924.30"));
	cookie->insertCookie(QNetworkCookie("vjuids", "604fbdd11.1582e3a43f8.0.c65f3906b4f35"));

    // 尝试登陆
    bool login_status = false;
    while (m_account_order < m_account_list.size())
    {
      AccountParam ac = m_account_list.at(m_account_order);   

      if (!LoginWY(ac._id,ac._password))
      {
        ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
        //m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
        ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255,0,0, 180));
        m_account_order++;

		cookie->deleteLater();
		QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
		network.GetManager().setCookieJar(cookie);
		GetCookies();
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
    msg.setNum(i+1);

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    if (DoPostFatie(m_comment_list[i]))
    {
      m_comment_item_list[i]->setCheckState(Qt::Checked);
      ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,已完成"));
    }
    else
    {
      ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,失败"));
    }

    if (i != m_comment_list.size() -1)
    {
      WaitforSeconds(m_interval);
    } 

    if (login_status)
    {
      Logout(m_account_list.at(m_account_order-1)._id);
    }

    cookie->deleteLater();
  }

  connector.DisConnect();
  EmitMsgStatusBar(QStringLiteral("已完成"));
}

void autobots_toutiao::EmitMsgStatusBar(const QString& msg)
{
  ui.statusBar->showMessage(msg);
}

void autobots_toutiao::WaitforSeconds(int nseconds)
{
  QTime t2;
  t2.restart();
  while(t2.elapsed() < nseconds*1000 )  
    QCoreApplication::processEvents();
}

bool autobots_toutiao::NeedValidate()
{
  QString str_url = "http://comment.news.163.com/reply/needvalidate.jsp?time="+GetTimeStr();

  QUrl url1;
  url1.setUrl(str_url);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"*/*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

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
    return false;
  }
  
  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 201 && n != 200)
  {
    return false;
  }

  QByteArray rp_data = reply->readAll();
  QString t = rp_data;
  NeedReply();

  return true;

}

bool autobots_toutiao::NeedReply()
{
  QString str_url = "http://comment.news.163.com/statistics.jsp?type=tieCommentReply3&time="+GetTimeStr();

  QUrl url1;
  url1.setUrl(str_url);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"*/*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

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
    return false;
  }
  
  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 201 && n != 200)
  {
    return false;
  }

  QByteArray rp_data = reply->readAll();
  QString t = rp_data;
  
  return true;
}

QString autobots_toutiao::GetToken()
{
	QUrl url1;
	url1.setUrl(m_token_url);

	//QNetworkRequest req;
	HttpParamList header_list;
	QString origin = "http://" + m_host;

	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
	header_list.push_back(HttpParamItem("Content-Length", "0"));
	header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("Host", m_host));
	header_list.push_back(HttpParamItem("Origin", origin));
	header_list.push_back(HttpParamItem("Referer", m_url));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	HttpParamList post_data;
	QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	QString msg;
	if (reply->error() != QNetworkReply::NoError)
	{
		msg = reply->errorString();
		EmitMsgStatusBar(msg);
		return QString();
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	QByteArray data = reply->readAll();

	QString ret = ParseToken(data, "gentoken");

	reply->deleteLater();

	return ret;
}

QString autobots_toutiao::ParseToken(const QByteArray & data, const QString& section)
{
	QString ret;
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains(section))
			{
				QJsonValue name_value = obj.take(section);
				if (name_value.isString())
				{
					ret = name_value.toString();
				}
			}
		}
	}

	return ret;
}

bool autobots_toutiao::GetVlidateCode(const QString& name)
{
    QString vcode, code_sign;
    QString url = QString("https://dl.reg.163.com/cp?pd=tie&pkid=cGTVPrV&random=%1").arg(GetTimeStr());
    QUrl url1;
    url1.setUrl(url);

    //QNetworkRequest req;
    HttpParamList header_list;
    //QString origin = "http://" + m_host;

    header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
    header_list.push_back(HttpParamItem("Accept", "image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5"));
    header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
    //header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
    header_list.push_back(HttpParamItem("Content-Type", "application/json"));
    header_list.push_back(HttpParamItem("Content-Length", "0"));
    header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
    header_list.push_back(HttpParamItem("Host", "dl.reg.163.com"));
    //header_list.push_back(HttpParamItem("Origin", origin));
    header_list.push_back(HttpParamItem("Referer", "http://webzj.reg.163.com/v1.0.1/pub/index_dl.html?cd=http://img1.cache.netease.com/tie/static/2016113014/&cf=tie_error_login_urs.css&MGID=1481201470968.5168&wdaId="));
    header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

    //HttpParamList post_data;
    QNetworkReply* reply = network.GetRequest_ssl(url1, header_list);

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

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
        msg = reply->errorString();
        EmitMsgStatusBar(msg);
        return false;
    }

    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    int n = statusCodeV.toInt();

    QByteArray data = reply->readAll();

    QImage image_ = QImage::fromData(data);//(data.data(),100,40,QImage::Format_RGB32);
    image_.save("e:/2.jpg");

    VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
    int res = obj->GetRecResults(data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
    //obj->ReportError("bestsalt", code_sign); 

    reply->deleteLater();

    if (res != 0)
        return false;

    // 确认验证码
    url = QString("https://dl.reg.163.com/vfcp");
    url1.setUrl(url);

    header_list.clear();
    //header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
    header_list.push_back(HttpParamItem("Accept", "*/*"));
    header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
    //header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
    header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
    //header_list.push_back(HttpParamItem("Content-Length", "0"));
    header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
    header_list.push_back(HttpParamItem("Host", "dl.reg.163.com"));
    //header_list.push_back(HttpParamItem("Origin", origin));
    header_list.push_back(HttpParamItem("Referer", "https://dl.reg.163.com/src/mp-agent-finger.html?WEBZJVersion=1481026394335"));
    header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));


    HttpParamList post_data;
    post_data.push_back(HttpParamItem("cap", vcode));
    post_data.push_back(HttpParamItem("pd", "tie"));
    post_data.push_back(HttpParamItem("pkid", "cGTVPrV"));
    post_data.push_back(HttpParamItem("un", name));

    QNetworkReply* reply2 = network.PostRequest_ssl(url1, header_list,post_data);

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

    msg = "";
    if (reply2->error() != QNetworkReply::NoError)
    {
        msg = reply2->errorString();
        EmitMsgStatusBar(msg);
        return false;
    }

    statusCodeV = reply2->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    n = statusCodeV.toInt();

    QByteArray data2 = reply2->readAll();

    QString str = ParseToken(data2, "ret");

    if (str == "201" || str == "200")
        return true;

    return false;
}

void autobots_toutiao::intialVPN()
{
	// 读取配置文件
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("缺少ini配置文件"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);
	QStringList groups = setting.childGroups();
	ui.comboBox_adsl->addItems(groups);

	// 设置默认值
	if (groups.isEmpty())
	{
		QMessageBox::critical(this, "warning", QStringLiteral("配置文件中没有有效值"));
		return;
	}

	int ndefault = setting.value("default").toInt();

	if (ndefault >= groups.size() || ndefault < 0)
	{
		ndefault = 0;
	}

	QString first = groups.at(ndefault);
	ui.comboBox_adsl->setCurrentText(first);
	setting.beginGroup(first);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}

void autobots_toutiao::SaveIniSettings()
{
	// 读取配置文件
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("缺少ini配置文件"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);
	setting.setValue("default", ui.comboBox_adsl->currentIndex());
}

QString autobots_toutiao::GetTk(const QString & name)
{
    //netease_encrypt::GetInstance()->EncryptPassword();
    QString url = QString("https://dl.reg.163.com/gt?un=%1&pkid=cGTVPrV&pd=tie&nocache=%2").arg(name).arg(GetTimeStr());
    QUrl url1;
    url1.setUrl(url);

    //QNetworkRequest req;
    HttpParamList header_list;
    //QString origin = "http://" + m_host;

    header_list.push_back(HttpParamItem("Accept", "*/*"));
    header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
    //header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language", "zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3"));
    header_list.push_back(HttpParamItem("Content-Type", "application/json"));
    header_list.push_back(HttpParamItem("Host", "dl.reg.163.com"));
    header_list.push_back(HttpParamItem("Referer", "https://dl.reg.163.com/src/mp-agent-finger.html?WEBZJVersion=1481026394335"));
    header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

    //HttpParamList post_data;
    QNetworkReply* reply = network.GetRequest_ssl(url1, header_list);

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

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
        msg = reply->errorString();
        EmitMsgStatusBar(msg);
        return QString();
    }

    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    int n = statusCodeV.toInt();

    QByteArray data = reply->readAll();

    QString ret = ParseToken(data, "tk");

    reply->deleteLater();

    return ret;
}

void autobots_toutiao::onChanged(const QString& text)
{
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("缺少ini配置文件"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);

	setting.beginGroup(text);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}