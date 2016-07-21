#include "my_work.h"

MyWork::MyWork()
{
  m_b_run = true;
}

MyWork::MyWork(const QStringList& commentlist)
{
  m_url_list = commentlist;
}

MyWork::~MyWork()
{

}

void MyWork::onWorkStart()
{
  QString msg = QStringLiteral("运行中");
  int count = 0;
  qDebug()<< QThread::currentThreadId();
  while( m_b_run )
  {
    QString temp = msg;

    work_run();

    count ++; 

    QString temp2;
    temp2.setNum(count);

    // 发送消息
    QString msg = temp +temp2;
    //emitMsg(msg);
    //ui.lineEdit_msg->setText(temp + temp2);

    //sleep(2);
    waitForSeconds(2);

  }
}

void MyWork::work_run()
{
  //QNetworkCookieJar* cookie = new QNetworkCookieJar();
  //m_manager.setCookieJar(cookie);
  QNetworkAccessManager mangager;
  foreach(QString str, m_url_list)
  {
    //QString str_url1 = m_url;
    QNetworkRequest req;
    QString str_host = GetHost(str);
    req.setUrl(QUrl(str));

    //QString post_data = QString("act=support&rid=%1&tid=%2").arg(str,m_news_id);
    //int len = post_data.length();
    //QByteArray request_params = post_data.toUtf8();

    //HttpParamList header_list;
    //req.setHeader(QNetworkRequest::ContentTypeHeader,"application/x-www-form-urlencoded; charset=UTF-8");
    //req.setHeader(QNetworkRequest::ContentLengthHeader, QString::number(len).toUtf8());
    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
    req.setRawHeader("Cache-Control","no-cache");
    req.setRawHeader("Connection","Keep-Alive");
    //req.setRawHeader("Accept-Encoding","gzip, deflate");
    req.setRawHeader("Accept-Language","zh-CN,zh;q=0.8");
    req.setRawHeader("Connection", "keep-alive");
    req.setRawHeader("Host", str_host.toUtf8());
    //req.setRawHeader("Origin", "http://comment8.mydrivers.com");
    req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

    //HttpParamList post_data;


    //     post_data.push_back(HttpParamItem("act","support"));
    //     post_data.push_back(HttpParamItem("rid",str));
    //     post_data.push_back(HttpParamItem("tid", m_news_id));

    QNetworkReply* reply = mangager.get(req);

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

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
      msg = reply->errorString();
    }

    QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

    int n = statusCodeV.toInt();

    msg = reply->readAll();
    #endif
    waitForSeconds(1);
    reply->deleteLater();

    

  }

  //cookie->deleteLater();
}

void MyWork::waitForSeconds(int n)
{
  QTime time;
  time.start();

  while(time.elapsed() < n*1000)
  {
    QCoreApplication::processEvents();
  }
}

void MyWork::onWorkStop()
{
  m_b_run  = false;
}

QString MyWork::GetHost(const QString& str)
{
  int pos1 = str.indexOf("://");

  if (pos1 == -1)
  {
    return "";
  }

  QString sub_str = str.right(str.length() - pos1 - 3);

  pos1 = sub_str.indexOf("/");

  if (pos1 == -1)
  {
    return "";
  }

  QString res_str = sub_str.left(pos1);

  return res_str;
}
