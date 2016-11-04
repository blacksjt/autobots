#include "autobots_toutiao.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>

const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 1*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),
    m_client_id("394e2173327e4ead8302dc27f4ae8879")
{
    ui.setupUi(this);

    initialize();

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    //connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

    //connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    //connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));

    connect(&network, SIGNAL(postIdReady(QString)), this, SLOT(onPostIdReday(QString)));

    connect(&network, SIGNAL(clientID(QString)), this, SLOT(onClientIdReday(QString)));

    connect(&network, SIGNAL(csrftokenReady(QString)), this, SLOT(onCsrfTokenReday(QString)));

    connect(&network, SIGNAL(loginsucceed(QString)), this, SLOT(onLoginsucceed(QString)));

    connect(&network, SIGNAL(votesucceed()), this, SLOT(onVotesucceed()));

    connect(&network, SIGNAL(resultsucceed(double)), this, SLOT(onResultChanged(double)));
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
  
  control_status = true;
  m_b_post_id = false;
  m_b_csrf = false;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    if (!control_status)
    {
      break;
    }

    QNetworkCookieJar* cookie = new QNetworkCookieJar();

    network.GetManager().setCookieJar(cookie);

    // 获取CSRF TOKEN
    QNetworkReply* rp = GetContent();

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

    //rp->deleteLater();

    if (_timeout || rp->error() != QNetworkReply::NoError) 
    {
      continue;
    }


    AccountParam ac = m_account_list.at(i);
    QNetworkReply* rp2 = LogInbyRenren(ac._id, ac._password);

    _t.restart();

    _timeout = false;

    while (rp2&& !rp2->isFinished())
    {
      QCoreApplication::processEvents();
      if (_t.elapsed() >= TIMEOUT) {
        _timeout = true;
        break;
      }
    }

    if (!_timeout && (rp2&& rp2->error() == QNetworkReply::NoError)) 
    {
      // 登陆成功
      DoAction();
    }

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<3000 )  
      QCoreApplication::processEvents();

    Logout();

    QString msg;
    msg.setNum(i);

    //cookie->deleteLater();

    ui.lineEdit_msg->setText(QStringLiteral("第") + msg + QStringLiteral("次"));
  }

  ui.lineEdit_msg->setText(QStringLiteral("已完成"));
}

QNetworkReply* autobots_toutiao::LogInbyRenren(const QString& name, const QString& password)
{
  //1 请求使用人人网登陆
  QNetworkReply* reply = RequestForRenren();

  QTime _t;
  _t.start();

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      break;
    }
  }

  _t.restart();

  bool _timeout = false;

  while(!m_b_post_id)
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (!m_b_post_id)
  {
    return NULL;
  }

  //3 授权请求

  return AuthorByRenren(name, password); 
}



void autobots_toutiao::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

int autobots_toutiao::DoAction()
{
  QString str_url1 = "http://toutiao.com/vote/log/add/";

  QUrl url1;
  url1.setUrl(str_url1);

   HttpParamList header_list;
   header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
   header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
   header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
   header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
   header_list.push_back(HttpParamItem("Content-Type", "application/json"));
   header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
   header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
   header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
   header_list.push_back(HttpParamItem("Referer", m_url));
   header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

   QJsonObject obj;
   obj.insert("id", m_vote_id);
   //obj.insert("value", QString("[\"1\"]"));

   QJsonArray arr1;
   QStringList vote_list = m_vote_order.split(",");
   foreach(QString vote, vote_list)
   {
     arr1.push_back(QJsonValue(vote));
   }
    
   //arr1.insert(0, m_vote_order);
   obj.insert("value", arr1);

   QJsonDocument doc1(obj);
   QString temp2 = doc1.toJson(QJsonDocument::Compact);

   QNetworkReply* reply = network.PostRequest_json(url1, header_list,  doc1);

   QTime _t;
   _t.start();
 
   while(reply && !reply->isFinished())
   {
     QCoreApplication::processEvents();
     if (_t.elapsed() >= TIMEOUT1) {
       break;
     }
   }

   GetVoteStatus();
  return 0;
}

QNetworkReply* autobots_toutiao::GetContent()
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
  header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", "http://toutiao.com/"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  return network.GetRequest(url_1, header_list1);

}

QNetworkReply* autobots_toutiao::RequestForRenren()
{
  QString str_url1 = "http://toutiao.com/auth/connect/?type=toutiao&platform=renren_sns";

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  return network.GetRequest(url1, header_list);
}


QNetworkReply* autobots_toutiao::AuthorByRenren(const QString& name, const QString& password)
{
  //1.
  QString str_url1 = "https://graph.renren.com/oauth/grant";

  QUrl url1(str_url1);
    
  QString str_temp = "http://graph.renren.com/oauth/grant?client_id=" + m_client_id + "&redirect_uri=http://api.snssdk.com/auth/login_success/&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000";

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
  header_list.push_back(HttpParamItem("Referer", str_temp));
  //Referer	
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("authFeed","true"));
  post_data.push_back(HttpParamItem("authorizeOrigin","00000"));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
