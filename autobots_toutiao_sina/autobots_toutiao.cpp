#include "autobots_toutiao.h"
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


const int TIMEOUT = 20*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : QMainWindow(parent),
    m_account_order(0),control_status(true)
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
//   QString str;
//   SinaData data;
//   bool r= PreLoginSina("c77882473yanye@163.com",data,str);

  // 检查输入
  if (!CheckInput())
  {
    return;
  }

  // 更新界面输入
  UpdateData();
  
  if (m_account_list.size() < m_comment_list.size())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("用户账户太少")); 
    return;
  }

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

      if (!RequestForSina())
      {
        ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
        continue;
      }

      AccountParam ac = m_account_list.at(m_account_order);

      if (!AuthorBySina(ac._id, ac._password))
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

    t2.restart();
    while(t2.elapsed()< m_interval*1000 )  // 时间间隔
    {
      ui.lineEdit_msg->setText(QString::number(t2.elapsed()));
      QCoreApplication::processEvents();   
    }

    ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("次"));
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
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
  header_list.push_back(HttpParamItem("Host", "toutiao.com"));
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
     return GetFatieStatus(rp_data); 
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
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", "http://toutiao.com/"));
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

bool autobots_toutiao::RequestForSina()
{
  QString str_url1 = "http://toutiao.com/auth/connect/?type=toutiao&platform=sina_weibo";

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Accept","text/html, application/xhtml+xml, */*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
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

bool autobots_toutiao::AuthorBySina(const QString& name, const QString& password)
{
  //1.检验验证码
  QString vcode,code_sign;
  SinaData sina_data;
  if (!PreLoginSina(name, sina_data, vcode,code_sign))
  {
    ui.lineEdit_msg->setText(QStringLiteral("获取验证码失败"));
    return false;
  }

  if (sina_data._showpin && vcode.isEmpty())
  {
    return false;
  }

  // 2.登陆
  if (!LoginSina(sina_data,name,password,vcode))
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

  m_client_id = "2504490989";
  m_appkey= "42iQjj";
  m_state = "f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D";

  m_code_online = true;
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
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
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

bool autobots_toutiao::PreLoginSina(const QString& name, SinaData& data, 
  QString& vcode, QString& code_sign)
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);

  QString str_url1 = QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);
  QUrl url1(str_url1);

  QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://service.zol.com.cn/user/api/sina/callback.php&response_type=code").arg(m_client_id, m_state);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  //  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",str_temp));

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
  //https://login.sina.com.cn/cgi/pin.php?r=54763124&s=0&p=gz-db24c75a1d5b6a51bea82087efb1a2ce12a5
  QString str_url2 = QString("https://login.sina.com.cn/cgi/pin.php?r=%1&s=0&p=%2").arg(str_r, data._pcid);
  QUrl url2(str_url2);
  HttpParamList header_list2;
  header_list2.push_back(HttpParamItem("Connection","Keep-Alive"));
  //  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list2.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list2.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list2.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list2.push_back(HttpParamItem("Referer",str_temp));

  QNetworkReply* reply2 = network.GetRequest_ssl(url2, header_list2);

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

bool autobots_toutiao::LoginSina(SinaData& sina_data,const 
  QString& name, const QString& password,const QString& vcode)
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);
  QString encrypted_pwd = encryptor->EncryptPassword(password, sina_data._servertime, sina_data._nonce, sina_data._pubkey);

  QString str_login_url; //= QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);
  
  if (sina_data._showpin)
  {
    // 需要验证码
    str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&pcid=%1&ct=1800&s=1&vsnf=1&vsnval=&door=%2&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
      sina_data._pcid, vcode, m_appkey, encrypted_name, sina_data._servertime,sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
  }
  else
  {
    // 无需验证码
    str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&ct=1800&s=1&vsnf=1&vsnval=&door=&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
      m_appkey, encrypted_name, sina_data._servertime,sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
  }
  
  QUrl url1(str_login_url);

  QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);
  
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",str_temp));

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

  QByteArray rp_data = reply->readAll();

  bool res = GetLoginResult(rp_data,sina_data);

  reply->deleteLater();

  return res;
}

bool autobots_toutiao::GetLoginResult(const QByteArray& str, SinaData& data)
{
  //sinaSSOController.loginCallBack({"
  //retcode":"0","ticket":"ST-MjE4MTM3NTY1MQ==-1451291428-gz-EA95CC25100A66DC457C19BC5DA43316",
  //"uid":"2181375651","nick":"\u7528\u62372181375651",
  //"crossDomainUrlList":["https:\/\/crosdom.weicaifu.com\/sso\/crosdom?action=login&savestate=1451291428",
  //"https:\/\/passport.weibo.cn\/sso\/crossdomain?action=login&savestate=1"]});

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
      if(obj.contains("ticket"))  
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
