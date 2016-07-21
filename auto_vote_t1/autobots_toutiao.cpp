#include "autobots_toutiao.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "code_dlg.h"
#include "vlidatecodeonline.h"

const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 10*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),
    m_client_id("394e2173327e4ead8302dc27f4ae8879")
{
    ui.setupUi(this);

    initialize();

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    //connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    //connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

    //connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    //connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    //connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));

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
  control_status = true;

  //QString interval = ui.lineEdit_interval->text();
  int nseconds = 10;//interval.toInt();
  int count = 0;

  while(control_status)
  {
    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

    network.GetManager().setCookieJar(cookie);

    QString temp = QStringLiteral("第");

    count ++; 

    QString temp2;
    temp2.setNum(count);

    ui.lineEdit_msg->setText(temp + temp2);

    GetContent();

    // 1.获取验证码
    QString pid;
    QString str = GetValidateCode(pid);

//     QElapsedTimer t;
//     t.start();
//     while(t.elapsed()<1000)  
//       QCoreApplication::processEvents();

    // 2.发送请求
    SubmitVote(str, pid);

    QElapsedTimer t;
    t.restart();
    while(t.elapsed()<nseconds*1000)  
      QCoreApplication::processEvents();
  }
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
   header_list.push_back(HttpParamItem("Host", "toutiao.com"));
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

void autobots_toutiao::GetContent()
{
  QString str_url_1 = "http://finance.sina.cn/finance_zt/2015sdjjrwpx?from=groupmessage&isappinstalled=0";

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
  header_list1.push_back(HttpParamItem("Host", "finance.sina.cn"));
  //header_list1.push_back(HttpParamItem("Referer", "http://toutiao.com/"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* rp = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();
  while (rp && !rp->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      break;
    }
  }
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
//   m_url = ui.lineEdit_url->text();
//   m_group_id = ui.lineEdit_page_id->text();
//   m_vote_id = ui.lineEdit_vote_id->text();
//   m_vote_order = ui.lineEdit_vote_order->text();
//   m_account_list.clear();
// 
//   int n = ui.tableWidget_account_id->rowCount();
// 
//   for (int i = 0 ; i < n; ++i)
//   {
//     QString name = ui.tableWidget_account_id->item(i, 0)->text();
//     QString password = ui.tableWidget_account_id->item(i, 1)->text();
//     m_account_list.push_back(AccountParam(name,password));
//   }
// 
//   network.setVote_id(m_vote_id);
//   network.setVote_order(m_vote_order);

}

void autobots_toutiao::initialize()
{
//   ui.tableWidget_account_id->setColumnWidth(0, 200);
//   ui.tableWidget_account_id->setColumnWidth(1, 150);
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
   header_list.push_back(HttpParamItem("Host", "toutiao.com"));
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


QString autobots_toutiao::GetValidateCode(QString& spid)
{
  qint64 ctime = QDateTime::currentMSecsSinceEpoch();
  spid = QString::number(ctime);
  QString str_url = "http://antispam.mix.sina.cn/captcha/show?key=oZ18xDw6MrXFBzl4&pid=" + spid;
  

  QUrl url_1(str_url);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"image/png, image/svg+xml, image/*;q=0.8, */*;q=0.5"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3"));

  header_list1.push_back(HttpParamItem("Host", "antispam.mix.sina.cn"));
  header_list1.push_back(HttpParamItem("Referer", "http://finance.sina.cn/finance_zt/2015sdjjrwpx?from=groupmessage&isappinstalled=0"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list1.push_back(HttpParamItem("Cookie", GetCookieString())); 
  QNetworkReply* reply = network.GetRequest(url_1, header_list1);
  
  QTime _t;
  _t.start();
  while (reply && !reply->isFinished())
  {
      QCoreApplication::processEvents();
      if (_t.elapsed() >= TIMEOUT1) {
        break;
      }
  }

  QVariant var = reply->header(QNetworkRequest::ContentTypeHeader);

  QString type = var.toString();

  QByteArray data = reply->readAll();
  data.data();

  QImage image_ = QImage::fromData(data);//(data.data(),100,40,QImage::Format_RGB32);
  
  //image_.save("e:\\1.jpg");
  QString vcode;
  if (m_code_by_manual)
  {
    ValidateDlg dlg(this, image_);
    if (dlg.exec() == QDialog::Accepted)
    {
      vcode = dlg.getCode();
    }
  }
  else
  {
    bool result = image_.save("e:\\1.jpg");

    if (result)
    {
      VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
      QString code_sign;
      int res = obj->GetRecResults(data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
      obj->ReportError("bestsalt", code_sign);
      return vcode;
    }
  }
  

  return vcode;
}

void autobots_toutiao::SubmitVote(const QString& ccode, const QString& spid)
{
  //http://survey.sina.cn/aj/submit?poll_id=112091&q_16535=88851&c_key=oZ18xDw6MrXFBzl4&c_pid=1450927612944&c_code=elbk
//   qint64 ctime = QDateTime::currentMSecsSinceEpoch();
//   QString spid = QString::number(ctime);
  QString str_url = "http://survey.sina.cn/aj/submit?poll_id=112091&q_16535=88851&c_key=oZ18xDw6MrXFBzl4&c_pid="+ spid;
  str_url += "&c_code=" + ccode;

  QUrl url_1(str_url);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"*/*"));
  header_list1.push_back(HttpParamItem("Connection","keep-alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3"));

  header_list1.push_back(HttpParamItem("Host", "survey.sina.cn"));
  header_list1.push_back(HttpParamItem("Origin", "http://finance.sina.cn"));
  header_list1.push_back(HttpParamItem("Referer", "http://finance.sina.cn/finance_zt/2015sdjjrwpx?from=groupmessage&isappinstalled=0"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list1.push_back(HttpParamItem("Cookie", GetCookieString())); 

  QNetworkReply* reply = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();
  while (reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT1) {
      break;
    }
  }
  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  QVariant var = reply->header(QNetworkRequest::ContentTypeHeader);

  QString type = var.toString();

  QByteArray data = reply->readAll();

  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    { 
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("status"))  
      {  
        QJsonValue name_value = obj.take("status");
        if(name_value.isBool())
        {
          bool bstatus = name_value.toBool();
          if (bstatus)
          {
            ui.lineEdit_msg->setText(QStringLiteral("投票成功.."));
          }
        }
      }
      if(obj.contains("msg"))  
      {  
        QJsonValue name_value = obj.take("msg");
        if(name_value.isString())
        {
          QString msg = name_value.toString();
        }
      }
    }
  }

}


QString autobots_toutiao::GetCookieString()
{
  QList<QNetworkCookie> cookies = network.GetManager().cookieJar()->cookiesForUrl(
    QUrl("http://finance.sina.cn/finance_zt/2015sdjjrwpx?from=groupmessage&isappinstalled=0"));

  QString res ;
  for (int i = 0; i < cookies.size(); ++i)
  {
    QString tempe = cookies[i].toRawForm();
    if (i != 0)
    {
      res += ";" + tempe;
    }
    else
    {
      res = tempe;
    }
  }

  return res;
}
