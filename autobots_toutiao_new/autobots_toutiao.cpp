#include "autobots_toutiao.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "code_dlg.h"
#include "vlidatecodeonline.h"

const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 1*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),
    m_client_id("394e2173327e4ead8302dc27f4ae8879")
{
    ui.setupUi(this);

    initialize();

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

    connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));

    connect(ui.pushButton_compute_num, SIGNAL(clicked()), this, SLOT(onComputeNumber()));

    connect(ui.pushButton_count, SIGNAL(clicked()), this, SLOT(onCountAll()));
}

autobots_toutiao::~autobots_toutiao()
{

}

void autobots_toutiao::onStart()
{
  // �������
  if (!CheckInput())
  {
    return;
  }

  // ���½�������
  UpdateData();

  //bool res = GetDongtaiIDMap();
  //ui.lineEdit_msg->setText(QStringLiteral("��ȡ��̬ID"));
  
  control_status = true;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    if (!control_status || m_comment_list.isEmpty())
    {
      break;
    }

    QNetworkCookieJar* cookie = new QNetworkCookieJar();

    network.GetManager().setCookieJar(cookie);

    // ��ȡCSRF TOKEN
    GetContent();
    if (m_csrf_token.isEmpty())
    {
		m_csrf_token = "undefined";
		//GetContent();
		//ui.lineEdit_msg->setText(QStringLiteral("������վ��..."));
		//if (m_csrf_token.isEmpty())
		//{
		//  continue;
		//}
    }

    if (!RequestForRenren())
    {
      ui.lineEdit_msg->setText(QStringLiteral("����ʧ��..."));
      continue;
    }

    AccountParam ac = m_account_list.at(i);

    if (!AuthorByRenren(ac._id, ac._password))
    {
      ui.lineEdit_msg->setText(QStringLiteral("��½ʧ��..."));
      ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(255,0,0, 180));
      continue;
    }

    ui.lineEdit_msg->setText(QStringLiteral("��½�ɹ�"));
    ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(0,255,0, 180));

    bool do_up = DoAction();

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<500 )  
      QCoreApplication::processEvents();

//     if (m_current_count >= m_base_count)
//     {
//       // �Ѿ��ﵽ������,ÿ��һ��������ټ���һ��
//       // �������һ��������
//       m_comment_list.pop_back();
//       //m_comment_item_list.pop_back();
//       int n = GetRandom();
//       m_base_count += n;
//     }

    t2.restart();
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    cookie->deleteLater();
  }

  ui.lineEdit_msg->setText(QStringLiteral("�����"));
}

void autobots_toutiao::Logout()
{
  QString str_url_1 = "http://web.toutiao.com/auth/logout/";

  QUrl url_1(str_url_1);

  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  ////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", m_url));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* reply_1 = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();

  while(reply_1 && !reply_1->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= 2000) {
      break;
    }
  }
}

void autobots_toutiao::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("��ֹͣ"));
}

