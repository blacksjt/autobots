#include "autobots_weibo_vote.h"
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
#include <windows.h>

const int TIMEOUT = 20*1000;


autobots_weibo_vote::autobots_weibo_vote(QWidget *parent)
    : QMainWindow(parent),
    m_account_order(0)
{
    ui.setupUi(this);

    initialize();

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));
}

autobots_weibo_vote::~autobots_weibo_vote()
{

}

void autobots_weibo_vote::onStart()
{
  // 检查输入
  if (!CheckInput())
  {
    return;
  }

  ui.pushButton_start->setEnabled(false);

  // 更新界面输入
  UpdateData();

  int ncount = 0;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    
    QString msg = QStringLiteral("第%1次").arg(i+1);
    //msg.setNum(i+1);
    ui.lineEdit_msg->setText(msg);

     ncount++;
     if (m_auto_switch_ip && ncount> 3)
     {
       ncount = 0;
       switchIP();
     }

    QNetworkCookieJar* cookie = new QNetworkCookieJar();

    network.GetManager().setCookieJar(cookie);

    bool login_status = false;
    //GetContent();
    if (m_auto_switch_ip )
    {
      while(true)
      {
        if (GetContent())
        {
          break;
        }
        switchIP();
        QElapsedTimer t2;
        t2.start();
        while(t2.elapsed() < 4000 )  
          QCoreApplication::processEvents();
      }
    }
    else
    {
      GetContent();
    }

    if (!RequestForLogin())
    {
      ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
      continue;
    }

    AccountParam ac = m_account_list.at(i);

    if (!AuthorBySina(ac._id, ac._password))
    {
      ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
      ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(255,0,0, 180));
      //m_account_order++;
      continue;
    }
    else
    {
      ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
      ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(0,255,0, 180));
      //m_account_order++;
      login_status = true;
    }

    bool res = DoWork();

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed() < 1000 )  
      QCoreApplication::processEvents();

    Logout();

  }

  ui.pushButton_start->setEnabled(true);
}

void autobots_weibo_vote::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

bool autobots_weibo_vote::DoWork()
{
  //http://vote.weibo.com/aj/joinpoll?ajwvr=6&__rnd=1452062896586
  QString cur_time = GetTimeStr();
  QString str_url1 = "http://vote.weibo.com/aj/joinpoll";//?ajwvr=6&__rnd=" + cur_time;

  QUrl url1;
  url1.setUrl(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
  header_list.push_back(HttpParamItem("Host", m_host));
  header_list.push_back(HttpParamItem("Referer", m_url));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  //post_data.push_back(HttpParamItem("_t", "0"));
  post_data.push_back(HttpParamItem("anonymous", "0"));
  post_data.push_back(HttpParamItem("items", m_vote_order));
  post_data.push_back(HttpParamItem("poll_id", m_vote_id));
  post_data.push_back(HttpParamItem("share", "1"));

  QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

  QTime _t;
  _t.start();

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) )
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  QByteArray data = reply->readAll();

  return GetVoteResult(data);

  //return true;
}

bool autobots_weibo_vote::GetContent()
{
  QString str_url_1 = m_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"text/html, application/xhtml+xml, */*"));
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

bool autobots_weibo_vote::RequestForLogin()
{
  qint64 ntime = QDateTime::currentMSecsSinceEpoch();
  ntime = ntime*10 + 1;
  QString time = QString::number(ntime);
  QString str_url1 = "http://login.sina.com.cn/sso/qrcode/image?entry=weibo&size=180&callback=STK_" + time;
  //                  http://login.sina.com.cn/sso/qrcode/image?entry=weibo&size=180&callback=STK_14520627690575
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


bool autobots_weibo_vote::AuthorBySina(const QString& name, const QString& password)
{
  //1.检验验证码
  QString vcode, code_sign;
  SinaData sina_data;
  if (!PreLoginSina(name, sina_data, vcode,code_sign))
  {
    ui.lineEdit_msg->setText(QStringLiteral("获取验证码失败"));
    return false;
  }

//   if (sina_data._showpin && vcode.isEmpty())
//   {
//     return false;
//   }

  // 2.登陆
  int res = LoginSina(sina_data,name,password,vcode,code_sign);
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


void autobots_weibo_vote::UpdateData()
{
  m_url = ui.lineEdit_url->text();
  m_vote_id = ui.lineEdit_vote_id->text();
  m_vote_order = ui.lineEdit_vote_order->text();
  m_host = ui.lineEdit_host->text();
  //m_code_online = true;
  //m_comment_list.clear();
  m_account_list.clear();
  //m_comment_item_list.clear();
  m_account_row_list.clear();
  //m_comment_order = 0;
  m_account_order = 0;

  m_code_online = (ui.checkBox_code_online->checkState() == Qt::Checked) ;
  m_auto_switch_ip = (ui.checkBox_switch_ip->checkState() == Qt::Checked);

  int n = ui.tableWidget_account_id->rowCount();

  for (int i = 0 ; i < n; ++i)
  {
    QString name = ui.tableWidget_account_id->item(i, 0)->text();
    QString password = ui.tableWidget_account_id->item(i, 1)->text();
    m_account_list.push_back(AccountParam(name,password));
    m_account_row_list.push_back(i);
  }

}

void autobots_weibo_vote::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
  //ui.pushButton_develiver->setEnabled(false);
  m_client_id = "2504490989";
  m_appkey= "42iQjj";
  m_state = "f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D";
}

void autobots_weibo_vote::onActFromTxt()
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

void autobots_weibo_vote::Logout()
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

void autobots_weibo_vote::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  m_account_order = 0;
  m_account_list.clear();
  m_account_row_list.clear();
}

