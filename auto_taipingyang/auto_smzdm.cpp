#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include <QTextCodec>
#include "html_parse_utils.h"

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true),m_total(0), QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    connect(&network, SIGNAL(supportsucceed()), this,  SLOT(onSupportSucceed()));
}

auto_smzdm::~auto_smzdm()
{

}

void auto_smzdm::onStart()
{
  if (!CheckInput())
  {
    return;
  }

  UpdateData();
  m_total = 0;

  control_status = true;
  ui.lineEdit_msg->setText(QStringLiteral("运行中"));
  QString msg = QStringLiteral("运行中");

  int count = 0;

  while( control_status )
  {
    QString temp = msg;

    smzdm_run();

    count ++; 

    QString temp2;
    temp2.setNum(count);

    ui.lineEdit_msg->setText(temp + temp2);

    QElapsedTimer t;
    t.start();
    while(t.elapsed()<1000)  
      QCoreApplication::processEvents();

  }
}

void auto_smzdm::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

int auto_smzdm::smzdm_run()
{

  QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

  network.GetManager().setCookieJar(cookie);

  foreach(QString str, m_comment_list)
  {
    QString str_url1 = m_url;

    QUrl url1(str_url1);

    HttpParamList header_list;
    header_list.push_back(HttpParamItem("Referer", m_referer));
    header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
    header_list.push_back(HttpParamItem("Accept","text/html, application/xhtml+xml, */*"));
    header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
    //header_list.push_back(HttpParamItem("Accept-Encoding","gzip,deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","en"));
    header_list.push_back(HttpParamItem("Host", "cmt.pconline.com.cn"));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
    
    HttpParamList post_data;

    int n = rand()%9;
    double d = 0.987654321235647/n;

    QString str_r = QString::number(d, 'f', 15);
//     int p = str_r.indexOf('.');
//     QString temp = str_r.right(str_r.length() - p - 1);
    
    post_data.push_back(HttpParamItem("cid",str));
    post_data.push_back(HttpParamItem("r",str_r));
    post_data.push_back(HttpParamItem("sp", "1"));
    post_data.push_back(HttpParamItem("version", "2"));
    post_data.push_back(HttpParamItem("windowname", "1"));

    network.PostRequest(url1, header_list, post_data);

  }

  cookie->deleteLater();

  return 0;
}

void auto_smzdm::onOneClick()
{
  UpdateData();
  smzdm_run();
}

void smzdm_network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{

}

void smzdm_network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{

}

void smzdm_network::ProcessReplyData(QNetworkReply* reply)
{
  if (reply->error() != QNetworkReply::NoError)
  {
    QString msg = reply->errorString();
    return;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  QVariant content_type = reply->header(QNetworkRequest::ContentTypeHeader);

  QString type_ = content_type.toString();

  QByteArray arr = reply->readAll();

  QString msg = arr;

  if (msg.contains("code:1"))
  {
    emit supportsucceed();
  }

  HttpBase::ProcessReplyData(reply);
}

smzdm_network::smzdm_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

smzdm_network::~smzdm_network()
{

}

void auto_smzdm::onActClearComments()
{
  ui.listWidget_comment_id->clear();
}

void auto_smzdm::onActImportComment()
{
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL,  QStringLiteral("提示"), QStringLiteral("无法打开文件"));  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.listWidget_comment_id->clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    ui.listWidget_comment_id->addItem(new QListWidgetItem(line_str));
  }

  file.close();
}

void auto_smzdm::UpdateData()
{
   m_url = ui.lineEdit_URL1->text();
   m_referer = ui.lineEdit_REFERER->text();
   m_news_id = ui.lineEdit_news_id->text();
   m_comment_list.clear();
 
  int n = ui.listWidget_comment_id->count();

  for (int i = 0 ; i < n; ++i)
  {
    QString text = ui.listWidget_comment_id->item(i)->text();
    if (!text.isEmpty())
    {
      m_comment_list.push_back(text);
    } 
  }
}

bool auto_smzdm::CheckInput()
{
  if (ui.lineEdit_URL1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  //if (ui.lineEdit_HOST1->text().isEmpty())
  //{
  //  QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
  //  return false;
  //}

  if (ui.lineEdit_REFERER->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Referer没有输入")); 
    return false;
  }

//   if (ui.lineEdit_news_id->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
//     return false;
//   }

  if (ui.listWidget_comment_id->count() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
    return false;
  }

  return true;
}

void auto_smzdm::onAddCommentID()
{

  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  ui.listWidget_comment_id->addItem(id);

}

void auto_smzdm::onSupportSucceed()
{
  m_total++;
  QString msg = QStringLiteral("已赞次数：");
  msg += QString::number(m_total);
  ui.lineEdit_msg->setText(msg);
}


void smzdm_network::ParseHtml(const QByteArray rp_data)
{
  HtmlParseUtils html_parse(rp_data.data(), rp_data.length());

  // "查找post_from_id"
//   GumboNode* node = html_parse.FirstElementNode("name", "post_form_id");
// 
//   if (node != NULL)
//   {
//     std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");
// 
//     QString str = QString::fromStdString(post_from_id);
// 
//     if (!str.isEmpty())
//     {
//       emit postIdReady(str);
//     }
//   }
// 
//   // "查找client_id"
//   node = html_parse.FirstElementNode("name", "client_id");
// 
//   if (node != NULL)
//   {
//     std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");
// 
//     QString str = QString::fromStdString(client_id);
// 
//     if (!str.isEmpty())
//     {
//       emit clientID(str);
//     }
//   }
// 
//   // "查找client_id"
//   node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");
// 
//   if (node != NULL)
//   {
//     std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
//     QString str = QString::fromStdString(csrf_token);
//     emit csrftokenReady(str);
//   }
// 
//   QString str = rp_data;
//   if (str.contains("window.opener.user.connected") 
//     || str.contains("window.parent.user.connected") )
//   {
//     emit loginsucceed("111");
//   }
}