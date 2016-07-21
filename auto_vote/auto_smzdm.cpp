#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "html_parse_utils.h"

const int TIMEOUT = 20*1000;

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
    ui.setupUi(this);

    //networkManager = new QNetworkAccessManager(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(&network, SIGNAL(csrfToken(QString)), this, SLOT(onCsrfToken(QString)));

    connect(&network, SIGNAL(votecount(double)), this, SLOT(onVoteSucceed(double)));
}

auto_smzdm::~auto_smzdm()
{

}

void auto_smzdm::onStart()
{

  control_status = true;
  ui.lineEdit_msg->setText(QStringLiteral("运行中"));
  QString msg = QStringLiteral("运行中");

  int count = 0;

  QString interval = ui.lineEdit_interval->text();
  int nseconds = interval.toInt();

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
    while(t.elapsed()<nseconds*1000)  
      QCoreApplication::processEvents();

  }
}

void auto_smzdm::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

int auto_smzdm::smzdm_run()
{
  QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

  network.GetManager().setCookieJar(cookie);

  //获取CSRF TOKEN

  QString str_url_1 = "http://innoawards.geekpark.net/";

  QUrl url_1(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "innoawards.geekpark.net"));
  header_list1.push_back(HttpParamItem("Referer", "http://innoawards.geekpark.net/"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* rp1 = network.GetRequest(url_1, header_list1);


//    QString str_url0 = "http://innoawards.geekpark.net/";
//    QUrl url0(str_url0);
//  
//    HttpParamList header_list;
//    header_list.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8"));
//    header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate, sdch"));
//    header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8,en-US;q=0.5,en;q=0.3"));
//    header_list.push_back(HttpParamItem("Cache-Control","max-age=0"));
//    header_list.push_back(HttpParamItem("Connection","keep-alive"));
//    header_list.push_back(HttpParamItem("Host", "innoawards.geekpark.net"));
//    header_list.push_back(HttpParamItem("Upgrade-Insecure-Requests", "1"));
//    header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
//    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; rv:42.0) Gecko/20100101 Firefox/42.0"));
//  
//    QNetworkReply* rp1 = network.GetRequest(url0,header_list);

   QTime _t;
   _t.start();
 
   bool _timeout = false;
 
   while (!rp1->isFinished())
   {
     QCoreApplication::processEvents();
     if (_t.elapsed() >= TIMEOUT) {
       _timeout = true;
       break;
     }
   }

  if (m_csrf_token.isEmpty())
  {
    return -1;
  }

  //投票
  QString str_url1 = "http://innoawards.geekpark.net/ranks/97e194f9-f003-400a-ad82-60528c1060a8/vote";

  QUrl url1(str_url1);

  HttpParamList header_list2;
  header_list2.push_back(HttpParamItem("Referer", "http://innoawards.geekpark.net/?from=timeline&isappinstalled=0"));
  header_list2.push_back(HttpParamItem("Cache-Control","no-cache"));
  header_list2.push_back(HttpParamItem("Connection","keep-alive"));
  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list2.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list2.push_back(HttpParamItem("Host", "innoawards.geekpark.net"));
  header_list2.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
  header_list2.push_back(HttpParamItem("X-CSRF-Token", m_csrf_token));
  header_list2.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; rv:42.0) Gecko/20100101 Firefox/42.0"));

  HttpParamList post_data;
  network.PostRequest(url1, header_list2, post_data);

  //cookie->deleteLater();

  return 0;
}

void auto_smzdm::onOneClick()
{
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
  if (reply->error() != QNetworkReply::NoError)
  {
    QString msg = reply->errorString();
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n == 200)
  {
    QByteArray rp_data = reply->readAll();
    QString str = rp_data;
    int length = str.size();
    if (rp_data.size() != 0)
    {
      QVariant content_type = reply->header(QNetworkRequest::ContentTypeHeader);

      QString type = content_type.toString();

      if (type.contains("html"))
      {
        HtmlParseUtils html_parse(rp_data.data(), rp_data.length());

        // "查找post_from_id"
        GumboNode* node = html_parse.FirstElementNode("name", "csrf-token");

        if (node != NULL)
        {
          std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "content");

          QString str = QString::fromStdString(post_from_id);

          if (!str.isEmpty())
          {
            emit csrfToken(str);
          }
        }

      }
      else if (type.contains("json"))
      {
        QJsonParseError json_error;
        QJsonDocument parse_doucment = QJsonDocument::fromJson(rp_data, &json_error); 
        if(json_error.error == QJsonParseError::NoError) 
        {  
          if(parse_doucment.isObject())  
          { 
            QJsonObject obj = parse_doucment.object(); 
            if(obj.contains("vote"))  
            {  
              QJsonValue name_value = obj.take("vote");
              if(name_value.isDouble())
              {
                double count = name_value.toDouble();
                emit votecount(count);
              }
            }
          }
        }
      } 
    }
  }

  HttpBase::ProcessReplyData(reply);
}

smzdm_network::smzdm_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

smzdm_network::~smzdm_network()
{

}


void auto_smzdm::onCsrfToken(QString str)
{
  m_csrf_token = str;
}

void auto_smzdm::onVoteSucceed(double count)
{
  QString comment_id_str = QString::number(count, 'f' , 0);
  ui.lineEdit_counts->setText( comment_id_str );
}
