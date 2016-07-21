
#include "work_thread.h"



void network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{

}

void network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{

}

void network::ProcessReplyData(QNetworkReply* reply)
{

}

network::network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

network::~network()
{

}


WorkThread::WorkThread(QObject *parent /*= 0*/)
  : QThread(parent)
{

}

WorkThread::~WorkThread()
{

}

void WorkThread::run()
{

  QString msg = QStringLiteral("运行中");
  int count = 0;

  while( /*control_status*/ true)
  {
    QString temp = msg;

    work_run();

    count ++; 

    QString temp2;
    temp2.setNum(count);

    // 发送消息
    QString msg = temp +temp2;
    emitMsg(msg);
    //ui.lineEdit_msg->setText(temp + temp2);

    sleep(8);

  }
}

void WorkThread::work_run()
{
  QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
  m_manager.setCookieJar(cookie);

  foreach(QString str, m_comment_list)
  {
    QNetworkRequest req;

    QString str_url1 = str;//"http://comment.tech.163.com/reply/upvote/tech_bbs/B98L34L200094P0U_" + str;

    //QUrl url1(str_url1);
    req.setUrl(QUrl(str_url1));

    QString origin = "http://" + m_host;

    req.setRawHeader("Cache-Control","no-cache");
    req.setRawHeader("Accept","*/*");
    req.setRawHeader("Connection","Keep-Alive");
    req.setRawHeader("Accept-Encoding","gzip, deflate");
    req.setRawHeader("Accept-Language","zh-CN");
    req.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    req.setRawHeader("X-Requested-With", "XMLHttpRequest");
    req.setRawHeader("Host", m_host.toUtf8().data());
    req.setRawHeader("Origin", origin.toUtf8().data());
    req.setRawHeader("Referer", m_referer.toUtf8().data());
    req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

    /*req.setAttribute()*/
    QByteArray request_params;
    //request_params.append("undefined");
    QNetworkReply* reply = m_manager.post(req,request_params);

    while(!reply->isFinished())
    {
      QCoreApplication::processEvents();
    }
// 
//     QString msg;
//     if (reply->error() != QNetworkReply::NoError)
//     {
//       msg = reply->errorString();
//     }
// 
//     QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
// 
//     int n = statusCodeV.toInt();
// 
//     msg = reply->readAll();

    reply->deleteLater();
   
    msleep(100); 
  }

  cookie->deleteLater();

//    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
//    mnet.GetManager().setCookieJar(cookie);
// 
//   //network mnet;
// 
//   foreach(QString str, m_comment_list)
//   {
//     QString str_url1 = str;//"http://comment.tech.163.com/reply/upvote/tech_bbs/B98L34L200094P0U_" + str;
// 
//     QUrl url1(str_url1);
// 
//     HttpParamList header_list;
//     QString origin = "http://" + m_host;
//     
//     header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
//     header_list.push_back(HttpParamItem("Accept","*/*"));
//     header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//     header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
//     header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
//     header_list.push_back(HttpParamItem("Content-Type","application/x-www-form-urlencoded"));
//     header_list.push_back(HttpParamItem("Host", m_host));
//     header_list.push_back(HttpParamItem("Origin", origin));
//     header_list.push_back(HttpParamItem("Referer", m_referer));
//     header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
//     
//     HttpParamList post_data;
//     QNetworkReply* reply = mnet.PostRequest(url1, header_list, post_data);
// 
//     msleep(100);
// 
//     reply->deleteLater();
//   }
// 
//   cookie->deleteLater();
}