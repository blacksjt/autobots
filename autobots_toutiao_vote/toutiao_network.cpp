#include "toutiao_network.h"
#include "html_parse_utils.h"
#include <QVariant>


toutiao_network::toutiao_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{
  
}

toutiao_network::~toutiao_network()
{

}

void toutiao_network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
  //QString msg = reply->readAll();

  HttpBase::OnAuthenticationRequired(reply, authenticator);
}

void toutiao_network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
  //QString err_msg = reply->readAll();

  foreach(QSslError err, errors)
  {
    QString msg = errors.at(0).errorString();
  }
  
  HttpBase::OnSsllErrors(reply, errors);
}

void toutiao_network::ProcessReplyData(QNetworkReply* reply)
{

}
