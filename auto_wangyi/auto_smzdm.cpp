#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "work_thread.h"
#include <QtNetwork>

const int TIMEOUT = 5*1000;

auto_smzdm::auto_smzdm(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);

    //networkManager = new QNetworkAccessManager(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));


}

auto_smzdm::~auto_smzdm()
{
  if (mthread != NULL)
  {
    mthread->disconnect();
    mthread->terminate();
    mthread->deleteLater();
    mthread = NULL;
  }
}

void auto_smzdm::onStart()
{
  if (!CheckInput())
  {
    return;
  }

  UpdateData();

  //control_status = true;
  ui.lineEdit_msg->setText(QStringLiteral("运行中"));
  //QString msg = QStringLiteral("运行中");

//   while (true)
//   {
//     smzdm_run();
// 
//     QTime t;
//     t.start();
//     while(t.elapsed() < 5000)
//     {
//       QCoreApplication::processEvents();
//     }
//   }

  // 开启线程进行工作
  mthread = new WorkThread(this);
  connect(mthread, SIGNAL(emitMsg(const QString&)), this, SLOT(onMsg(const QString&)),Qt::QueuedConnection);
  mthread->SetParameters(m_token_url,m_referer,m_host,m_comment_list);
  mthread->start();

}

void auto_smzdm::onPause()
{
  //control_status = false;
  if (mthread != NULL)
  {
    mthread->disconnect();
    mthread->terminate();
    mthread->deleteLater();
    mthread = NULL;
  }
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}

 int auto_smzdm::smzdm_run()
 {
  QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
  m_manager.setCookieJar(cookie);

  foreach(QString str, m_comment_list)
  {
    QNetworkRequest req;

    QString str_url1 = str;//"http://comment.tech.163.com/reply/upvote/tech_bbs/B98L34L200094P0U_" + str;

    //QUrl url1(str_url1);
    req.setUrl(QUrl(str_url1));

    QString origin = "http://" + m_host;

    req.setRawHeader("Cache-Control","no-cache");
    req.setRawHeader("Accept","*/*");
    req.setRawHeader("Connection","Keep-Alive");
    req.setRawHeader("Accept-Encoding","gzip, deflate");
    req.setRawHeader("Accept-Language","zh-CN");
    req.setRawHeader("Content-Type","application/x-www-form-urlencoded");
//    req.setRawHeader("Content-Length","9");
    req.setRawHeader("Host", m_host.toUtf8().data());
    req.setRawHeader("Origin", origin.toUtf8().data());
    req.setRawHeader("Referer", m_referer.toUtf8().data());
    req.setRawHeader("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

    /*req.setAttribute()*/
    QByteArray request_params;
//    request_params.append("undefined");
    QNetworkReply* reply = m_manager.post(req,request_params);

    while(!reply->isFinished())
    {
      QCoreApplication::processEvents();
    }

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
      msg = reply->errorString();
    }

    QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

    int n = statusCodeV.toInt();
    //QNetworkReply* reply = m_manager.get(req);
    //msleep(100);

    reply->deleteLater();
  }

  cookie->deleteLater();

  return 0;

//    QStringList strlist;
//    strlist = m_comment_list;
//  
//    QNetworkCookieJar* cookie = new QNetworkCookieJar(this);
//  
//    network.GetManager().setCookieJar(cookie);
//  
//    foreach(QString str, strlist)
//    {
//      QString str_url1 = str;//"http://comment.tech.163.com/reply/upvote/tech_bbs/B98L34L200094P0U_" + str;
//  
//      QUrl url1(str_url1);
//  
//      HttpParamList header_list;
//      QString origin = "http://" + m_host;
//      
//      header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
//      header_list.push_back(HttpParamItem("Accept","*/*"));
//      header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
//      header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
//      header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
//      header_list.push_back(HttpParamItem("Content-Type","application/x-www-form-urlencoded"));
//      header_list.push_back(HttpParamItem("Host", m_host));
//      header_list.push_back(HttpParamItem("Origin", origin));
//      header_list.push_back(HttpParamItem("Referer", m_referer));
//      header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
//      
//      HttpParamList post_data;
//      QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);
//  
//      QTime _t;
//      _t.start();
//  
//      while (reply && !reply->isFinished())
//      {
//        QCoreApplication::processEvents();
//        if (_t.elapsed() >= TIMEOUT) {
//          break;
//        }
//      }
//  
//      reply->deleteLater();
//    }
//  
//      cookie->deleteLater();
//  
//    return 0;
 }

void auto_smzdm::onOneClick()
{
  //smzdm_run();
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
  m_token_url = ui.lineEdit_token->text();
  m_referer = ui.lineEdit_REFERER1->text();
  m_host = ui.lineEdit_HOST1->text();
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
//   if (ui.lineEdit_URL1->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
//     return false;
//   }

  if (ui.lineEdit_HOST1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
    return false;
  }

  if (ui.lineEdit_REFERER1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻网址没有输入")); 
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

void auto_smzdm::onMsg(const QString& msg)
{
  ui.lineEdit_msg->setText(msg);
}