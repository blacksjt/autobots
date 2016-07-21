#include "toutiao_network.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QJsonDocument>


toutiao_network::toutiao_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{
  
}

toutiao_network::~toutiao_network()
{

}

 void toutiao_network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
 {
   QString msg = reply->readAll();
 
   HttpBase::OnAuthenticationRequired(reply, authenticator);
 }
 
 void toutiao_network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
 {
   QString err_msg = reply->readAll();
 
   foreach(QSslError err, errors)
   {
     QString msg = errors.at(0).errorString();
   }
   
   HttpBase::OnSsllErrors(reply, errors);
 }

void toutiao_network::ProcessReplyData(QNetworkReply* reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    QString msg = reply->errorString();
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
  
  int n = statusCodeV.toInt();

  if (n == 302 || n == 301)
  {
    // 重定向
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    ProcessRedirect(red_url);
  }
  else if (n == 200)
  {
    QByteArray rp_data = reply->readAll();
    if (rp_data.size() != 0)
    {
      QVariant content_type = reply->header(QNetworkRequest::ContentTypeHeader);

      QString type = content_type.toString();

      if (type.contains("html"))
      {
        ParseHtml(rp_data);
      }
      else if (type.contains("json"))
      {
        ParseJson(rp_data);
      } 
    }
  }
  else
  {
    QString text = reply->readAll();
  }
  
  //HttpBase::ProcessReplyData(reply);
}

void toutiao_network::ProcessRedirect(const QUrl& url)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QString str = url.toString();
  if (str.contains("https://",Qt::CaseInsensitive))
  {
    QNetworkReply* reply = GetRequest_ssl(url, header_list);

    QString msg = reply->readAll();
  }
  else
  {
    QNetworkReply* reply = GetRequest(url, header_list);

    QString msg = reply->readAll();
  }
}

void toutiao_network::ParseHtml(const QByteArray rp_data)
{
  HtmlParseUtils html_parse(rp_data.data(), rp_data.length());

  // "查找post_from_id"
  GumboNode* node = html_parse.FirstElementNode("name", "post_form_id");

  if (node != NULL)
  {
    std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString str = QString::fromStdString(post_from_id);

    if (!str.isEmpty())
    {
      emit postIdReady(str);
    }
  }

  // "查找client_id"
  node = html_parse.FirstElementNode("name", "client_id");

  if (node != NULL)
  {
    std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString str = QString::fromStdString(client_id);

    if (!str.isEmpty())
    {
      emit clientID(str);
    }
  }

  // "查找client_id"
  node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");

  if (node != NULL)
  {
    std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
    QString str = QString::fromStdString(csrf_token);
    emit csrftokenReady(str);
  }

  QString str = rp_data;
  if (str.contains("window.opener.user.connected") 
    || str.contains("window.parent.user.connected") )
  {
    emit loginsucceed("111");
  }
}

void toutiao_network::ParseJson(const QByteArray byte_arr)
{
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(byte_arr, &json_error); 
  if(json_error.error == QJsonParseError::NoError) 
  {  
    if(parse_doucment.isObject())  
    {  
      QJsonObject obj = parse_doucment.object(); 
      if(obj.contains("message"))  
      {  
        QJsonValue name_value = obj.take("message");
        if(name_value.isString())
        {
          QString str_value = name_value.toString(); // success
          if (str_value == "success")
          {
            // 发帖成功
            if(obj.contains("comment"))  
            { 
              QString comment_str;
              double comment_id = 0;
              QJsonValue comment_obj = obj.take("comment");
              if(comment_obj.isObject())
              {
                QJsonObject obj = comment_obj.toObject();
                if (obj.contains("text"))
                {
                  QJsonValue text_value = obj.take("text");
                  if (text_value.isString())
                  {
                    comment_str = text_value.toString();
                    //emit fatiesucceed(comment_str);
                  }
                } 
                if (obj.contains("id"))
                {
                  QJsonValue text_value = obj.take("id");
                  if (text_value.isDouble())
                  {
                    comment_id = text_value.toDouble();
                    //QString comment_str = QString::number(d, 'f', 0);
                  }
                } 
              }

              if (!comment_str.isEmpty())
              {
                emit fatiesucceed(comment_str, comment_id);
              }
            }
          }
        }
      }

      //if(obj.contains("status"))  
      //{  
      //  QJsonValue name_value = obj.take("status");
      //  QJsonValue::Type t = name_value.type();
      //  if(name_value.isString())
      //  {
      //    QString name = name_value.toString();
      //  }
      //}
    }
  }
}


