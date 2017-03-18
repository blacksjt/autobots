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

protected:
  virtual void ProcessReplyData(QNetworkReply* reply);
  virtual void OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
  virtual void OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors);
  
  
private:
private:

};


#endif // AUTOBOTS_TOUTIAO_NETWORK_H