bool autobots_toutiao::DoAction()
{
  //http://www.toutiao.com/api/comment/digg/
  QString str_url1 = "http://www.toutiao.com/api/comment/digg/";

  QUrl url1;
  url1.setUrl(str_url1);

  QList<int> remove_list;

  for(int i = 0; i <m_comment_list.size(); ++i)
  {
     QString str_id = m_comment_list[i]->text(0);
// 	QString str_d_id;
// 	if (m_id_dongtaiid.find(str_id) != m_id_dongtaiid.end())
// 	{
// 		str_d_id = m_id_dongtaiid[str_id];
// 	}
// 	else
// 	{
// 		continue;
// 	}

	 HttpParamList header_list;
	 header_list.push_back(HttpParamItem("Accept", "text/javascript, text/html, application/xml, text/xml, */*"));
	 header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	 header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	 header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	 header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	 header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	 header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
	 header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
	 header_list.push_back(HttpParamItem("Referer", m_url));
	 header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
	 header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));

    HttpParamList post_data;
    post_data.push_back(HttpParamItem("action", "digg"));
    post_data.push_back(HttpParamItem("comment_id", str_id));
	//post_data.push_back(HttpParamItem("dongtai_id", str_d_id));
    post_data.push_back(HttpParamItem("group_id", m_group_id));
	//post_data.push_back(HttpParamItem("item_id", m_item_id));

    QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

    QTime _t;
    _t.start();

    while(reply && !reply->isFinished())
    {
      QCoreApplication::processEvents();
      if (_t.elapsed() >= TIMEOUT1) {
        break;
      }
    }

    if (reply == NULL || (reply->error() != QNetworkReply::NoError) )
    {
      continue;
    }

    QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

    int n = statusCodeV.toInt();

    QByteArray data = reply->readAll();

    int count = 0;
    bool res = UpdateDiggCount(data, count);

    if (res)
    {
      QString ct = QString::number(count);
      m_comment_list[i]->setText(1, ct);

      int max = m_comment_list[i]->text(2).toInt();

      if (count >= max)
      {
        remove_list.push_back(i);
      }  
    }
  }
  
  for (int j = 0; j < remove_list.size(); ++j)
  {
    m_comment_list.removeAt(remove_list[j]);
  }

  return true;
}

