#include "auto_ithome.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>

const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 1*1000;

auto_ithome::auto_ithome(QWidget *parent)
    : control_status(true),QMainWindow(parent)
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

auto_ithome::~auto_ithome()
{

}

void auto_ithome::onStart()
{
  // 检查输入
  if (!CheckInput())
  {
    return;
  }

  // 更新界面输入
  UpdateData();
  
  control_status = true;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    if (!control_status)
    {
      break;
    }

    QNetworkCookieJar* cookie = new QNetworkCookieJar();

    network.GetManager().setCookieJar(cookie);

    // 
    GetContent();

    do 
    {
      if (RequestForLogin())
      {
        break;
      }
      else
      {
        ui.lineEdit_msg->setText(QStringLiteral("连接网站中，请更换IP尝试..."));
        QElapsedTimer t;
        t.start();
        while(t.elapsed()< 5000 )  
          QCoreApplication::processEvents();
      }
    } while (true);

    AccountParam ac = m_account_list.at(i);

    if (!Login(ac._id, ac._password))
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
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    Logout();

    cookie->deleteLater();
  }

  ui.lineEdit_msg->setText(QStringLiteral("已完成"));
}

void auto_ithome::Logout()
{
  QString str_url_1 = "http://www.ithome.com/ithome/login.aspx/btnLogout_Click";

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list1.push_back(HttpParamItem("Host", m_host));
  header_list1.push_back(HttpParamItem("Referer", m_url));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  HttpParamList post_data;


  QNetworkReply* reply_1 = network.PostRequest(url_1, header_list1,post_data);

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

void auto_ithome::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool auto_ithome::DoAction()
{
  QString str_url1 = "http://www.ithome.com/ithome/postComment.aspx";

  QUrl url1;
  url1.setUrl(str_url1);

  for(int i = 0; i <m_comment_list.size(); ++i)
  {
    QString str = m_comment_list[i];

    HttpParamList header_list;
    header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//    header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
    header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
    header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
//    header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
    header_list.push_back(HttpParamItem("Host", m_host));
    header_list.push_back(HttpParamItem("Referer", m_url));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

    HttpParamList post_data;
    post_data.push_back(HttpParamItem("commentid", str));
    post_data.push_back(HttpParamItem("type", "loginReplyVote"));
    post_data.push_back(HttpParamItem("typeid", "1"));

    QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

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

    QByteArray rp_data = reply->readAll();
    QString data = rp_data;
    bool ok;
    int count = data.toInt(&ok);

    if (ok)
    {
       m_comment_item_list[i]->setText(1, data);
    }
    else
    {
      QString msg = QString::fromLocal8Bit(rp_data.constData());
      ui.lineEdit_msg->setText(msg);
    }

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()< 3000 )  
      QCoreApplication::processEvents();

  }

  return true;
}

bool auto_ithome::GetContent()
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
  header_list1.push_back(HttpParamItem("Host", m_host));
  //header_list1.push_back(HttpParamItem("Referer", m_url));
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

  //bool res = GetCsrfToken(rp->readAll());

  if (rp!=NULL)
    rp->deleteLater();

  return  true;

}

bool auto_ithome::RequestForLogin()
{
  QString str_url1 = "http://www.ithome.com/ithome/divlogin.aspx";

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", m_host));
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

  bool res = (n == 200 ? true : false);

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}


bool auto_ithome::Login(const QString& name, const QString& password)
{
  QString str_url1 = "http://www.ithome.com/ithome/login.aspx/btnLogin_Click";
  QUrl url1(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept","application/json, text/javascript, */*; q=0.01"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Content-Type", "application/json; charset=utf-8"));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  //Referer	
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QJsonObject obj;

  obj.insert("username", name);
  obj.insert("password", password);
 
  QJsonDocument doc1(obj);
  QString temp2 = doc1.toJson();

  QNetworkReply* reply = network.PostRequest_json(url1, header_list,  doc1);

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

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

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

void auto_ithome::onAddCommentID()
{
  
  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  AddToTree(id);
}

void auto_ithome::UpdateData()
{
  m_url = ui.lineEdit_url->text();
  m_host = ui.lineEdit_host->text();
  m_comment_list.clear();
  m_account_list.clear();
  m_comment_item_list.clear();
  m_account_row_list.clear();

  int n = ui.treeWidget_comment_id->topLevelItemCount();

  for (int i = 0 ; i < n; ++i)
  {
    QString text = ui.treeWidget_comment_id->topLevelItem(i)->text(0);
    m_comment_list.push_back(text);
    m_comment_item_list.push_back(ui.treeWidget_comment_id->topLevelItem(i));
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

void auto_ithome::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
}

void auto_ithome::onActFromTxt()
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
  m_account_row_list.clear();
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

void auto_ithome::onActImportComment()
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
  m_comment_item_list.clear();
  m_comment_list.clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();

    AddToTree(line_str);
  }

  file.close();
}

void auto_ithome::onActClearComments()
{
  ui.treeWidget_comment_id->clear();
  m_comment_list.clear();
  m_comment_item_list.clear();
}

void auto_ithome::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  m_account_list.clear();
  m_account_row_list.clear();
}

bool auto_ithome::CheckInput()
{
  if (ui.lineEdit_url->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_host->text().isEmpty())
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

  return true;
}

void auto_ithome::AddToTree(const QString& text)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget_comment_id);
  item->setText(0,text);
  ui.treeWidget_comment_id->addTopLevelItem(item);
}

bool auto_ithome::UpdateDiggCount(const QByteArray& data, int& count_)
{
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
            double comment_id = 0;
            if(obj.contains("digg_count"))  
            {
              QJsonValue count_value = obj.take("digg_count");
              count_ = count_value.toDouble();
              return true;
            }

          }
        }
      }
    }
  }

  return false;
}

bool auto_ithome::GetLoginResult(const QByteArray& data)
{
  //{"d":"ok:salt"}
  //{"d":"ok:姐多久额哦"}
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())
    {
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("d"))  
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
