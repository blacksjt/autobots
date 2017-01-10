#ifndef HTTP_BASE_H
#define HTTP_BASE_H

#include "http_base_global.h"
#include <QObject>
#include <QtNetwork>
#include <QList>
#include <QString>

//http url ����
struct HttpParamItem
{
  HttpParamItem()
    :strValue("null")
  {

  }

  HttpParamItem(const QString& name ,const QString& value)
    :strName(name),
    strValue(value)
  {

  }

  QString strName;
  QString strValue;
};
typedef QList<HttpParamItem> HttpParamList;

class HTTP_BASE_EXPORT HttpBase : public QObject
{
  Q_OBJECT
public:
    HttpBase(QObject* parent = 0);
    ~HttpBase();

public:
  //ʹ��post��������url����
  QNetworkReply* PostRequest(const QUrl& url, const HttpParamList& headers,const HttpParamList& post_data);
  QNetworkReply* PostRequest_ssl(const QUrl& url, const HttpParamList& headers,const HttpParamList& post_data);
  QNetworkReply* PostRequest_json(const QUrl& url, const HttpParamList& headers,const QJsonDocument& json_doc);
  QNetworkReply* PostRequest_json_ssl(const QUrl& url, const HttpParamList& headers, const QJsonDocument& json_doc);

  //ʹ��get��������url����
  QNetworkReply* GetRequest(const QUrl& url, const HttpParamList& headers);

  QNetworkReply* GetRequest_ssl(const QUrl& url, const HttpParamList& headers);

  QNetworkAccessManager& GetManager(){return networkManager;}
  //���ô�����Ϣ
//  void SetProxy(const HttpProxyInfo& info);

protected:
  //������Ӧ����,��������Ҫ��д�˺���
  virtual void ProcessReplyData(QNetworkReply* reply);

protected slots:
    virtual void OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
    virtual void OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private slots:    
      void OnNetworkReply(QNetworkReply* reply);

private:
  QNetworkAccessManager networkManager;
};

#endif // HTTP_BASE_H
