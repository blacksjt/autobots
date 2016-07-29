#include "autobots_sina.h"
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

autobots_toutiao::autobots_toutiao(QWidget *parent)
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

    if (!RequestForLogin())
    {
      ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
      continue;
    }

    AccountParam ac = m_account_list.at(m_account_order);

    if (!AuthorBySina(ac._id, ac._password))
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
    ui.lineEdit_msg->setText(QStringLiteral("账号登陆不成功，无法继续"));
    return false;
  }

  QString msg;
  msg.setNum(m_comment_order+1);

  QElapsedTimer t2;
  t2.start();
  while(t2.elapsed()<1000 )  
    QCoreApplication::processEvents();

  bool b = DoPostFatie(m_comment_list[m_comment_order]);

  if (!b)
  {
    // 多尝试一次
    b = DoPostFatie(m_comment_list[m_comment_order]);
  }

  if (b)
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

  Logout();

  //cookie->deleteLater();

  return true;
}


bool autobots_toutiao::DoPostFatie(const QString& content)
{
  QString str_url1 = "http://comment5.news.sina.com.cn/cmnt/submit";

  QUrl url1;
  url1.setUrl(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  qint64 ntime = QDateTime::currentMSecsSinceEpoch();
  ntime = ntime - ntime%1000;
  QString time = QString::number(ntime);
  QString call_back = "iJax" + time;
  post_data.push_back(HttpParamItem("callback", call_back));
  post_data.push_back(HttpParamItem("channel", m_chanel));
  post_data.push_back(HttpParamItem("content", content));
  post_data.push_back(HttpParamItem("format", "json"));
  post_data.push_back(HttpParamItem("ie", "utf-8"));
  post_data.push_back(HttpParamItem("iframe", "1"));
  post_data.push_back(HttpParamItem("img_url", ""));
  post_data.push_back(HttpParamItem("ispost", "null"));
  post_data.push_back(HttpParamItem("newsid", m_news_id));
  post_data.push_back(HttpParamItem("oe", "utf-8"));
  post_data.push_back(HttpParamItem("parent", ""));
  post_data.push_back(HttpParamItem("share_url", m_url));
  post_data.push_back(HttpParamItem("video_url", ""));

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

//   if (type.contains("json"))
//   {
     return GetFatieStatus(rp_data); 
//  }
//   else 
//   {
//       return false;
//   } 
  
}

bool autobots_toutiao::GetContent()
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

  QVariant statusCodeV =  rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
  bool res = statusCodeV.toInt() == 200 ? true : false;

  if (rp!=NULL)
    rp->deleteLater();

  return  res;
}