bool autobots_toutiao::GetContent()
{
  QString str_url_1 = m_url;

  QUrl url_1(str_url_1);
  url_1.setUrl(str_url_1);
  HttpParamList header_list1;
  header_list1.push_back(HttpParamItem("Accept",	"application/json, text/javascript, */*; q=0.01"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", "http://www.toutiao.com/"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* rp = network.GetRequest(url_1, header_list1);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while (rp && !rp->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  bool res = GetCsrfToken(rp->readAll());

  if (rp!=NULL)
    rp->deleteLater();

  return  res;

}

bool autobots_toutiao::RequestForRenren()
{
  QString str_url1 = "http://toutiao.com/auth/connect/?type=toutiao&platform=renren_sns";

  QUrl url1(str_url1);

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(url1, header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  bool res = false;

  if (n == 302 || n == 301)
  {
    // �ض���
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    res = ProcessRedirectSSL(str);
  }
  else
  {
    res = false;
  }

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}


bool autobots_toutiao::AuthorByRenren(const QString& name, const QString& password)
{
  //1.������֤��
  QString vcode, code_sign;
  bool need_code = NeedValidateCode(name, vcode, code_sign);
  QString str_need_code = need_code ? "true" : "";

  QString str_url1 = "https://graph.renren.com/oauth/grant";

  QUrl url1(str_url1);

  QString str_temp = "http://graph.renren.com/oauth/grant?client_id=" + m_client_id + "&redirect_uri=http://api.snssdk.com/auth/login_success/&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000";

  HttpParamList header_list;
  //header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
  header_list.push_back(HttpParamItem("Referer", str_temp));
  //Referer	
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("authFeed","true"));
  post_data.push_back(HttpParamItem("authorizeOrigin","00000"));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
  post_data.push_back(HttpParamItem("display","page"));
  post_data.push_back(HttpParamItem("follow","true"));
  post_data.push_back(HttpParamItem("icode",vcode));
  post_data.push_back(HttpParamItem("isNeedIcode",str_need_code));
  post_data.push_back(HttpParamItem("login_type","false"));
  post_data.push_back(HttpParamItem("password",password));
  post_data.push_back(HttpParamItem("porigin","80100"));
  post_data.push_back(HttpParamItem("post_form_id", m_post_id));
  post_data.push_back(HttpParamItem("redirect_uri","http://api.snssdk.com/auth/login_success/"));
  post_data.push_back(HttpParamItem("response_type","code"));
  post_data.push_back(HttpParamItem("scope","status_update photo_upload create_album"));
  post_data.push_back(HttpParamItem("secure","true"));
  post_data.push_back(HttpParamItem("state","renren_sns__0____toutiao____2__0__24"));
  post_data.push_back(HttpParamItem("username",name));

  QNetworkReply* reply = network.PostRequest_ssl(url1, header_list,post_data);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while (reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  bool res = false;
  if (n == 302 || n == 301)
  {
    // �ض���
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    int r = ProcessRedirectLoginGet(str);

    if (r == 0)
    {
      res = true;
    }
    else if (r = -2)
    {
      // ��֤�����
      VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
      obj->ReportError("bestsalt",code_sign);
    }
  }
  else
  {
    res = false;
  }

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

void autobots_toutiao::onAddCommentID()
{
  
  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  AddToTree(id);
}

void autobots_toutiao::UpdateData()
{
  m_url = ui.lineEdit_url->text();
  m_group_id = ui.lineEdit_page_id->text();
  m_item_id = ui.lineEdit_item_id->text();
//  m_base_count = ui.spinBox_min_count->value();
  m_comment_list.clear();
  m_account_list.clear();
  m_code_online = true;

  int n = ui.treeWidget_comment_id->topLevelItemCount();

  for (int i = 0 ; i < n; ++i)
  {
    //QString text = ui.treeWidget_comment_id->topLevelItem(i)->text(0);
    m_comment_list.push_back(ui.treeWidget_comment_id->topLevelItem(i));
    //m_comment_item_list.push_back();
  }

  n = ui.tableWidget_account_id->rowCount();

  for (int i = 0 ; i < n; ++i)
  {
    QString name = ui.tableWidget_account_id->item(i, 0)->text();
    QString password = ui.tableWidget_account_id->item(i, 1)->text();
    m_account_list.push_back(AccountParam(name,password));
    //m_account_row_list.push_back(i);
  }

}

void autobots_toutiao::initialize()
{
  ui.tableWidget_account_id->setColumnWidth(0, 200);
  ui.tableWidget_account_id->setColumnWidth(1, 150);
//  m_current_count = 0;
}

void autobots_toutiao::onActFromTxt()
{
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��ѡ���ļ�·��"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //�ж��û��Ƿ�ѡ�����ļ�
  {
    //emitLogMessage(QStringLiteral("ȡ�����ļ�"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL, "��ʾ", "�޷����ļ�");  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  //m_account_row_list.clear();
  m_account_list.clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();

    QStringList str_list = line_str.split("----");

    if (str_list.size() >= 2)
    {
      ui.tableWidget_account_id->setRowCount(ui.tableWidget_account_id->rowCount()+1);
      QString name = str_list[0];
      QString password = str_list[1];
      QTableWidgetItem* item = new QTableWidgetItem(name);
      ui.tableWidget_account_id->setItem(n_row, 0, item);
      item = new QTableWidgetItem(password);
      ui.tableWidget_account_id->setItem(n_row, 1, item);
      n_row++;
    }
  }

  file.close();
}

void autobots_toutiao::onActImportComment()
{
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��ѡ���ļ�·��"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //�ж��û��Ƿ�ѡ�����ļ�
  {
    //emitLogMessage(QStringLiteral("ȡ�����ļ�"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL, "��ʾ", "�޷����ļ�");  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.treeWidget_comment_id->clear();
  //m_comment_item_list.clear();
  m_comment_list.clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    if (!line_str.isEmpty())
    {
      AddToTree(line_str);
    }
  }

  file.close();
}

void autobots_toutiao::onActClearComments()
{
  ui.treeWidget_comment_id->clear();
  m_comment_list.clear();
}

void autobots_toutiao::onActClearAccounts()
{
  ui.tableWidget_account_id->clearContents();
  ui.tableWidget_account_id->setRowCount(0);
  m_account_list.clear();
}

bool autobots_toutiao::CheckInput()
{
  if (ui.lineEdit_url->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("URLû������")); 
    return false;
  }

  if (ui.lineEdit_page_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("����IDû������")); 
    return false;
  }

  if (ui.treeWidget_comment_id->topLevelItemCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("û�����۵�ID")); 
    return false;
  }

  if (ui.tableWidget_account_id->rowCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("�뵼���˺�")); 
    return false;
  }
  
  for (int i = 0; i < ui.treeWidget_comment_id->topLevelItemCount(); ++i)
  {
    // ��һ�в��գ������пգ���δ������ЧĿ����
    if (!ui.treeWidget_comment_id->topLevelItem(i)->text(0).isEmpty() &&
      ui.treeWidget_comment_id->topLevelItem(i)->text(2).isEmpty())
    {
      QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("������Ŀ�����������Զ�����Ŀ������")); 
      return false;
    }
  }

  return true;
}

