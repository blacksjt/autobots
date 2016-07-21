#include "auto_smzdm.h"

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
    ui.setupUi(this);

    //networkManager = new QNetworkAccessManager(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));
}

auto_smzdm::~auto_smzdm()
{

}

void auto_smzdm::onStart()
{
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
  //QString str_url1 = "http://smzdm01.wt-cn01.net/999768690672041/wt?p=325,www_smzdm_com.p.715043.,1,1366x768,24,1,1446799583196,2,1366x226,1&tz=8&eid=2144679956800903080&one=0&fns=0&ct=javascript%3Avoid(0)%3B&ctx=1&la=zh&pu=http%3A%2F%2Fwww.smzdm.com%2Fp%2F715043%2F&eor=1";
  QString str_url1 = ui.lineEdit_URL1->text();
  QUrl url1(str_url1);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Referer", ui.lineEdit_REFERER1->text()));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("Host", ui.lineEdit_HOST1->text()));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  network.GetRequest(url1, header_list);



  return 0;
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


  HttpBase::ProcessReplyData(reply);
}

smzdm_network::smzdm_network(QObject* parent /*= 0*/)
  : HttpBase(parent)
{

}

smzdm_network::~smzdm_network()
{

}