bool autobots_toutiao::RequestForLogin()
{
  qint64 ntime = QDateTime::currentMSecsSinceEpoch();
  ntime = ntime*10 + 1;
  QString time = QString::number(ntime);
  QString str_url1 = "http://login.sina.com.cn/sso/qrcode/image?entry=tech&size=128&callback=STK_" + time;

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Accept","application/javascript, */*;q=0.8"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(url1, header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
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
  bool res = false;

  if (n == 200)
  {
    QString qrid;
    QByteArray rp_data = reply->readAll();
    res = GetQrid(rp_data, qrid);
  }

  return res;
}

void autobots_toutiao::CodeCheckForRenren()
{

}

bool autobots_toutiao::AuthorBySina(const QString& name, const QString& password)
{
  //1.检验验证码
  QString vcode,code_sign;
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
  int res = LoginSina(sina_data,name,password,vcode);

  if (res == -2)
  {
    ui.lineEdit_msg->setText(QStringLiteral("登录失败"));
    return false;
  }
  else if (res == -1)
  {
    ui.lineEdit_msg->setText(QStringLiteral("验证码错误"));

    if (m_code_online)
    {
      VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
      obj->ReportError("bestsalt", code_sign);
    } 

    return AuthorBySina(name, password);
  }

//   // 3 授权
//   if (!AuthorizeSina(sina_data))
//   {
//     // 再次尝试
//     if (!AuthorizeSina(sina_data))
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("授权失败"));
//       return false;
//     }
//   }
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

void autobots_toutiao::Logout()
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString stime = QString::number(time);
  QString str_url_1 = "http://login.sina.com.cn/sso/logout.php?entry=tech&callback=pluginSSOController.ssoLogoutCallBack&sr=1192*670&client=ssologin.js(v1.4.18)&_="+stime;

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  ////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "login.sina.com.cn"));
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

  return true;
}

void autobots_toutiao::FatieSucceed(const QString& id)
{
  m_comment_item_list[m_comment_order]->setCheckState(Qt::Checked);
  //ui.lineEdit_msg->setText(comment + QStringLiteral(":成功"));

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
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Host","api.weibo.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  if (!str.contains("https://",Qt::CaseInsensitive))
  {
    return false;
  }

  QNetworkReply* reply = network.GetRequest_ssl(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
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

  if (n != 200)
  {
    return false;
  }

  QByteArray data = reply->readAll();

  return GetPostId(data);

//   if (n == 302 || n == 301)
//   {
//     // 重定向
//     QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
// 
//     QUrl red_url = redirectionTarget.toUrl();
// 
//     QString str = red_url.toString();
// 
//     return ProcessRedirectGet(str);
//   }
//   else
//   {
//     return false;
//   }
}

bool autobots_toutiao::ProcessRedirectGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
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

  if (n != 200)
  {
    return false;
  }

  QByteArray data = reply->readAll();

  return GetPostId(data);

}

bool autobots_toutiao::GetPostId(const QByteArray& arr)
{
  // name="client_id" value="2504490989"
  // name="appkey62" value="42iQjj"
  // name="state" value="f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D"

  HtmlParseUtils html_parse(arr.data(), arr.length());

// "查找appkey62"
  GumboNode* node = html_parse.FirstElementNode("name", "appkey62");
  QString temp1;
  if (node != NULL)
  {
    std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString str = QString::fromStdString(post_from_id);

    if (!str.isEmpty())
    {
       m_appkey = str;
    }
  }

  // "查找client_id"
   node  = html_parse.FirstElementNode("name", "client_id");
  if (node != NULL)
  {
    std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString temp = QString::fromStdString(client_id);

    if (!temp.isEmpty())
    {
      m_client_id = temp;
    }
  }

   node  = html_parse.FirstElementNode("name", "state");
  if (node != NULL)
  {
    std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString temp = QString::fromStdString(client_id);

    if (!temp.isEmpty())
    {
      m_state = temp;
    }
  }

  return true;
}

bool autobots_toutiao::ProcessRedirectLoginGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Host","api.snssdk.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
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

  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str2 = red_url.toString();

    return ProcessRedirectLoginGet2(str2);
  }
  else
  {
    return false;
  }
}

bool autobots_toutiao::ProcessRedirectLoginGet2(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html, application/xhtml+xml, */*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Host","toutiao.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
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

  if (n != 200)
  {
    return false;
  }

  QByteArray data = reply->readAll();

  QString temp_s = data;
  if (temp_s.contains("window.opener.user.connected") 
    || temp_s.contains("window.parent.user.connected") )
  {
    return true;
  }

  return false;
}


bool autobots_toutiao::GetFatieStatus(const QByteArray& byte_arr)
{
  QString temp = byte_arr;

  int first = temp.indexOf("(");
  QString temp2= temp.right(temp.length() - first -1);
  int last = temp2.lastIndexOf(")");
  temp2 = temp2.left(last);

  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toUtf8(), &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    {  
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("result"))  
      {  
        QJsonValue name_value = obj.take("result");
        if(name_value.isObject())
        {
          QJsonObject res_obj = name_value.toObject(); // success

          if (res_obj.contains("status"))
          {
            QJsonObject status_obj = res_obj.take("status").toObject();
            if (status_obj.contains("code"))
            {
              QJsonValue code_value = status_obj.take("code");
              if (code_value.isDouble())
              {
                int res = code_value.toDouble();
                if (res == 0)
                {
                  return true;
                }
              }
            }
          }

//           if (res_obj.contains("id"))
//           {
//             QJsonValue id_value = obj.take("id");
//             if (id_value.isString())
//             {
//               QString comment_id = id_value.toString();
//               if (!comment_id.isEmpty())
//               {
//                 FatieSucceed(comment_id);
//                 return true;
//               }            
//             }
//           }

        }
      }
    }
  }

  return false;
}

