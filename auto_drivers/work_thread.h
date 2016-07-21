#ifndef WORK_THREAD_HHHH
#define WORK_THREAD_HHHH

#include <QtNetwork>
#include <QThread>
#include "../http_base/http_base.h"

class network : public HttpBase
{
  Q_OBJECT
public:
  network(QObject* parent = 0);
  ~network();

protected:
  virtual void ProcessReplyData(QNetworkReply* reply);

  protected slots:
    virtual void OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
    virtual void OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
};


class WorkThread : public QThread
{
  Q_OBJECT
public:
  WorkThread(QObject *parent = 0);
  ~WorkThread();

  inline void SetParameters(const QString& url, const QString& ref, 
    const QString& id, const QStringList& commentlist)
  {
    m_url = url;
    m_referer =ref;
    m_news_id = id;
    m_comment_list = commentlist;
  }

signals:
  void emitMsg(const QString&);
  

protected:
  void run();

private:
  void WorkThread::work_run();

private:
  //network mnet;
  QNetworkAccessManager* m_manager;
  QString m_url;  // URL
  QString m_referer; // referer
  QString m_news_id;  // ÐÂÎÅID
  QStringList m_comment_list;  // ÆÀÂÛID
};


#endif //WORK_THREAD_HHHH