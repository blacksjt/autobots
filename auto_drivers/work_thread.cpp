
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
  : QThread(parent),m_manager(NULL)
{
  
}

WorkThread::~WorkThread()
{
  if (m_manager != NULL)
  {
    m_manager->deleteLater();
    m_manager = NULL;
  }
}

void WorkThread::run()
 {
   qDebug() << "thread_id" << QThread::currentThreadId();
 
   QString msg = QStringLiteral("线程")+QString::number((int)(QThread::currentThreadId())) + QStringLiteral("_运行中:") ;
   int count = 0;
 
   m_manager = new QNetworkAccessManager();
   while( /*control_status*/ true)
   {
     QString temp = msg;
 
     work_run();
 
     count ++; 
 
     QString temp2;
     temp2.setNum(count);
 
     // 发送消息
     QString msg2 = temp +temp2;
     emitMsg(msg2);
     //ui.lineEdit_msg->setText(temp + temp2);
 
     sleep(2);
 
   }

   m_manager->deleteLater();
   m_manager = NULL;
 }

 void WorkThread::work_run()
 {
   QNetworkCookieJar* cookie = new QNetworkCookieJar();
   m_manager->setCookieJar(cookie);
 
   foreach(QString str, m_comment_list)
   {
     QString str_url1 = m_url;
     QNetworkRequest req;
 
     req.setUrl(QUrl(str_url1));
 
     QString post_data = QString("act=support&rid=%1&tid=%2").arg(str,m_news_id);
     int len = post_data.length();
     QByteArray request_params = post_data.toUtf8();

     //HttpParamList header_list;
     req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
     req.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(len).toUtf8());
     req.setRawHeader("Referer", m_referer.toUtf8());
     req.setRawHeader("Cache-Control","no-cache");
     req.setRawHeader("Connection","close");
     req.setRawHeader("Accept-Encoding","gzip, deflate");
     req.setRawHeader("Accept-Language","zh-CN");
     req.setRawHeader("Host", "comment8.mydrivers.com");
     req.setRawHeader("Origin", "http://comment8.mydrivers.com");
     req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");
 
     QNetworkReply* reply = m_manager->post(req, request_params);
 
//#ifdef _DEBUG
     QTime _t;
     _t.start();

     bool _timeout = false;

	 if (reply == NULL)
	 {
		 continue;
	 }

     while (!reply->isFinished())
     {
       QCoreApplication::processEvents();
       if (_t.elapsed() >= 10*1000) {
         _timeout = true;
         break;
       }
     }

     QString msg;
     if (reply->error() != QNetworkReply::NoError)
     {
       msg = reply->errorString();
     }

     QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

     int n = statusCodeV.toInt();

     msg = reply->readAll();
//#endif
 
     reply->deleteLater();
 
   }
   cookie->deleteLater();
 }