bool autobots_toutiao::PreLoginSina(const QString& name, SinaData& data, QString& vcode, QString& code_sign)
{
  //https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=Yzc3ODgyNDczeWFueWUlNDAxNjMuY29t&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=1451282884248
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);
                            //http://login.sina.com.cn/sso/prelogin.php?entry=tech&callback=pluginSSOController.preloginCallBack&su=  &rsakt=mod&checkpin=1&client=ssologin.js(v1.4.18)&_=1451392617550
  QString str_url1 = QString("http://login.sina.com.cn/sso/prelogin.php?entry=tech&callback=pluginSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);
  QUrl url1(str_url1);

  //QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);
  
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",m_url));

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
  
  int n_rand = rand()%9;
  double d = 0.987654321235647/n_rand;
  d = d*100000000;

  QString str_r = QString::number(d, 'f', 0);

  // 获取验证码
  //http://login.sina.com.cn/cgi/pin.php?r=84409877&s=0&p=gz-cfbe02d86c3ac4fe5be776d8034e83e56d20
  //http://login.sina.com.cn/cgi/pin.php?r=54763124&s=0&p=gz-db24c75a1d5b6a51bea82087efb1a2ce12a5
  QString str_url2 = QString("http://login.sina.com.cn/cgi/pin.php?r=%1&s=0&p=%2").arg(str_r, data._pcid);
  QUrl url2(str_url2);
  HttpParamList header_list2;
  header_list2.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list2.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list2.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list2.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list2.push_back(HttpParamItem("Referer",m_url));

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

  if (n != 200)
  {
    return false;
  }

  QByteArray rp_data = reply2->readAll();

//   QVariant var = reply2->header(QNetworkRequest::ContentTypeHeader);
// 
//   QString type = var.toString();

  QImage image_ = QImage::fromData(rp_data);//(data.data(),100,40,QImage::Format_RGB32);

  if (m_code_online)
  {
    // 在线验证
    //bool result = image_.save("e:\\1.jpg");

    VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
    int res = obj->GetRecResults(rp_data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
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
  
//   ValidateDlg dlg(this, image_);
//   if (dlg.exec() == QDialog::Accepted)
//   {
//     vcode = dlg.getCode();
//   }

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
  QString temp2= temp.right(temp.length() - first -1);
  int last = temp2.lastIndexOf(")");
  temp2 = temp2.left(last);

  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toLocal8Bit(), &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    {  
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("retcode"))  
      {  
        QJsonValue name_value = obj.take("retcode");
        int d = name_value.toDouble();
        if (d != 0)
        {
          return false;
        }
      }
      if(obj.contains("servertime"))  
      {  
        QJsonValue name_value = obj.take("servertime");
        double d = name_value.toDouble();
        data._servertime = QString::number(d, 'f', 0);

        if (data._servertime.isEmpty())
        {
          return false;
        }

      }
      if(obj.contains("nonce"))  
      {  
        QJsonValue name_value = obj.take("nonce");
        data._nonce = name_value.toString();
        if (data._nonce.isEmpty())
        {
          return false;
        }
      }
      if(obj.contains("pubkey"))  
      {  
        QJsonValue name_value = obj.take("pubkey");
        data._pubkey = name_value.toString();
        if (data._pubkey.isEmpty())
        {
          return false;
        }
      }
      if(obj.contains("rsakv"))  
      {  
        QJsonValue name_value = obj.take("rsakv");
        data._rsakv = name_value.toString();
        if (data._rsakv.isEmpty())
        {
          return false;
        }
      }
      if(obj.contains("pcid"))  
      {  
        QJsonValue name_value = obj.take("pcid");
        data._pcid = name_value.toString();
      }
      if(obj.contains("showpin"))  
      {  
        QJsonValue name_value = obj.take("showpin");
        int i = name_value.toDouble();
        data._showpin = (i == 0)? false : true;
      }

      return true;
    }
  }

  return false;
}

