#include "auto_cnbeta.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "QDateTime"

auto_cnbeta::auto_cnbeta(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
  m_csrf_token = "89fac5a185d6bb823ad892724d40dda8f22edd4f";

    ui.setupUi(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));
}

auto_cnbeta::~auto_cnbeta()
{

}

void auto_cnbeta::onStart()
{
  if (!CheckInput())
  {
    return;
  }

  UpdateData();

  control_status = true;
  ui.lineEdit_msg->setText(QStringLiteral("运行中"));
  QString msg = QStringLiteral("运行中");

  int count = 0;

  while( control_status )
  {
    QString temp = msg;

    smzdm_run();

    count ++; 

    QString temp2;
    temp2.setNum(count);

    ui.lineEdit_msg->setText(temp + temp2);

    QElapsedTimer t;
    t.start();
    while(t.elapsed()<1000)  
      QCoreApplication::processEvents();

  }
}

void auto_cnbeta::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

int auto_cnbeta::smzdm_run()
{
  QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

  network.GetManager().setCookieJar(cookie);

  bool res = GetContent();
  res = GetToken();

  if (!res)
  {
    if (!GetToken())
    {
      ui.lineEdit_msg->setText(QStringLiteral("打开不了网页或速度过慢"));
      return -1;
    }
  }


  foreach(QString str, m_comment_list)
  {
    QString str_url1 = "http://www.cnbeta.com/comment";

    QUrl url1(str_url1);

    HttpParamList header_list;
    header_list.push_back(HttpParamItem("Accept", "application/json, text/javascript, */*; q=0.01"));
    header_list.push_back(HttpParamItem("Content-Type","application/x-www-form-urlencoded; charset=UTF-8"));
    header_list.push_back(HttpParamItem("Referer", m_news_url));
    header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
    //header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
    header_list.push_back(HttpParamItem("Host", m_host));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

    HttpParamList post_data;
    post_data.push_back(HttpParamItem("csrf_token",m_token));
    post_data.push_back(HttpParamItem("op","support"));
    post_data.push_back(HttpParamItem("sid",m_news_id));
    post_data.push_back(HttpParamItem("tid", str));

    QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

#ifdef _DEBUG
      QTime _t;
      _t.start();
  
      bool _timeout = false;
 
      while (reply && !reply->isFinished())
      {
        QCoreApplication::processEvents();
        if (_t.elapsed() >= 10*1000) {
          _timeout = true;
          break;
        }
      }

      QByteArray rp = reply->readAll();
      QString ss =rp;
#endif
      reply->deleteLater();
// 
   }

  cookie->deleteLater();

  return 0;
}

void auto_cnbeta::onOneClick()
{
  UpdateData();
  smzdm_run();
}

void smzdm_network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{

}

void smzdm_network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{

}

void smzdm_network::ProcessReplyData(QNetworkReply* reply)
{
  //TODO
//   if (reply->error() != QNetworkReply::NoError)
//   {
//     QString msg = reply->errorString();
//   }
// 
//   QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
// 
//   int n = statusCodeV.toInt();
// 
//   QString text = reply->readAll();
// 
//   HttpBase::ProcessReplyData(reply);
}

smzdm_network::smzdm_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

smzdm_network::~smzdm_network()
{

}

void auto_cnbeta::onActClearComments()
{
  ui.listWidget_comment_id->clear();
}

void auto_cnbeta::onActImportComment()
{
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL,  QStringLiteral("提示"), QStringLiteral("无法打开文件"));  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.listWidget_comment_id->clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    ui.listWidget_comment_id->addItem(new QListWidgetItem(line_str));
  }

  file.close();
}

void auto_cnbeta::UpdateData()
{
   m_news_url = ui.lineEdit_news_url->text();
   m_news_id = ui.lineEdit_page_id->text();
   m_host = ui.lineEdit_HOST1->text();
   m_comment_list.clear();
 
  int n = ui.listWidget_comment_id->count();

  for (int i = 0 ; i < n; ++i)
  {
    QString text = ui.listWidget_comment_id->item(i)->text();
    if (!text.isEmpty())
    {
      m_comment_list.push_back(text);
    } 
  }
}

bool auto_cnbeta::CheckInput()
{
  if (ui.lineEdit_news_url->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻网址URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_page_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
    return false;
  }

  if (ui.lineEdit_HOST1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
    return false;
  }

  //if (ui.lineEdit_REFERER->text().isEmpty())
  //{
  //  QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Referer没有输入")); 
  //  return false;
  //}

  //if (ui.lineEdit_news_id->text().isEmpty())
  //{
  //  QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
  //  return false;
  //}

  if (ui.listWidget_comment_id->count() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
    return false;
  }

  return true;
}

void auto_cnbeta::onAddCommentID()
{

  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  ui.listWidget_comment_id->addItem(id);

}

bool auto_cnbeta::GetContent()
{
  QString str_url_1 = m_news_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  //header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
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
    if (_t.elapsed() >= 20*1000) {
      _timeout = true;
      break;
    }
  }

  bool res = GetCsrfToken(rp->readAll());

  if (rp!=NULL)
    rp->deleteLater();

  return  true;
}

bool auto_cnbeta::GetToken()
{
  QString str_url_1 = "http://www.cnbeta.com/cmt";

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
  header_list1.push_back(HttpParamItem("Referer", m_news_url));
  header_list1.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
  header_list1.push_back(HttpParamItem("Host", m_host)); // mhost
  header_list1.push_back(HttpParamItem("Origin", "http://www.cnbeta.com"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.103 Safari/537.36"));
 
  HttpParamList post_data;
  post_data.push_back(HttpParamItem("csrf_token", m_csrf_token));
  post_data.push_back(HttpParamItem("op","1,514733,0daad"));

  QNetworkReply* rp = network.PostRequest(url_1, header_list1,post_data);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while (rp && !rp->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= 20*1000) {
      _timeout = true;
      break;
    }
  }
  QVariant statusCodeV =  rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  QByteArray rp_data = rp->readAll();
  bool res = GetCsrfToken2(rp_data);

  if (rp!=NULL)
    rp->deleteLater();

  return  res;
}

bool auto_cnbeta::GetCsrfToken(const QByteArray& data)
{
  QString rp_data = data;

  if (rp_data.indexOf("TOKEN:") != -1)
  {
    int pos1 = rp_data.indexOf("TOKEN:\"");
    //QString sub1 = rp_data.right()
    int pos2 = rp_data.indexOf("\"", pos1+7);

    m_csrf_token = rp_data.mid(pos1+7, pos2-pos1-7);
    if (m_csrf_token.isEmpty())
    {
      return false;
    }
    return true;
  }

  return false;

}

bool auto_cnbeta::GetCsrfToken2(const QByteArray& data)
{
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
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

          QJsonObject result_obj = name_value.toObject();

          if (result_obj.contains("token"))
          {
            QJsonValue token = result_obj.take("token");
            if (token.isString())
            {
              m_token = token.toString();
            }
            if (!m_token.isEmpty())
            {
              return true;
            }
          }
        }
      }
    }
  }
  return false;
}