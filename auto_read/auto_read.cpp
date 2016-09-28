#include "auto_read.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "QTextStream"
#include "work_thread.h"
#include "my_work.h"

void waitForSeconds(int n)
{
  QTime time;
  time.start();

  while(time.elapsed() < n*1000)
  {
    QCoreApplication::processEvents();
  }
}

QString GetHost(const QString& str)
{
  int pos1 = str.indexOf("://");

  if (pos1 == -1)
  {
    return "";
  }

  QString sub_str = str.right(str.length() - pos1 - 3);

  pos1 = sub_str.indexOf("/");

  if (pos1 == -1)
  {
    return "";
  }

  QString res_str = sub_str.left(pos1);

  return res_str;
}

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
    m_thread_count = 1;
    ui.setupUi(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_url, SIGNAL(clicked()), this, SLOT(onAddUrl()));

    connect(ui.pushButton_remove_url, SIGNAL(clicked()), this, SLOT(onRemoveUrl()));

    connect(ui.action_import_URLs, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_URLs, SIGNAL(triggered()), this, SLOT(onActClearComments()));
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
  ui.statusBar->showMessage(QStringLiteral("运行开始")); 
  
  int n = 1;

  while(control_status)
  {
    workRun();
    QTime _t;
    _t.start();
    while (_t.elapsed() < m_interval*1000)
    {
      QCoreApplication::processEvents();
    }

    QString num = QString::number(n++);
    ui.statusBar->showMessage(num);
  }
}

void auto_smzdm::onPause()
{
//   for (int i = 0; i < m_work_list.size(); ++i)
//   {
//     if (m_work_list[i] != NULL)
//     {
//       m_work_list[i]->deleteLater();
//       m_work_list[i] = NULL;
//     }
//   }
// 
//   for (int i = 0; i < m_thread_list.size(); ++i)
//   {
//     if (m_thread_list[i] != NULL)
//     {
//       m_thread_list[i]->disconnect();
//       m_thread_list[i]->terminate();
//       m_thread_list[i]->deleteLater();
//       m_thread_list[i] = NULL;
//     }
//   }
// 
//   m_work_list.clear();
//   m_thread_list.clear();

  control_status = false;
  ui.statusBar->showMessage(QStringLiteral("已停止"));
}




void auto_smzdm::onActClearComments()
{
  ui.comboBox_url_list->clear();
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
  
  ui.comboBox_url_list->clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    ui.comboBox_url_list->addItem(line_str);
  }

  file.close();
}

void auto_smzdm::UpdateData()
{
   m_thread_count = ui.spinBox_thread_count->value();
   m_interval = ui.spinBox_interval->value();
 
  int n = ui.comboBox_url_list->count();

  for (int i = 0 ; i < n; ++i)
  {
    QString text = ui.comboBox_url_list->itemText(i);
    if (!text.isEmpty())
    {
      m_url_list.push_back(text);
    } 
  }
}

bool auto_smzdm::CheckInput()
{
  if (ui.comboBox_url_list->count() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

//   if (ui.lineEdit_HOST1->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
//     return false;
//   }

//   if (ui.lineEdit_REFERER->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Referer没有输入")); 
//     return false;
//   }
// 
//   if (ui.lineEdit_news_id->text().isEmpty())
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
//     return false;
//   }
// 
//   if (ui.listWidget_comment_id->count() < 1)
//   {
//     QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
//     return false;
//   }

  return true;
}

void auto_smzdm::onAddUrl()
{
  QString t = ui.comboBox_url_list->currentText();

  if (ui.comboBox_url_list->findText(t) != -1)
  {
    onMsg(QStringLiteral("URL已存在"));
  }
  else
  {
    ui.comboBox_url_list->addItem(t);
  }

  ui.comboBox_url_list->clearEditText();
}

void auto_smzdm::onRemoveUrl()
{
  int index = ui.comboBox_url_list->findText(ui.comboBox_url_list->currentText());
  ui.comboBox_url_list->removeItem(index);
}

void auto_smzdm::onMsg(const QString& msg)
{
  //ui.lineEdit_msg->setText(msg);
  ui.statusBar->showMessage(msg);
}

void auto_smzdm::workRun()
{
  //QNetworkCookieJar* cookie = new QNetworkCookieJar();

  //m_manager.setCookieJar(cookie);
  QNetworkAccessManager mana;

  foreach(QString str, m_url_list)
  {
    //QString str_url1 = m_url;
    QNetworkRequest req;
    QString str_host = GetHost(str);
    req.setUrl(QUrl(str));

    req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
    req.setRawHeader("Cache-Control","no-cache");
    req.setRawHeader("Connection","Keep-Alive");
    req.setRawHeader("Accept-Encoding","gzip, deflate");
	//req.setRawHeader("X-Requested-With", "XMLHttpRequest");
    req.setRawHeader("DNT", "1");
    req.setRawHeader("Accept-Language","zh-CN,zh;q=0.8");
    req.setRawHeader("Connection", "keep-alive");
    req.setRawHeader("Host", str_host.toUtf8().data());
    req.setRawHeader("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/51.0.2704.106 Safari/537.36");

	
	if (str.startsWith("https://",Qt::CaseInsensitive))
	{
		QSslConfiguration conf = req.sslConfiguration();
		conf.setPeerVerifyMode(QSslSocket::VerifyNone);
		conf.setProtocol(QSsl::TlsV1SslV3);
		req.setSslConfiguration(conf);
	}

	QNetworkReply* reply = mana.get(req);

    //#ifdef _DEBUG
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

    QString msg;
    if (reply->error() != QNetworkReply::NoError)
    {
      msg = reply->errorString();
    }

    QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

    int n = statusCodeV.toInt();

    msg = reply->readAll();
    //#endif
    //waitForSeconds(1);
    reply->deleteLater();
  }

  //cookie->deleteLater();

}
