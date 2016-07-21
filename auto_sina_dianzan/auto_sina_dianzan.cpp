#include "auto_sina_dianzan.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
    ui.setupUi(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));
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
    while(t.elapsed()<1500)  
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
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
    header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
    header_list.push_back(HttpParamItem("Host", m_host));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
    
    HttpParamList post_data;
    post_data.push_back(HttpParamItem("callback","function(a){}"));
    post_data.push_back(HttpParamItem("channel",m_chanel));
    post_data.push_back(HttpParamItem("domain", "sina.com.cn"));
    post_data.push_back(HttpParamItem("format", "js"));
    post_data.push_back(HttpParamItem("newsid", m_news_id));
    post_data.push_back(HttpParamItem("parent", str));
    post_data.push_back(HttpParamItem("vote", "1"));

    QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);
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

    QString rp_data = reply->readAll();

    reply->deleteLater();
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
  //TODO
//   if (reply->error() != QNetworkReply::NoError)
//   {
//     QString msg = reply->errorString();
//   }
// 
//   QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  
// 
//   int n = statusCodeV.toInt();
// 
//   QString text = reply->readAll();
// 
//   HttpBase::ProcessReplyData(reply);
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
   m_chanel = ui.lineEdit_news_chanel->text();
   m_host = ui.lineEdit_host->text();
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

  if (ui.lineEdit_host->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
    return false;
  }

  if (ui.lineEdit_REFERER->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Referer没有输入")); 
    return false;
  }

  if (ui.lineEdit_news_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
    return false;
  }

  if (ui.lineEdit_news_chanel->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("输入新闻的频道")); 
    return false;
  }

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