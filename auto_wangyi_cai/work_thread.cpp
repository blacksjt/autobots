
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

    count++;

    QString temp2;
    temp2.setNum(count);

    // 发送消息
    QString msg = temp + temp2;
    emitMsg(msg);

    work_run();

    sleep(8);

  }
}

QString WorkThread::GetToken()
{
    QNetworkRequest req;

    QString str_url1 = m_token_url;

    req.setUrl(QUrl(str_url1));

    QString origin = "http://" + m_host;

    req.setRawHeader("Cache-Control", "no-cache");
    req.setRawHeader("Accept", "*/*");
    req.setRawHeader("Connection", "Keep-Alive");
    //req.setRawHeader("Accept-Encoding", "gzip, deflate");
    req.setRawHeader("Accept-Language", "zh-CN");
    req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
    req.setRawHeader("Content-Length", "0");
    req.setRawHeader("X-Requested-With", "XMLHttpRequest");
    req.setRawHeader("Host", m_host.toUtf8().data());
    req.setRawHeader("Origin", origin.toUtf8().data());
    req.setRawHeader("Referer", m_referer.toUtf8().data());
    req.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

    /*req.setAttribute()*/
    QByteArray request_params;
    //request_params.append("undefined");
    QNetworkReply* reply = m_manager.post(req, request_params);

	QTime t;
	t.start();
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (t.elapsed() >= 10 * 1000) {
			break;
		}
	}

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
        msg = reply->errorString();
    }

    QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

    int n = statusCodeV.toInt();

    QByteArray data = reply->readAll();

    QString ret = ParseToken(data);

    reply->deleteLater();

    return ret;
}

QString WorkThread::ParseToken(const QByteArray & data)
{
    QString ret;
    QJsonParseError json_error;
    QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
    if (json_error.error == QJsonParseError::NoError)
    {
        if (parse_doucment.isObject())
        {
            QJsonObject obj = parse_doucment.object();
            if (obj.contains("gentoken"))
            {
                QJsonValue name_value = obj.take("gentoken");
                if (name_value.isString())
                {
                    ret = name_value.toString();
                }
            }
        }
    }

    return ret;
}

void WorkThread::work_run()
{
  QNetworkCookieJar* cookie = new QNetworkCookieJar();
  m_manager.setCookieJar(cookie);

  //QString token = GetToken();

  //QString path = GetMatchedText(m_token_url);

  foreach(QString str, m_comment_list)
  {
	QString token = GetToken();
    if (token.isEmpty())
    {
        token = GetToken();
        if (token.isEmpty())
        {
            continue;
        }
    }
	QString path = GetMatchedText(m_token_url);

    QNetworkRequest req;

    QString str_url1 = path + str + "/action/against?ntoken=" + token + "&ibc=newspc";

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

	QTime t;
	t.start();
    while(!reply->isFinished())
    {
      QCoreApplication::processEvents();
	  if (t.elapsed() >= 10*1000) {
		  break;
	  }
    }
// 
     QString msg;
     if (reply->error() != QNetworkReply::NoError)
     {
       msg = reply->errorString();
	   //emitMsg(msg);
	   reply->deleteLater();
	   continue;
     }
 
     QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
 
     int n = statusCodeV.toInt();

     if (n == 200 )
     {
         emitMsg(QStringLiteral("点赞成功"));
     }
     else if (n == 429)
     {
         emitMsg(QStringLiteral("频率太高"));
     }
 
     msg = reply->readAll();

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

QString WorkThread::GetMatchedText(const QString& text)
{
    //QString s = "http://comment.dy.163.com/api/v1/products/a2869674571f77b5a0867c3d71db5856/threads/C1G1285K0511A5KT/comments/gentoken?ibc=newspc";
//     QString pattern = "(.*)gentoken?ibc=newspc";
//     QRegExp regexp(pattern, Qt::CaseInsensitive);
// 
//     int pos = text.indexOf(regexp);
// 
//     QString s1 = regexp.cap(0);
//     QString s2 = regexp.cap(1);

    int pos = text.indexOf("gentoken?ibc=newspc");

    QString s2 = text.left(pos);

    return s2;
}