int autobots_toutiao::LoginSina(SinaData& sina_data,const 
  QString& name, const QString& password,const QString& vcode)
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);
  QString encrypted_pwd = encryptor->EncryptPassword(password, sina_data._servertime, sina_data._nonce, sina_data._pubkey);

  QString str_login_url = QString("http://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.18)&_=%1").arg(GetTimeStr()); 
  //= QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);
  
//   if (sina_data._showpin)
//   {
//     // 需要验证码
//     str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&pcid=%1&ct=1800&s=1&vsnf=1&vsnval=&door=%2&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
//       sina_data._pcid, vcode, m_appkey, encrypted_name, sina_data._servertime,sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
//   }
//   else
//   {
//     // 无需验证码
//     str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&ct=1800&s=1&vsnf=1&vsnval=&door=&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
//       m_appkey, encrypted_name, sina_data._servertime,sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
//   }
  
  QUrl url1(str_login_url);

  //QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);
  
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",m_url));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("cdult","3"));
  post_data.push_back(HttpParamItem("domain","sina.com.cn"));
  post_data.push_back(HttpParamItem("door",vcode));
  post_data.push_back(HttpParamItem("encoding", "UTF-8"));
  post_data.push_back(HttpParamItem("entry","tech"));
  post_data.push_back(HttpParamItem("from",""));
  post_data.push_back(HttpParamItem("gateway","1"));
  post_data.push_back(HttpParamItem("nonce",sina_data._nonce));
  post_data.push_back(HttpParamItem("pagerefer",""));
  post_data.push_back(HttpParamItem("prelt","134"));
  post_data.push_back(HttpParamItem("pwencode","rsa2"));
  post_data.push_back(HttpParamItem("returntype","TEXT"));
  post_data.push_back(HttpParamItem("rsakv",sina_data._rsakv));
  post_data.push_back(HttpParamItem("savestate","30"));
  post_data.push_back(HttpParamItem("servertime",sina_data._servertime));
  post_data.push_back(HttpParamItem("service","sso"));
  post_data.push_back(HttpParamItem("sp", encrypted_pwd));
  post_data.push_back(HttpParamItem("sr","1192*670"));
  post_data.push_back(HttpParamItem("su",encrypted_name));
  post_data.push_back(HttpParamItem("useticket","0"));
  post_data.push_back(HttpParamItem("vsnf","1"));

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
    QString msg = reply->errorString();
    return -2;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 200)
  {
    return -2;
  }

  QByteArray rp_data = reply->readAll();

  int res = GetLoginResult(rp_data,sina_data);

  reply->deleteLater();

  return res;
}

int autobots_toutiao::GetLoginResult(const QByteArray& str, SinaData& data)
  {
    //sinaSSOController.loginCallBack({"
    //retcode":"0","ticket":"ST-MjE4MTM3NTY1MQ==-1451291428-gz-EA95CC25100A66DC457C19BC5DA43316",
    //"uid":"2181375651","nick":"\u7528\u62372181375651",
    //"crossDomainUrlList":["https:\/\/crosdom.weicaifu.com\/sso\/crosdom?action=login&savestate=1451291428",
    //"https:\/\/passport.weibo.cn\/sso\/crossdomain?action=login&savestate=1"]});

    QString temp = str;

    //   if (temp.contains("\"retcode\":\"0\""))
    //   {
    //     return true;
    //   }

    int first = temp.indexOf("(");
    QString temp2= temp.right(temp.length() - first -1);
    int last = temp2.lastIndexOf(")");
    temp2 = temp2.left(last);

    int res = -2;

    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toLocal8Bit(), &json_error); 
    if(json_error.error == QJsonParseError::NoError) 
    {  
      if(parse_doucment.isObject())  
      {  
        QJsonObject obj = parse_doucment.object(); 
        if(obj.contains("retcode"))  
        {  
          QJsonValue name_value = obj.take("retcode");
          int d = name_value.toDouble();
          if (d == 0)
          {
            res = 0;
          }
          else if ( d == 2070)
          {
            res = -1;
          }
        }
        if(obj.contains("ticket"))  
        {  
          QJsonValue name_value = obj.take("ticket");
          data._ticket = name_value.toString();
          if (data._ticket.isEmpty())
          {
            res = -2;
          }
        }
      }
    }

    return res;
  }

