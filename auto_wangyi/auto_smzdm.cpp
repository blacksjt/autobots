#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "work_thread.h"
#include <QtNetwork>

const int TIMEOUT = 5*1000;

void waitForSeconds(int n)
{
	QTime time;
	time.start();

	while (time.elapsed() < n * 1000)
	{
		QCoreApplication::processEvents();
	}
}

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
//   if (mthread != NULL)
//   {
//     mthread->disconnect();
//     mthread->terminate();
//     mthread->deleteLater();
//     mthread = NULL;
//   }
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
  ui.pushButton_start->setEnabled(false);

  int ncount = 1;

  while (control_status)
  {
    smzdm_run();

	onMsg(QString::number(ncount++));

    QTime t;
    t.start();
    while(t.elapsed() < 1000)
    {
      QCoreApplication::processEvents();
    }
  }

  // 开启线程进行工作
//   mthread = new WorkThread(this);
//   connect(mthread, SIGNAL(emitMsg(const QString&)), this, SLOT(onMsg(const QString&)),Qt::QueuedConnection);
//   mthread->SetParameters(m_token_url,m_referer,m_host,m_comment_list);
//   mthread->start();

}

void auto_smzdm::onPause()
{
  control_status = false;
//   if (mthread != NULL)
//   {
//     mthread->disconnect();
//     mthread->terminate();
//     mthread->deleteLater();
//     mthread = NULL;
//   }
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
  ui.pushButton_start->setEnabled(true);
}

 int auto_smzdm::smzdm_run()
 {
	 QNetworkCookieJar* cookie = new QNetworkCookieJar();
	 m_manager.setCookieJar(cookie);

	 //QString token = GetToken();

	 //QString path = GetMatchedText(m_token_url);
	 QString token = GetToken();

	 foreach(QString str, m_comment_list)
	 {
		 //QString token = GetToken();
		 if (token.isEmpty())
		 {
			 token = GetToken();
			 if (token.isEmpty())
			 {
				 continue;
			 }
		 }
		 QString path = GetMatchedText(m_token_url);

		 QNetworkRequest req;

		 QString str_url1 = path + str + "/action/upvote?ntoken=" + token + "&ibc=newspc";

		 //QUrl url1(str_url1);
		 req.setUrl(QUrl(str_url1));

		 QString origin = "http://" + m_host;

		 req.setRawHeader("Cache-Control", "no-cache");
		 req.setRawHeader("Accept", "*/*");
		 req.setRawHeader("Connection", "Keep-Alive");
		 req.setRawHeader("Accept-Encoding", "gzip, deflate");
		 req.setRawHeader("Accept-Language", "zh-CN");
		 req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
		 req.setRawHeader("X-Requested-With", "XMLHttpRequest");
		 req.setRawHeader("Host", m_host.toUtf8().data());
		 req.setRawHeader("Origin", origin.toUtf8().data());
		 req.setRawHeader("Referer", m_referer.toUtf8().data());
		 req.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

		 /*req.setAttribute()*/
		 QByteArray request_params;
		 //request_params.append("undefined");
		 QNetworkReply* reply = m_manager.post(req, request_params);

		 QTime t;
		 t.start();
		 while (!reply->isFinished())
		 {
			 QCoreApplication::processEvents();
			 if (t.elapsed() >= 10 * 1000) {
				 break;
			 }
		 }
		 // 
		 QString msg;
		 if (reply->error() != QNetworkReply::NoError)
		 {
			 msg = reply->errorString();
			 //emitMsg(msg);
			 reply->deleteLater();
			 ui.statusBar->showMessage(msg);
			 continue;
		 }

		 QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

		 int n = statusCodeV.toInt();


		 if (n == 200)
		 {
			 //onMsg(QStringLiteral("点赞成功"));
			 ui.statusBar->showMessage(QStringLiteral("点赞成功"));
		 }
		 else if (n == 429)
		 {
			 //onMsg(QStringLiteral("频率太高"));
			 ui.statusBar->showMessage(QStringLiteral("频率太高"));
		 }

		 msg = reply->readAll();

		 reply->deleteLater();

		 waitForSeconds(1);
	 }

	 cookie->deleteLater();
	 return 0;
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

QString auto_smzdm::GetMatchedText(const QString& text)
{
	//QString s = "http://comment.dy.163.com/api/v1/products/a2869674571f77b5a0867c3d71db5856/threads/C1G1285K0511A5KT/comments/gentoken?ibc=newspc";
	//     QString pattern = "(.*)gentoken?ibc=newspc";
	//     QRegExp regexp(pattern, Qt::CaseInsensitive);
	// 
	//     int pos = text.indexOf(regexp);
	// 
	//     QString s1 = regexp.cap(0);
	//     QString s2 = regexp.cap(1);

	int pos = text.indexOf("gentoken?ibc=newspc");

	QString s2 = text.left(pos);

	return s2;
}

QString auto_smzdm::GetToken()
{
	QNetworkRequest req;

	QString str_url1 = m_token_url;

	req.setUrl(QUrl(str_url1));

	QString origin = "http://" + m_host;

	req.setRawHeader("Cache-Control", "no-cache");
	req.setRawHeader("Accept", "*/*");
	req.setRawHeader("Connection", "Keep-Alive");
	//req.setRawHeader("Accept-Encoding", "gzip, deflate");
	req.setRawHeader("Accept-Language", "zh-CN");
	req.setRawHeader("Content-Type", "application/x-www-form-urlencoded");
	req.setRawHeader("Content-Length", "0");
	req.setRawHeader("X-Requested-With", "XMLHttpRequest");
	req.setRawHeader("Host", m_host.toUtf8().data());
	req.setRawHeader("Origin", origin.toUtf8().data());
	req.setRawHeader("Referer", m_referer.toUtf8().data());
	req.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

	/*req.setAttribute()*/
	QByteArray request_params;
	//request_params.append("undefined");
	QNetworkReply* reply = m_manager.post(req, request_params);

	QTime t;
	t.start();
	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
		if (t.elapsed() >= 10 * 1000) {
			break;
		}
	}

	QString msg;
	if (reply->error() != QNetworkReply::NoError)
	{
		msg = reply->errorString();
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	QByteArray data = reply->readAll();

	QString ret = ParseToken(data);

	reply->deleteLater();

	return ret;
}

QString auto_smzdm::ParseToken(const QByteArray & data)
{
	QString ret;
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("gentoken"))
			{
				QJsonValue name_value = obj.take("gentoken");
				if (name_value.isString())
				{
					ret = name_value.toString();
				}
			}
		}
	}

	return ret;
}