//  post_data.push_back(HttpParamItem("display","page"));
  post_data.push_back(HttpParamItem("follow","true"));
  //post_data.push_back(HttpParamItem("icode",""));
//  post_data.push_back(HttpParamItem("isNeedIcode",""));
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


  QNetworkReply* rp = network.PostRequest_ssl(url1, header_list,post_data);

  rp->ignoreSslErrors();

  return rp;

}


void autobots_toutiao::onPostIdReday(QString post_id)
{
  m_post_id = post_id ;
  m_b_post_id = true;
}

void autobots_toutiao::onClientIdReday(QString _id)
{
  m_client_id = _id ;
}

void autobots_toutiao::onAddCommentID()
{
  
//   QString id = ui.lineEdit_comment_id->text();
// 
//   ui.lineEdit_comment_id->setText("");
// 
//   AddToTree(id);
}

void autobots_toutiao::onCsrfTokenReday(QString str)
{
  m_csrf_token = str;
  m_b_csrf = true;
}

void autobots_toutiao::onLoginsucceed(QString id)
{
  ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
  DoAction();
}

void autobots_toutiao::UpdateData()
{
  m_url = ui.lineEdit_url->text();
  m_group_id = ui.lineEdit_page_id->text();
  m_vote_id = ui.lineEdit_vote_id->text();
  m_vote_order = ui.lineEdit_vote_order->text();
  m_account_list.clear();

  int n = ui.tableWidget_account_id->rowCount();

  for (int i = 0 ; i < n; ++i)
  {
    QString name = ui.tableWidget_account_id->item(i, 0)->text();
    QString password = ui.tableWidget_account_id->item(i, 1)->text();
    m_account_list.push_back(AccountParam(name,password));
  }

  network.setVote_id(m_vote_id);
  network.setVote_order(m_vote_order);

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
  header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", m_url));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* reply_1 = network.GetRequest(url_1, header_list1);
}

void autobots_toutiao::onActImportComment()
{
  //QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  //if (path.isEmpty())  //判断用户是否选择了文件
  //{
  //  //emitLogMessage(QStringLiteral("取消打开文件"));
  //  return;
  //}

  //QFile file(path);

  //if (!file.open(QIODevice::ReadOnly)) {    
  //  QMessageBox::critical(NULL, "提示", "无法打开文件");  
  //  return ;    
  //}  

  //QTextStream text_stream(&file);
  ////QString str = text_stream.readAll();
  //int n_row = 0;
  //ui.treeWidget_comment_id->clear();

  //while(!text_stream.atEnd())
  //{
  //  QString line_str = text_stream.readLine();

  //  AddToTree(line_str);
  //}

  //file.close();
}

void autobots_toutiao::onActClearComments()
{
  //ui.treeWidget_comment_id->clear();
}

void autobots_toutiao::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
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

  if (ui.lineEdit_vote_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("评论ID没有输入")); 
    return false;
  }

   if (ui.lineEdit_vote_order->text().isEmpty())
   {
     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有输入投票的选项，从1开始投第几个")); 
     return false;
   }

  if (ui.tableWidget_account_id->rowCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请导入账号")); 
    return false;
  }

  return true;
}

void autobots_toutiao::onVotesucceed()
{
  ui.lineEdit_msg->setText(QStringLiteral("投票成功"));
}

void autobots_toutiao::AddToTree(const QString& text)
{
  //
}

void autobots_toutiao::onResultChanged(double count)
{
  QString msg = QString::number(count, 'f', 0);
  ui.lineEdit_msg->setText(QStringLiteral("当前票数:") + msg);
}

void autobots_toutiao::GetVoteStatus()
{
  QString str_url2 = "http://toutiao.com/vote/log/stat/";

  QUrl url2;
  url2.setUrl(str_url2);

   HttpParamList header_list;
   header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
   header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
   header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
   header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
   header_list.push_back(HttpParamItem("Content-Type", "application/json"));
   header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
   header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
   header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
   header_list.push_back(HttpParamItem("Referer", m_url));
   header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QJsonObject obj2;
  QJsonArray arr2;
  arr2.insert(0,m_vote_id);
  obj2.insert("id_list", arr2);
  QJsonDocument doc2(obj2);
  //QString temp2 = doc2.toJson(QJsonDocument::Compact);

  QNetworkReply* reply2 = network.PostRequest_json(url2, header_list, doc2);

  if (reply2 && (reply2->error() != QNetworkReply::NoError))
  {
    return;
  }

  QTime _t;
  _t.start();

  //while(!reply2->isFinished())
  //{
  //  QCoreApplication::processEvents();
  //  if (_t.elapsed() >= TIMEOUT1) {
  //    break;
  //  }
  //}
  while (_t.elapsed() < 1000)
  {
    QCoreApplication::processEvents();
  }
}