bool autobots_weibo_vote::CheckInput()
{
  if (ui.lineEdit_url->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_vote_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("投票选项没有输入")); 
    return false;
  }

  if (ui.tableWidget_account_id->rowCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请导入账号")); 
    return false;
  }

  return true;
}


bool autobots_weibo_vote::GetCsrfToken(const QByteArray& rp_data)
{
//   HtmlParseUtils html_parse(rp_data.data(), rp_data.length());
// 
//   // "查找csrftoken"
//   GumboNode*  node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");
// 
//   if (node != NULL)
//   {
//     std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
//     m_csrf_token = QString::fromStdString(csrf_token);
//     return true;
//   }
// 
   return false;
}

bool autobots_weibo_vote::ProcessRedirectSSL(const QString& str)
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
}

bool autobots_weibo_vote::ProcessRedirectGet(const QString& str)
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

bool autobots_weibo_vote::GetPostId(const QByteArray& arr)
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

bool autobots_weibo_vote::ProcessRedirectLoginGet(const QString& str)
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

    QString str = red_url.toString();

    return ProcessRedirectLoginGet2(str);
  }
  else
  {
    return false;
  }
}

bool autobots_weibo_vote::ProcessRedirectLoginGet2(const QString& str)
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


bool autobots_weibo_vote::GetFatieStatus(const QByteArray& byte_arr)
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

