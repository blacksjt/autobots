#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent),network(NULL)
{
    ui.setupUi(this);

    //networkManager = new QNetworkAccessManager(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));
}

auto_smzdm::~auto_smzdm()
{

}

void auto_smzdm::onStart()
{

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
	if(network != NULL)
	{
		network->deleteLater();
		network = NULL;
	}
	network = new smzdm_network(this);

  //network.GetManager().setCookieJar(cookie);

  foreach(QString str, m_comment_list)
  {
    //QString str_url1 = str;
    QUrl url1(str);

    HttpParamList header_list;
    header_list.push_back(HttpParamItem("Referer", m_referer));
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
    header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
    header_list.push_back(HttpParamItem("Host", m_host));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
    QNetworkReply* reply = network->GetRequest(url1, header_list);

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
	 QString ret = reply->readAll();
	 ui.lineEdit_msg->setText(ret);
    reply->deleteLater();
  }

  //cookie->deleteLater();

  return 0;
}

void smzdm_network::OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator)
{
  HttpBase::OnAuthenticationRequired(reply, authenticator);
}

void smzdm_network::OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors)
{
  HttpBase::OnSsllErrors(reply, errors);
}

void smzdm_network::ProcessReplyData(QNetworkReply* reply)
{
  //TODO


  //HttpBase::ProcessReplyData(reply);
}

smzdm_network::smzdm_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

smzdm_network::~smzdm_network()
{

}

void auto_smzdm::onAddCommentID()
{

  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  ui.listWidget_comment_id->addItem(id);

}

void auto_smzdm::UpdateData()
{
  m_url = ui.lineEdit_URL1->text();
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