void autobots_toutiao::AddToTree(const QString& text)
{
  QTreeWidgetItem* item = new QTreeWidgetItem(ui.treeWidget_comment_id);
  item->setText(0,text);
  item->setFlags(item->flags() | Qt::ItemIsEditable);  
  ui.treeWidget_comment_id->addTopLevelItem(item);
}

bool autobots_toutiao::UpdateDiggCount(const QByteArray& data, int& count_)
{
	bool ret = false;
  QJsonParseError json_error;
  QJsonDocument parse_doucment = QJsonDocument::fromJson(data, &json_error); 
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
          QString msg = name_value.toString();

          if (msg == "success" )
          {
			ret = true;
          }
        }

      }

	  if (obj.contains("data"))
	  {
		  QJsonValue data_value = obj.take("data");

		  QJsonObject data_ojb = data_value.toObject();

		  double comment_id = 0;
		  if (data_ojb.contains("digg_count"))
		  {
			  QJsonValue count_value = data_ojb.take("digg_count");
			  count_ = count_value.toDouble();	  
		  }
	  }
    }
  }

  return ret;
}

bool autobots_toutiao::GetCsrfToken(const QByteArray& rp_data)
{
  HtmlParseUtils html_parse(rp_data.data(), rp_data.length());

  // "����csrftoken"
  GumboNode*  node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");

  if (node != NULL)
  {
    std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
    m_csrf_token = QString::fromStdString(csrf_token);
    return true;
  }

  return false;
}

