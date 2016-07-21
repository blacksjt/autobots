
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

//void WorkThread::run()
// {
// 
//   QString msg = QStringLiteral("运行中");
//   int count = 0;
// 
//   while( /*control_status*/ true)
//   {
//     QString temp = msg;
// 
//     work_run();
// 
//     count ++; 
// 
//     QString temp2;
//     temp2.setNum(count);
// 
//     // 发送消息
//     QString msg = temp +temp2;
//     emitMsg(msg);
//     //ui.lineEdit_msg->setText(temp + temp2);
// 
//     sleep(2);
// 
//   }
// }

// void WorkThread::work_run()
// {
//   QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
//   m_manager.setCookieJar(cookie);
// 
//   foreach(QString str, m_comment_list)
//   {
//     QString str_url1 = m_url;
//     QNetworkRequest req;
// 
//     req.setUrl(QUrl(str_url1));
// 
//     //HttpParamList header_list;
//     req.setRawHeader("Referer", m_referer.toUtf8());
//     req.setRawHeader("Cache-Control","no-cache");
//     req.setRawHeader("Connection","Keep-Alive");
//     req.setRawHeader("Accept-Encoding","gzip, deflate");
//     req.setRawHeader("Accept-Language","zh-CN");
//     req.setRawHeader("Host", "comment8.mydrivers.com");
//     req.setRawHeader("Origin", "http://comment8.mydrivers.com");
//     req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");
// 
//     //HttpParamList post_data;
//     QString post_data = QString("act=support&rid=%1&tid=%2").arg(str,m_news_id);
//     QByteArray request_params = post_data.toUtf8();
// 
// //     post_data.push_back(HttpParamItem("act","support"));
// //     post_data.push_back(HttpParamItem("rid",str));
// //     post_data.push_back(HttpParamItem("tid", m_news_id));
// 
//     QNetworkReply* reply = m_manager.post(req, request_params);
// 
// //#ifdef _DEBUG
//     QTime _t;
//     _t.start();
// 
//     bool _timeout = false;
// 
//     while (reply && !reply->isFinished())
//     {
//       QCoreApplication::processEvents();
//       if (_t.elapsed() >= 10*1000) {
//         _timeout = true;
//         break;
//       }
//     }
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
// //#endif
// 
//     reply->deleteLater();
// 
//     msleep(100); 
// 
//   }
// 
//   cookie->deleteLater();
// }