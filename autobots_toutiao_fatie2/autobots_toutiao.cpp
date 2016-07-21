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


const int TIMEOUT = 20*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),
    m_account_order(0),
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

    //connect(&network, SIGNAL(postIdReady(QString)), this, SLOT(onPostIdReday(QString)));

    //connect(&network, SIGNAL(clientID(QString)), this, SLOT(onClientIdReday(QString)));

    //connect(&network, SIGNAL(csrftokenReady(QString)), this, SLOT(onCsrfTokenReday(QString)));

    //connect(&network, SIGNAL(loginsucceed(QString)), this, SLOT(onLoginsucceed(QString)));

   // connect(&network, SIGNAL(fatiesucceed(QString, double)), this, SLOT(onFatieSucceed(QString, double)));

    //connect(&network, SIGNAL(newCommentID(double)), this, SLOT(onNewCommentID(QString)));
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
      QNetworkCookieJar* cookie = new QNetworkCookieJar();

      network.GetManager().setCookieJar(cookie);

      // 获取CSRF TOKEN
      GetContent();
      if (m_csrf_token.isEmpty())
      {
        ui.lineEdit_msg->setText(QStringLiteral("连接网站中..."));
        continue;
      }

      if (!RequestForRenren())
      {
        ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
        continue;
      }

      AccountParam ac = m_account_list.at(m_account_order);

      if (!AuthorByRenren(ac._id, ac._password))
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
      return;
    }

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<1000 )  
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
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    Logout();

    QString msg;
    msg.setNum(i+1);

    ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("次完成"));

    t2.restart();
    while(t2.elapsed()< m_interval*1000 )  // 时间间隔
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
  // http://toutiao.com/group/6222456302762934530/post_comment/
     http://toutiao.com/group/6226929286129942786/post_comment/
  QString str_url1 = "http://toutiao.com/group/" + m_group_id + "/post_comment/";

  QUrl url1;
  url1.setUrl(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
  header_list.push_back(HttpParamItem("Host", "toutiao.com"));
  header_list.push_back(HttpParamItem("Origin", "http://toutiao.com"));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("format", "json"));
  post_data.push_back(HttpParamItem("group_id", m_group_id));
  post_data.push_back(HttpParamItem("item_id", m_news_id));
  post_data.push_back(HttpParamItem("status", content));

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

bool autobots_toutiao::GetContent()
{
  QString str_url_1 = m_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"text/html, application/xhtml+xml, */*"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
//  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "toutiao.com"));
//  header_list1.push_back(HttpParamItem("Referer", "http://toutiao.com/"));
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

  bool res = GetCsrfToken(rp->readAll());

  if (rp!=NULL)
    rp->deleteLater();

  return  res;
}

bool autobots_toutiao::RequestForRenren()
{
  QString str_url1 = "http://toutiao.com/auth/connect/?type=toutiao&platform=renren_sns";

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "toutiao.com"));
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

bool autobots_toutiao::AuthorByRenren(const QString& name, const QString& password)
{
  //1.检验验证码
  QString vcode, code_sign;
  bool need_code = NeedValidateCode(name, vcode, code_sign);
  QString str_need_code = need_code ? "true" : "";

  QString str_url1 = "https://graph.renren.com/oauth/grant";

  QUrl url1(str_url1);
    
  QString str_temp = "http://graph.renren.com/oauth/grant?client_id=" + m_client_id + "&redirect_uri=http://api.snssdk.com/auth/login_success/&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000";

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
  header_list.push_back(HttpParamItem("Referer", str_temp));
  //Referer	
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("authFeed","true"));
  post_data.push_back(HttpParamItem("authorizeOrigin","00000"));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
  post_data.push_back(HttpParamItem("display","page"));
  post_data.push_back(HttpParamItem("follow","true"));
  post_data.push_back(HttpParamItem("icode",vcode));
  post_data.push_back(HttpParamItem("isNeedIcode",str_need_code));
  post_data.push_back(HttpParamItem("login_type","false"));
  post_data.push_back(HttpParamItem("password",password));
  post_data.push_back(HttpParamItem("porigin","80100"));
  post_data.push_back(HttpParamItem("post_form_id", m_post_id));
  post_data.push_back(HttpParamItem("redirect_uri","http://api.snssdk.com/auth/login_success/"));
  post_data.push_back(HttpParamItem("response_type","code"));
  post_data.push_back(HttpParamItem("scope","status_update photo_upload create_album"));
  post_data.push_back(HttpParamItem("secure","true"));
  post_data.push_back(HttpParamItem("state","renren_sns__0____toutiao____2__0__24"));
  post_data.push_back(HttpParamItem("username",name));

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
    reply->deleteLater();
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  bool res = false;
  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    int r = ProcessRedirectLoginGet(str);

    if (r == 0)
    {
      res = true;
    }
    else if (r == -2)
    {
      // 验证码错误
      VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
      obj->ReportError("bestsalt",code_sign);
    }
  }
  else
  {
    res = false;
  }

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
    QString name = ui.tableWidget_account_id->item(i, 0)->text();
    QString password = ui.tableWidget_account_id->item(i, 1)->text();
    m_account_list.push_back(AccountParam(name,password));
    m_account_row_list.push_back(i);
  }

}