bool autobots_toutiao::ProcessRedirectSSL(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  if (!str.contains("https://",Qt::CaseInsensitive))
  {
    return false;
  }

  QNetworkReply* reply = network.GetRequest_ssl(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  bool res = false;
  if (n == 302 || n == 301)
  {
    // �ض���
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    res= ProcessRedirectGet(str);
  }
  else
  {
    res= false;
  }

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

bool autobots_toutiao::ProcessRedirectGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 200)
  {
    return false;
  }

  QByteArray data = reply->readAll();

  bool res = GetPostId(data);

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

bool autobots_toutiao::GetPostId(const QByteArray& arr)
{
  HtmlParseUtils html_parse(arr.data(), arr.length());

  // "����post_from_id"
  GumboNode* node = html_parse.FirstElementNode("name", "post_form_id");

  if (node != NULL)
  {
    std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

    QString str = QString::fromStdString(post_from_id);

    m_post_id = str;
  }

  // "����client_id"
  node = html_parse.FirstElementNode("name", "client_id");

  if (node != NULL)
  {
    std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

    m_client_id = QString::fromStdString(client_id);
  }

  if (m_post_id.isEmpty() || m_client_id.isEmpty())
  {
    return false;
  }

  return true;
}

// 0 ������-1 δ֪���� -2 ��֤�����
int autobots_toutiao::ProcessRedirectLoginGet(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","api.snssdk.com"));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    if(reply != NULL)
    {
      QString t = reply->errorString();

      int n = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();

      t = reply->readAll();
      //reply->deleteLater();
      return ProcessRedirectLoginGetTemp(str);
    }
    else
    {
      return -1;
    }   
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();
  int res = -1;
  if (n == 302 || n == 301)
  {
    // �ض���
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    res = ProcessRedirectLoginGet2(str) ? 0 : -1;
  }
  else if (n == 302)
  {
    QString str = reply->readAll();
    if (str.contains(QStringLiteral("��֤�����")))
    {
      res = -2;
    }
  }
  else if (n == 500)
  {
    // �Ҳ���ҳ��
    res = ProcessRedirectLoginGetTemp(str);
  }
  else
  {
    QString t = reply->readAll();
  }

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

bool autobots_toutiao::ProcessRedirectLoginGet2(const QString& str)
{
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 200)
  {
    return false;
  }

  QByteArray data = reply->readAll();

  reply->deleteLater();

  QString temp_s = data;
  if (temp_s.contains("window.opener.user.connected") 
    || temp_s.contains("window.parent.user.connected") )
  {
    return true;
  }

  return false;
}

bool autobots_toutiao::NeedValidateCode(const QString& name, QString& vcode, QString& code_sign)
{
  QString str_url1 = "http://graph.renren.com/icode/check";

  QUrl url1(str_url1);
  QString str_temp = "http://graph.renren.com/oauth/grant?client_id=" + m_client_id + "&redirect_uri=http://api.snssdk.com/auth/login_success/&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000";
  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN"));
  header_list.push_back(HttpParamItem("Host", "graph.renren.com"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
  header_list.push_back(HttpParamItem("Referer",str_temp));
  //network.GetManager().setCookieJar(new QNetworkCookieJar(this));

  HttpParamList post_data;
  post_data.push_back(HttpParamItem("authFeed","true"));
  post_data.push_back(HttpParamItem("authorizeOrigin","00000"));
  post_data.push_back(HttpParamItem("client_id", m_client_id));
  post_data.push_back(HttpParamItem("display","page"));
  post_data.push_back(HttpParamItem("follow","true"));
  post_data.push_back(HttpParamItem("icode",""));
  post_data.push_back(HttpParamItem("isNeedIcode",""));
  post_data.push_back(HttpParamItem("login_type","false"));
  post_data.push_back(HttpParamItem("password",""));
  post_data.push_back(HttpParamItem("porigin","80100"));
  post_data.push_back(HttpParamItem("post_form_id", m_post_id));
  post_data.push_back(HttpParamItem("redirect_uri","http://api.snssdk.com/auth/login_success/"));
  post_data.push_back(HttpParamItem("response_type","code"));
  post_data.push_back(HttpParamItem("scope","status_update photo_upload create_album"));
  post_data.push_back(HttpParamItem("secure","true"));
  post_data.push_back(HttpParamItem("state","renren_sns__0____toutiao____2__0__24"));
  post_data.push_back(HttpParamItem("username",name));

  QNetworkReply* reply = network.PostRequest(url1, header_list, post_data);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while (reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();
    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
  {
    return false;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);  

  int n = statusCodeV.toInt();

  if (n != 200)
  {
    return false;
  }

  QString str = reply->readAll();

  reply->deleteLater();

  if (str.toInt() == 0)
  {
    return false;
  }

  // ��ȡ��֤��
   QUrl url2("http://icode.renren.com/getcode.do?t=openplatform");
   HttpParamList header_list2;
   header_list2.push_back(HttpParamItem("Connection","Keep-Alive"));
   header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
   header_list2.push_back(HttpParamItem("Accept-Language","zh-CN"));
   header_list2.push_back(HttpParamItem("Host", "icode.renren.com"));
   header_list2.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
   header_list2.push_back(HttpParamItem("Referer",str_temp));
 
   QNetworkReply* reply2 = network.GetRequest(url2, header_list2);
 
   _t.restart();
 
   _timeout = false;
 
   while (reply2 && !reply2->isFinished())
   {
     QCoreApplication::processEvents();
     if (_t.elapsed() >= TIMEOUT) {
       _timeout = true;
       break;
     }
   }
 
   if (reply2 == NULL || (reply2->error() != QNetworkReply::NoError) || _timeout)
   {
     return false;
   }
 
   if (n != 200)
   {
     return false;
   }
 
   QByteArray data = reply2->readAll();
 
   QImage image_ = QImage::fromData(data);//(data.data(),100,40,QImage::Format_RGB32);

   if (m_code_online)
   {
     // ������֤
     //bool result = image_.save("e:\\1.jpg");
     VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
     int res = obj->GetRecResults(data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
   }
   else
   {
     // �ֶ���֤
     ValidateDlg dlg(this, image_);
     if (dlg.exec() == QDialog::Accepted)
     {
       vcode = dlg.getCode();
     }
   }

  return true;
}

int autobots_toutiao::GetRandom()
{
  // [m n]֮��������
  int m = 1, n = 8;
  return rand()%(n-m+1)+m;
}

void autobots_toutiao::onComputeNumber()
{
  int n = ui.spinBox_min_count->value();
  
  int count = ui.treeWidget_comment_id->topLevelItemCount();

  for (int i = count-1 ; i >= 0; --i)
  {
    QString n_str  = QString::number(n);
    ui.treeWidget_comment_id->topLevelItem(i)->setText(2,n_str);
    n = n + GetRandom();
  }
}

// 0 ������-1 δ֪���� -2 ��֤�����
int autobots_toutiao::ProcessRedirectLoginGetTemp(const QString& str)
{
  WaitforSeconds(2);

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","api.snssdk.com"));
  header_list.push_back(HttpParamItem("Referer","http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
  //header_list.push_back(HttpParamItem("",""));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

  QNetworkReply* reply = network.GetRequest(QUrl(str), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || _timeout)
  {
   
    reply->deleteLater();
    return -1;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  int n = statusCodeV.toInt();

  if ((reply->error() != QNetworkReply::NoError) && n != 500)
  {
     QString t = reply->errorString();
     return -1;
  }
  
  int res = -1;
  if (n == 302 || n == 301)
  {
    // �ض���
    QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

    QUrl red_url = redirectionTarget.toUrl();

    QString str = red_url.toString();

    res = ProcessRedirectLoginGet2(str) ? 0 : -1;
  }
  else if (n == 302)
  {
    QString str = reply->readAll();
    if (str.contains(QStringLiteral("��֤�����")))
    {
      res = -2;
    }
  }
  else if (n == 500)
  {
    // �Ҳ���ҳ��
    res = ProcessRedirectLoginGetTemp2(str);
  }

  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

int autobots_toutiao::ProcessRedirectLoginGetTemp2(const QString& str)
{
  //http://api.snssdk.com/auth/login_success/?code=vM988uwAJr91qLN7Yh4tKCxw4alqNDDz&state=renren_sns__0____toutiao____2__0__24
  int pos1 = str.indexOf("code=");
  int pos2 = str.indexOf("&",pos1);
  QString t = str.mid(pos1, pos2-pos1);
  QString s_url = "http://toutiao.com/auth/connected/?state=renren_sns__0____toutiao____2__0__24&" + t;

  HttpParamList header_list;
  header_list.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list.push_back(HttpParamItem("Host","toutiao.com"));
  //header_list.push_back(HttpParamItem("Referer","http://graph.renren.com/oauth/grant?client_id=394e2173327e4ead8302dc27f4ae8879&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&response_type=code&display=page&scope=status_update+photo_upload+create_album&state=renren_sns__0____toutiao____2__0__24&secure=true&origin=00000"));
  //header_list.push_back(HttpParamItem("",""));
  header_list.push_back(HttpParamItem("Connection","keep-alive"));
  //header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
  header_list.push_back(HttpParamItem("Accept-Language","zh-CN,zh;q=0.8"));
  header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (Windows NT 6.1; WOW64; Trident/7.0; rv:11.0) like Gecko"));

  QNetworkReply* reply = network.GetRequest(QUrl(s_url), header_list);

  QTime _t;
  _t.start();

  bool _timeout = false;

  while(reply && !reply->isFinished())
  {
    QCoreApplication::processEvents();

    if (_t.elapsed() >= TIMEOUT) {
      _timeout = true;
      break;
    }
  }

  if (reply == NULL || _timeout )
  {
   
    reply->deleteLater();
    return -1;
  }

  QVariant statusCodeV =  reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
  int n = statusCodeV.toInt();

  if ((reply->error() != QNetworkReply::NoError) && n != 500)
  {
     QString temp = reply->errorString();
     return -1;
  }
  
  int res = -1;

  if (n != 200)
  {
    return -1;
  }

  QByteArray data = reply->readAll();

  QString temp_s = data;
  if (temp_s.contains("window.opener.user.connected") 
    || temp_s.contains("window.parent.user.connected") )
  {
    return 0;
  }


  if (reply != NULL)
  {
    reply->deleteLater();
  }

  return res;
}

void autobots_toutiao::WaitforSeconds(int nseconds)
{
  QTime t2;
  t2.restart();
  while(t2.elapsed() < nseconds*1000 )  
    QCoreApplication::processEvents();
}


void autobots_toutiao::onCountAll()
{
  int count = ui.treeWidget_comment_id->topLevelItemCount();
  int all = 0;
  for (int i = count-1 ; i >= 0; --i)
  {
    //QString n_str  = QString::number(n);
    QString str = ui.treeWidget_comment_id->topLevelItem(i)->text(1);

    if (str.isEmpty())
    {
      continue;
    }

    int n = str.toInt();

    if ( n < 0 || n > 99999)
    {
      //EmitMsgStatusBar(QStringLiteral("�����д���"));
      continue;
    }
    else
    {
      all += n;
    }  
  }

  ui.lineEdit_countall->setText(QString::number(all));

}

bool autobots_toutiao::GetDongtaiIDMap()
{
	m_id_dongtaiid.clear();

	bool has_more = true;
	int offset = 0;
	while (has_more)
	{
		has_more = false;
		GetIDList(has_more, offset);
		offset += 500;
	}

	return m_id_dongtaiid.isEmpty() ? false : true;
}

bool autobots_toutiao::ExactComments(const QByteArray& rp_data, bool& has_more)
{
	bool res = false;
	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(rp_data, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();

			// ����data�ڵ�
			if (obj.contains("data"))
			{
				QJsonObject data_obj = obj.take("data").toObject();

				// ����comments�ڵ�
				if (data_obj.contains("comments"))
				{
					QJsonArray comments_arry = data_obj.take("comments").toArray();
					for (int i = 1; i < comments_arry.size(); ++i)
					{
						QJsonObject comment_obj = comments_arry.at(i).toObject();
						if (comment_obj.contains("id") && comment_obj.contains("dongtai_id"))
						{
							double d1 = comment_obj.take("id").toDouble();
							QString id = QString::number(d1, 'f', 0);
							QString d_id = QString::number(comment_obj.take("dongtai_id").toDouble(), 'f', 0);
							m_id_dongtaiid.insert(id, d_id);
						}
					}

				}

				if (data_obj.contains("has_more"))
				{
					has_more = data_obj.take("has_more").toBool();
				}				
			}
		}
	}

	return res;
}

void autobots_toutiao::GetIDList(bool& has_more, int offset)
{
	//http://www.toutiao.com/api/comment/list/?group_id=6333409505736392961&item_id=6333409505912553985&offset=5&count=10
	QString str_url = QString("http://www.toutiao.com/api/comment/list/?group_id=%1&item_id=%2&offset=%3&count=500").arg(
		m_group_id, m_item_id, QString::number(offset));

	QNetworkAccessManager manager;
	QNetworkRequest req;
	req.setUrl(str_url);
	req.setRawHeader("Cache-Control", "no-cache");
	req.setRawHeader("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8");
	req.setRawHeader("Connection", "Keep-Alive");
	req.setRawHeader("Referer", m_url.toUtf8().data());
	req.setRawHeader("Accept-Language", "zh-CN");
	req.setRawHeader("Host", "www.toutiao.com");
	req.setRawHeader("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)");

	QNetworkReply* reply = manager.get(req);

	while (!reply->isFinished())
	{
		QCoreApplication::processEvents();
	}

	QString msg;
	if (reply->error() != QNetworkReply::NoError)
	{
		msg = reply->errorString();
		ui.lineEdit_msg->setText(msg);
		reply->deleteLater();
		return;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		ui.lineEdit_msg->setText(QStringLiteral("��ȡ��̬IDʧ��"));
		reply->deleteLater();
		return;
	}

	QByteArray rp_data = reply->readAll();

	bool res = ExactComments(rp_data, has_more);

	reply->deleteLater();

	//return res;
}
