#include "http_base.h"

HttpBase::HttpBase(QObject* parent /*= 0*/)
  : QObject(parent)
{
  connect(&networkManager, SIGNAL(authenticationRequired(QNetworkReply*,QAuthenticator*)),
    this, SLOT(OnAuthenticationRequired(QNetworkReply*,QAuthenticator*)));

  connect(&networkManager, SIGNAL(sslErrors(QNetworkReply*,QList<QSslError>)),
    this, SLOT(OnSsllErrors(QNetworkReply*,QList<QSslError>)));

  connect(&networkManager, SIGNAL(finished(QNetworkReply*)),
    this, SLOT(OnNetworkReply(QNetworkReply*)));
}

HttpBase::~HttpBase()
{

}

void HttpBase::ProcessReplyData(QNetworkReply* reply)
{
  // 继承类可以考虑
  reply->deleteLater();
}

void HttpBase::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
  //忽略错误
  reply->ignoreSslErrors();
  reply->deleteLater();
}

void HttpBase::OnNetworkReply(QNetworkReply* reply)
{
  //处理响应数据
  ProcessReplyData(reply);

  //QString str = reply->readAll();

  //删除响应
  //reply->deleteLater();
}

void HttpBase::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
  reply->deleteLater();
}

QNetworkReply* HttpBase::PostRequest(const QUrl& url, const HttpParamList& headers,const HttpParamList& post_data)
{

  if (url.isEmpty() || !url.isValid())
  {
    return NULL;
  }

  QNetworkRequest r(url);

  //设置header
  for (size_t i = 0 ; i < headers.size(); ++i)
  {
    r.setRawHeader(headers[i].strName.toUtf8().data(), headers[i].strValue.toUtf8().data());
  }

  QByteArray request_params;

  for (size_t i = 0 ; i < post_data.size(); ++i)
  {
    QString str_temp;
    if (i == 0)
    {
      str_temp = post_data[i].strName + "=" + post_data[i].strValue;
    }
    else
    {
      str_temp = "&" + post_data[i].strName + "=" + post_data[i].strValue;
    }
    
    request_params.append(str_temp);
  }

  return networkManager.post(r,request_params);

}

QNetworkReply* HttpBase::PostRequest_ssl(const QUrl& url, const HttpParamList& headers,const HttpParamList& post_data)
{

  if (url.isEmpty() || !url.isValid())
  {
    return NULL;
  }

  QNetworkRequest r(url);
  //r.setHeader(QNetworkRequest::CookieHeader, GetCookieString(networkManager.cookieJar()->cookiesForUrl(url)));

  QSslConfiguration conf = r.sslConfiguration();
  conf.setPeerVerifyMode(QSslSocket::VerifyNone);
  conf.setProtocol(QSsl::TlsV1SslV3);
  r.setSslConfiguration(conf);

  //设置header
  for (size_t i = 0 ; i < headers.size(); ++i)
  {
    r.setRawHeader(headers[i].strName.toUtf8().data(), headers[i].strValue.toUtf8().data());
  }

  QByteArray request_params;

  for (size_t i = 0 ; i < post_data.size(); ++i)
  {
    QString str_temp;
    if (i == 0)
    {
      str_temp = post_data[i].strName + "=" + post_data[i].strValue;
    }
    else
    {
      str_temp = "&" + post_data[i].strName + "=" + post_data[i].strValue;
    }

    request_params.append(str_temp);
  }
  
  return networkManager.post(r,request_params);

}


QNetworkReply* HttpBase::GetRequest(const QUrl& url, const HttpParamList& headers)
{
  if (url.isEmpty() || !url.isValid())
  {
    return NULL;
  }

  QNetworkRequest r(url);

  //r.setHeader(QNetworkRequest::CookieHeader, GetCookieString(networkManager.cookieJar()->cookiesForUrl(url)));

  //设置header
  for (size_t i = 0 ; i < headers.size(); ++i)
  {
    r.setRawHeader(headers[i].strName.toUtf8().data(), headers[i].strValue.toUtf8().data());
  }
  //networkManager.cookieJar()
  return networkManager.get(r);
}

QNetworkReply* HttpBase::GetRequest_ssl(const QUrl& url, const HttpParamList& headers)
{
  if (url.isEmpty() || !url.isValid())
  {
    return NULL;
  }

  QNetworkRequest r(url);

  //r.setHeader(QNetworkRequest::CookieHeader, GetCookieString(networkManager.cookieJar()->cookiesForUrl(url)));

  QSslConfiguration conf = r.sslConfiguration();
  conf.setPeerVerifyMode(QSslSocket::VerifyNone);
  conf.setProtocol(QSsl::TlsV1SslV3);
  r.setSslConfiguration(conf);

  //设置header
  for (size_t i = 0 ; i < headers.size(); ++i)
  {
    r.setRawHeader(headers[i].strName.toUtf8().data(), headers[i].strValue.toUtf8().data());
  }
  //networkManager.cookieJar()
  return networkManager.get(r);
}

QNetworkReply* HttpBase::PostRequest_json(const QUrl& url, const HttpParamList& headers,const QJsonDocument& json_doc)
{
  if (url.isEmpty() || !url.isValid())
  {
    return NULL;
  }

  QNetworkRequest r(url);

  //设置header
  for (size_t i = 0 ; i < headers.size(); ++i)
  {
    r.setRawHeader(headers[i].strName.toUtf8().data(), headers[i].strValue.toUtf8().data());
  }

  return networkManager.post(r,json_doc.toJson(QJsonDocument::Compact));
}