bool autobots_weibo_vote::PreLoginSina(const QString& name, SinaData& data, QString& vcode,QString& code_sign)
{
  //https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=Yzc3ODgyNDczeWFueWUlNDAxNjMuY29t&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=1451282884248
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);
                            //http://login.sina.com.cn/sso/prelogin.php?entry=weibo&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.18)&_=1452062874752
  QString str_url1 = QString("http://login.sina.com.cn/sso/prelogin.php?entry=weibo&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);
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

  //if (!data._showpin) // 不需要验证码
  //{
  //  return true;
  //}
  
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
  QImage image_ = QImage::fromData(rp_data);
  if (m_code_online)
  {
    // 在线验证
      VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
	  int res = obj->GetRecResults(rp_data, "bestsalt", "hh610520;;", "bestsalt", vcode, code_sign);
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

//   QImage image_ = QImage::fromData(rp_data);//(data.data(),100,40,QImage::Format_RGB32);
// 
//   //image_.save("e:\\1.jpg");
//   ValidateDlg dlg(this, image_);
//   if (dlg.exec() == QDialog::Accepted)
//   {
//     vcode = dlg.getCode();
//   }

  return true;
}

bool autobots_weibo_vote::GetPreLoginResult(const QByteArray& str, SinaData& data)
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

// 返回 0-成功 -1：验证码错误 -2：其他错误
int autobots_weibo_vote::LoginSina(SinaData& sina_data,const QString& name, 
  const QString& password,const QString& vcode,const QString& code_sign)
{
  qint64 time = QDateTime::currentMSecsSinceEpoch();
  QString str_time = QString::number(time);

  // 获取用户名
  sina_encrypt* encryptor = sina_encrypt::GetInstance();
  QString encrypted_name = encryptor->EncryptName(name);
  QString encrypted_pwd = encryptor->EncryptPassword(password, sina_data._servertime, sina_data._nonce, sina_data._pubkey);

  QString str_login_url = 
    QString("http://login.sina.com.cn/sso/login.php?entry=weibo&gateway=1&from=&savestate=7&useticket=1&pagerefer=%1&pcid=%2&door=%3&vsnf=1&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=141&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.18)&_=%9").arg(
    m_url,sina_data._pcid,vcode,encrypted_name, sina_data._servertime, sina_data._nonce, sina_data._rsakv,encrypted_pwd, GetTimeStr()); 
  //http://login.sina.com.cn/sso/login.php?entry=weibo&gateway=1&from=&savestate=7
  //&useticket=1&pagerefer=http%3A%2F%2Fvote.weibo.com%2Fpoll%2F137073782&pcid=gz-28d94a22b8d53cb17497ad0c2578d2d57078
  //&door=bghqu&vsnf=1&su=dG9uZ3R1eWFxNTc3MSU0MDE2My5jb20%3D&service=miniblog&servertime=1452062908&nonce=9VFEYL
  //&pwencode=rsa2&rsakv=1330428213&sp=&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=141&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.18)&_=1452062881739
  
  QUrl url1(str_login_url);

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

//   if (!res)
//   {
//     VlidateCodeOnLine* Obj = VlidateCodeOnLine::GetInstance();
//     Obj->ReportError("bestsalt", code_sign);
//   }

  reply->deleteLater();

  return res;
}

int autobots_weibo_vote::GetLoginResult(const QByteArray& str, SinaData& data)
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

bool autobots_weibo_vote::AuthorizeSina(const SinaData& sina_data)
{
  QString str_url_author = 
    QString("https://passport.weibo.com/wbsso/login?callback=sinaSSOController.callbackLoginStatus&ticket=%1&ssosavestate=1453475228&client=ssologin.js(v1.4.18)&_=%2").arg(
    sina_data._ticket, GetTimeStr());

  QUrl url1(str_url_author);
  
  QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept",	"text/html, application/xhtml+xml, */*"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "passport.weibo.com"));
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

  QByteArray data = reply->readAll();

  return GetAuthorResult(data);
}

bool autobots_weibo_vote::GetQrid(const QByteArray& rp_data, QString& qrid)
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

  return false;
}

QString autobots_weibo_vote::GetTimeStr()
{
  qint64 n = QDateTime::currentMSecsSinceEpoch();

  return QString::number(n);
}

bool autobots_weibo_vote::GetVoteResult(const QByteArray& data)
{
  bool res = false;
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    {  
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("code"))  
      {  
        QJsonValue name_value = obj.take("code");
        if (name_value.isString())
        {
          QString str_code = name_value.toString();
          if (str_code.toInt() == 10000)
          {
            res = true;
          }
        }
      }
      if(obj.contains("msg"))  
      {  
        QJsonValue _data = obj.take("msg");
        if (_data.isString())
        {
          QString msg = _data.toString();
          ui.lineEdit_msg->setText(msg);
        }
      }
    }
  }

  return res;
}

bool autobots_weibo_vote::GetAuthorResult(const QByteArray& data)
{
  return true;
}


void autobots_weibo_vote::switchIP()
{
  //执行完即返回0  
  //int res = system("Rasdial ADSL /DISCONNECT"); 
  QString cmd1 = "cmd /c Rasdial ADSL /DISCONNECT";
  int res = WinExec(cmd1.toUtf8().data(), SW_HIDE);
  //qDebug() << "Disconnect -" << (res == 0 ? "succeed" : "failed"  );
  QString msg = (res > 31 ? QStringLiteral("成功断开") : QStringLiteral("断开失败"));
  ui.lineEdit_msg->setText(msg);

  QElapsedTimer t;
  t.start();
  while(t.elapsed()<2000)  
    QCoreApplication::processEvents();

  QString cmd2 = "cmd /c Rasdial ADSL 18120077177 852086"; 

  //成功连接返回0 其他为<span style="color:#ff0000">错误码</span>  
  //res = system(cmd.toLatin1().data());

  res = WinExec(cmd2.toUtf8().data(), SW_HIDE);

  msg = (res > 31 ? QStringLiteral("更换成功") : QStringLiteral("更换失败"));
  ui.lineEdit_msg->setText(msg);
  QElapsedTimer t2;
  t2.start();
  while(t2.elapsed()<2000)  
    QCoreApplication::processEvents();
}


