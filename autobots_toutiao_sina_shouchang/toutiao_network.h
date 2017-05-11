#ifndef AUTOBOTS_TOUTIAO_NETWORK_H
#define AUTOBOTS_TOUTIAO_NETWORK_H

#include <QtNetwork>
#include "../http_base/http_base.h"

class toutiao_network : public HttpBase
{
  Q_OBJECT
public:
  toutiao_network(QObject* parent = 0);
  ~toutiao_network();

signals:
//   void postIdReady(QString post_id);
//   void csrftokenReady(QString token);
//   void loginsucceed(QString id);
//   void clientID(QString id);
//   void diggsuccess(double commentid, int digcount);

protected:
  virtual void ProcessReplyData(QNetworkReply* reply);

protected slots:
  virtual void OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
  virtual void OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors);
  
  
private:
//   void ProcessRedirect(const QUrl& url);
//   void ParseHtml(const QByteArray byte_arr);
//   void ParseJson(const QByteArray byte_arr);

private:

};


#endif // AUTOBOTS_TOUTIAO_NETWORK_H