#include "my_work.h"

MyWork::MyWork()
{
  m_b_run = true;
}

MyWork::MyWork(const QString& url, const QString& ref, 
  const QString& id, const QStringList& commentlist)
{
  m_url = url;
  m_referer =ref;
  m_news_id = id;
  m_comment_list = commentlist;
  m_b_run = true;
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
    QString msg_temp = temp +temp2;
    //emitMsg(msg);
    //ui.lineEdit_msg->setText(temp + temp2);

    //sleep(2);
    waitForSeconds(3);

  }
}

void MyWork::work_run()
{
  QNetworkCookieJar* cookie = new QNetworkCookieJar();
  m_manager.setCookieJar(cookie);

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
    req.setRawHeader("X-Requested-With", "XMLHttpRequest");
    req.setRawHeader("Accept-Language","zh-CN");
    req.setRawHeader("Host", "comment8.mydrivers.com");
    req.setRawHeader("Origin", "http://comment8.mydrivers.com");
    req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

    //HttpParamList post_data;


    //     post_data.push_back(HttpParamItem("act","support"));
    //     post_data.push_back(HttpParamItem("rid",str));
    //     post_data.push_back(HttpParamItem("tid", m_news_id));

    QNetworkReply* reply = m_manager.post(req, request_params);

    //#ifdef _DEBUG
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
    //#endif

    reply->deleteLater();

    //waitForSeconds(1);

  }

  cookie->deleteLater();
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