bool autobots_toutiao::AuthorizeSina(const SinaData& sina_data)
{
  QString str_url_author = "https://api.weibo.com/oauth2/authorize";

  QUrl url1(str_url_author);
  
  QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"text/html, application/xhtml+xml, */*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
  header_list.push_back(HttpParamItem("Referer", str_temp));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("action","login"));
  post_data.push_back(HttpParamItem("appkey62",m_appkey));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
  post_data.push_back(HttpParamItem("display","default"));
  post_data.push_back(HttpParamItem("from","5"));
  post_data.push_back(HttpParamItem("isLoginSina",""));
  post_data.push_back(HttpParamItem("passwd",""));
  post_data.push_back(HttpParamItem("quick_auth","null"));
  post_data.push_back(HttpParamItem("redirect_uri","http://api.snssdk.com/auth/login_success/"));
  post_data.push_back(HttpParamItem("regCallback",str_temp));
  post_data.push_back(HttpParamItem("response_type","code"));
  post_data.push_back(HttpParamItem("scope",""));
  post_data.push_back(HttpParamItem("state",m_state));
  post_data.push_back(HttpParamItem("switchLogin","0"));
  post_data.push_back(HttpParamItem("ticket",sina_data._ticket));
  post_data.push_back(HttpParamItem("userId",""));
  post_data.push_back(HttpParamItem("verifyToken","null"));
  post_data.push_back(HttpParamItem("withOfficalAccount",""));
  post_data.push_back(HttpParamItem("withOfficalFlag","0"));

  QNetworkReply* reply = network.PostRequest_ssl(url1, header_list,post_data);

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
    return false;
  }
}