void autobots_toutiao::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
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
  QString str_url_1 = "http://web.toutiao.com/auth/logout/";

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  ////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "toutiao.com"));
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
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  //header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
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

  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str2 = red_url.toString();

    return ProcessRedirectGet(str2);
  }
  else
  {
    return false;
  }
}

bool autobots_toutiao::ProcessRedirectGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
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
  HtmlParseUtils html_parse(arr.data(), arr.length());

  // "查找post_from_id"
  GumboNode* node = html_parse.FirstElementNode("name", "post_form_id");

  if (node != NULL)
  {
    std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString str = QString::fromStdString(post_from_id);

    m_post_id = str;
  }

  // "查找client_id"
  node = html_parse.FirstElementNode("name", "client_id");

  if (node != NULL)
  {
    std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

    m_client_id = QString::fromStdString(client_id);
  }

  if (m_post_id.isEmpty() || m_client_id.isEmpty())
  {
    return false;
  }

  return true;
}

// 0 正常，-1 未知错误， -2 验证码错误
int autobots_toutiao::ProcessRedirectLoginGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","api.snssdk.com"));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

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
    if(reply != NULL)
    {
      QString t = reply->errorString();

      int n = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

      t = reply->readAll();
      //reply->deleteLater();
      return ProcessRedirectLoginGetTemp(str);
    }
    else
    {
      return -1;
    }   
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  int res = -1;
  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str2 = red_url.toString();

    res = ProcessRedirectLoginGet2(str2) ? 0 : -1;
  }
  else if (n == 302)
  {
    QString str2 = reply->readAll();
    if (str2.contains(QStringLiteral("验证码错误")))
    {
      res = -2;
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
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
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

  QString temp_s = data;
  if (temp_s.contains("window.opener.user.connected") 
    || temp_s.contains("window.parent.user.connected") )
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
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","api.snssdk.com"));
  header_list.push_back(HttpParamItem("Referer","http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
  //header_list.push_back(HttpParamItem("",""));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

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

  if (reply == NULL || _timeout)
  {
   
    reply->deleteLater();
    return -1;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  int n = statusCodeV.toInt();

  if ((reply->error() != QNetworkReply::NoError) && n != 500)
  {
     QString t = reply->errorString();
     return -1;
  }
  
  int res = -1;
  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str2 = red_url.toString();

    res = ProcessRedirectLoginGet2(str2) ? 0 : -1;
  }
  else if (n == 302)
  {
    QString str2 = reply->readAll();
    if (str2.contains(QStringLiteral("验证码错误")))
    {
      res = -2;
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
  int pos2 = str.indexOf("&",pos1);
  QString t = str.mid(pos1, pos2-pos1);
  QString s_url = "http://toutiao.com/auth/connected/?state=renren_sns__0____toutiao____2__0__24&" + t;

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","toutiao.com"));
  //header_list.push_back(HttpParamItem("Referer","http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
  //header_list.push_back(HttpParamItem("",""));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  //header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

  QNetworkReply* reply = network.GetRequest(QUrl(s_url), header_list);

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

  if (reply == NULL || _timeout )
  {
   
    reply->deleteLater();
    return -1;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
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
    || temp_s.contains("window.parent.user.connected") )
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
          QString str_value = name_value.toString(); // success
          if (str_value == "success")
          {
            // 发帖成功
            if(obj.contains("comment"))  
            { 
              QString comment_str;
              QString comment_id ;
              QJsonValue comment_obj = obj.take("comment");
              if(comment_obj.isObject())
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
                FatieSucceed(comment_str,comment_id);
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
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",str_temp));
  //network.GetManager().setCookieJar(new QNetworkCookieJar(this));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("authFeed","true"));
  post_data.push_back(HttpParamItem("authorizeOrigin","00000"));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
  post_data.push_back(HttpParamItem("display","page"));
  post_data.push_back(HttpParamItem("follow","true"));
  post_data.push_back(HttpParamItem("icode",""));
  post_data.push_back(HttpParamItem("isNeedIcode",""));
  post_data.push_back(HttpParamItem("login_type","false"));
  post_data.push_back(HttpParamItem("password",""));
  post_data.push_back(HttpParamItem("porigin","80100"));
  post_data.push_back(HttpParamItem("post_form_id", m_post_id));
  post_data.push_back(HttpParamItem("redirect_uri","http://api.snssdk.com/auth/login_success/"));
  post_data.push_back(HttpParamItem("response_type","code"));
  post_data.push_back(HttpParamItem("scope","status_update photo_upload create_album"));
  post_data.push_back(HttpParamItem("secure","true"));
  post_data.push_back(HttpParamItem("state","renren_sns__0____toutiao____2__0__24"));
  post_data.push_back(HttpParamItem("username",name));

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

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

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
  header_list2.push_back(HttpParamItem("Connection","Keep-Alive"));
//  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list2.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list2.push_back(HttpParamItem("Host", "icode.renren.com"));
  header_list2.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list2.push_back(HttpParamItem("Referer",str_temp));

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

  statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

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

void autobots_toutiao::WaitforSeconds(int nseconds)
{
  QTime t2;
  t2.restart();
  while(t2.elapsed() < nseconds*1000 )  
    QCoreApplication::processEvents();
}