bool autobots_toutiao::GetQrid(const QByteArray& rp_data, QString& qrid)
{

  //"window.STK_14514005525521 && STK_14514005525521({"retcode":20000000,"msg":"succ",
//   "data":{"qrid":"QRID-gz-1AdVAm-14MDap-hTfxFlFtpg40aJ9uV63jVK7397d7",
//     "image":"http:\/\/qr.weibo.cn\/inf\/gen?api_key=a0241ed0d922e7286303ea5818292a76
//     &data=https%3A%2F%2Fpassport.weibo.cn%2Fsignin%2Fqrcode%2Fscan%3Fqr%3DQRID-gz-1
//     AdVAm-14MDap-hTfxFlFtpg40aJ9uV63jVK7397d7&datetime=1451400554&deadline=0
//     &level=M&logo=http%3A%2F%2Fu1.sinaimg.cn%2Fupload%2F2014%2F05%2F27%2F
//     weibo-logo.png&output_type=img&redirect=0
//     &sign=fb9dbd8ee7746a00bb41522995da83a9&size=128&start_time=0&title=sso&type=url"}});"

  QString temp = rp_data;

  if (!temp.contains("qrid"))
  {
    return false;
  }

  int pos1 = temp.indexOf("qrid");
  QString sub_string = temp.right(temp.length() - pos1 -1);
  pos1 = sub_string.indexOf(":");
  sub_string = sub_string.right(sub_string.length() - pos1-2 );
  pos1 = sub_string.indexOf("\"");
  sub_string = sub_string.left(pos1);
  qrid = sub_string;
  return true;



//   int first = temp.indexOf("(");
//   QString temp2= temp.right(temp.length() - first -1);
//   int last = temp2.lastIndexOf(")");
//   temp2 = temp2.left(last);

  //QJsonParseError json_error;
  //QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toLocal8Bit(), &json_error); 
  //if(json_error.error == QJsonParseError::NoError) 
  //{  
  //  if(parse_doucment.isObject())  
  //  {  
  //    QJsonObject obj = parse_doucment.object(); 
  //    if(obj.contains("retcode"))  
  //    {  
  //      QJsonValue name_value = obj.take("retcode");
  //      int d = name_value.toDouble();
  //      if (d != 20000000)
  //      {
  //        return false;
  //      }
  //    }
  //    if(obj.contains("data"))  
  //    {  
  //      QJsonValue _data = obj.take("data");
  //      if (_data.isObject())
  //      {
  //        QJsonObject data_obj = _data.toObject();
  //        if (data_obj.contains("qrid"))
  //        {
  //          QJsonValue name_value = obj.take("qrid");
  //          qrid = name_value.toString();
  //          return true;
  //        }
  //      }
  //    }
  //  }
  //}

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
  DailConnector connector("VPN","ycc1","111");

  for (int i = 0; i < m_comment_list.size(); ++i)
  {
    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

    network.GetManager().setCookieJar(cookie);

    EmitMsgStatusBar(QStringLiteral("拨号中..."));
    QString dial_msg;
    while(!connector.ReConnect(dial_msg))
    {
      EmitMsgStatusBar(QStringLiteral("拨号失败：")+ dial_msg );
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

    // 尝试登陆
    bool login_status = false;
    while (m_account_order < m_account_list.size())
    {
      if (!RequestForLogin())
      {
        ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
        continue;
      }

      AccountParam ac = m_account_list.at(m_account_order);

      if (!AuthorBySina(ac._id, ac._password))
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

// void autobots_toutiao::AutoFatie()
// {
//   VpnConnector vpn;
// 
//   QString msg;
//   bool b = vpn.initVpnApi(msg);
// 
//   if (!b)
//   {
//     ui.lineEdit_msg->setText(msg);
//     return;
//   }
// 
//   for (int i = 0; i < m_comment_list.size(); ++i)
//   {
//     QNetworkCookieJar* cookie = new QNetworkCookieJar();
// 
//     network.GetManager().setCookieJar(cookie);
// 
//     b = vpn.ConnectToVpn(msg);
// 
//     int ncount = 0;
//     while (!GetContent() && ncount < 15)
//     {
//       vpn.ConnectToVpn(msg);
//       //ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
//       //break;
//       ncount++;
//     }
// 
//     if (ncount >= 15)
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
//       break;
//     }
// 
//     // 尝试登陆
//     bool login_status = false;
//     while (m_account_order < m_account_list.size())
//     {
//       if (!RequestForLogin())
//       {
//         ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
//         continue;
//       }
// 
//       AccountParam ac = m_account_list.at(m_account_order);
// 
//       if (!AuthorBySina(ac._id, ac._password))
//       {
//         ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
//         //m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
//         ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255,0,0, 180));
//         m_account_order++;
//         continue;
//       }
//       else
//       {
//         ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
//         m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
//         ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0,255,0, 180));
//         m_account_order++;
//         login_status = true;
//         break;
//       }
//     }
// 
//     if (!login_status)
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("1、网络无连接 \n 2、号登陆不成功，无法继续"));
//       return;
//     }
// 
//     QString msg;
//     msg.setNum(m_comment_order+1);
// 
//     QElapsedTimer t2;
//     t2.start();
//     while(t2.elapsed()<1000 )  
//       QCoreApplication::processEvents();
// 
//     if (DoPostFatie(m_comment_list[m_comment_order]))
//     {
//       m_comment_item_list[m_comment_order]->setCheckState(Qt::Checked);
//       ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,已完成"));
//       m_comment_order++;
//     }
//     else
//     {
//       ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("条,失败"));
//       m_comment_order++;
//     }
// 
//     t2.restart();
//     while(t2.elapsed()<1000 )  
//       QCoreApplication::processEvents();
// 
//     Logout();
//   }
// 
// 
// 
// 
//   //cookie->deleteLater();
// }

