#include "autobots_toutiao.h"
#include "html_parse_utils.h"
#include <QVariant>
#include <QFileDialog>
#include <QMessageBox>
#include "code_dlg.h"
#include "vlidatecodeonline.h"
#include "sina_encrypt.h"


const int TIMEOUT = 20*1000;
const int TIMEOUT1 = 1*1000;

autobots_toutiao::autobots_toutiao(QWidget *parent)
    : control_status(true),QMainWindow(parent),
    m_client_id("394e2173327e4ead8302dc27f4ae8879"),network(NULL)
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
  // 检查输入
  if (!CheckInput())
  {
    return;
  }

  // 更新界面输入
  UpdateData();

  bool res = GetDongtaiIDMap();
  ui.lineEdit_msg->setText(QStringLiteral("获取动态ID"));
  
  control_status = true;

  for (int i = 0; i < m_account_list.size(); ++i)
  {
    if (!control_status || m_comment_list.isEmpty())
    {
      break;
    }
	if (network != NULL)
	{
		delete network;
		network = NULL;
	}

	network = new toutiao_network;
//	network.GetManager().clearAccessCache();
//	network.GetManager().cookieJar()->deleteLater();

//    QNetworkCookieJar* cookie = new QNetworkCookieJar();

//    network.GetManager().setCookieJar(cookie);

    // 获取CSRF TOKEN
    GetContent();
    if (m_csrf_token.isEmpty())
    {
		m_csrf_token = "undefined";
      //GetContent();
      //ui.lineEdit_msg->setText(QStringLiteral("连接网站中..."));
      //if (m_csrf_token.isEmpty())
      //{
      //  continue;
      //}
    }

	if (!RequestForSina())
	{
		ui.lineEdit_msg->setText(QStringLiteral("请求失败..."));
		continue;
	}

	AccountParam ac = m_account_list.at(i);

	if (!AuthorBySina(ac._id, ac._password))
	{
		ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
		ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(255, 0, 0, 180));
		continue;
	}
	else
	{
		ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
		ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(0, 255, 0, 180));
	}

    //if (!AuthorByRenren(ac._id, ac._password))
    //{
    //  ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
    //  ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(255,0,0, 180));
    //  continue;
    //}

    //ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
    //ui.tableWidget_account_id->item(i, 0)->setBackgroundColor(QColor(0,255,0, 180));

    bool do_up = DoAction();

    QElapsedTimer t2;
    t2.start();
    while(t2.elapsed()<500 )  
      QCoreApplication::processEvents();

//     if (m_current_count >= m_base_count)
//     {
//       // 已经达到基本数,每隔一个随机数再减掉一个
//       // 随机产生一个基本数
//       m_comment_list.pop_back();
//       //m_comment_item_list.pop_back();
//       int n = GetRandom();
//       m_base_count += n;
//     }

    t2.restart();
    while(t2.elapsed()<1000 )  
      QCoreApplication::processEvents();

    //cookie->deleteLater();
  }

  ui.lineEdit_msg->setText(QStringLiteral("已完成"));
}


void autobots_toutiao::onPause()
{
  control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
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
	QString str_d_id;
	if (m_id_dongtaiid.find(str_id) != m_id_dongtaiid.end())
	{
		str_d_id = m_id_dongtaiid[str_id];
	}
	else
	{
		continue;
	}

    HttpParamList header_list;
    header_list.push_back(HttpParamItem("Accept",	"text/javascript, text/html, application/xml, text/xml, */*"));
    header_list.push_back(HttpParamItem("Connection","Keep-Alive"));
    header_list.push_back(HttpParamItem("Accept-Encoding","deflate"));
    header_list.push_back(HttpParamItem("Accept-Language","zh-cn"));
    header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
    header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
    header_list.push_back(HttpParamItem("X-CSRFToken", m_csrf_token));
    header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
    header_list.push_back(HttpParamItem("Referer", m_url));
    header_list.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
	header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));

    HttpParamList post_data;
    post_data.push_back(HttpParamItem("action", "digg"));
    post_data.push_back(HttpParamItem("comment_id", str_id));
	post_data.push_back(HttpParamItem("dongtai_id", str_d_id));
    post_data.push_back(HttpParamItem("group_id", m_group_id));
	post_data.push_back(HttpParamItem("item_id", m_item_id));

    QNetworkReply* reply = network->PostRequest(url1, header_list, post_data);

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
  header_list1.push_back(HttpParamItem("Accept","text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
  header_list1.push_back(HttpParamItem("Connection","Keep-Alive"));
  header_list1.push_back(HttpParamItem("Accept-Encoding","deflate"));
  header_list1.push_back(HttpParamItem("Accept-Language","zh-cn"));
  header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

  //header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
  //header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
  header_list1.push_back(HttpParamItem("Host", "www.toutiao.com"));
  header_list1.push_back(HttpParamItem("Referer", "http://www.toutiao.com/"));
  header_list1.push_back(HttpParamItem("User-Agent","Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
 
  QNetworkReply* rp = network->GetRequest(url_1, header_list1);

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

  if (rp->error() != QNetworkReply::NoError)
  {
	  QString msg = rp->errorString();
	  ui.lineEdit_msg->setText(msg);
	  rp->deleteLater();
	  return false;
  }

  bool res = GetCsrfToken(rp->readAll());

  if (rp!=NULL)
    rp->deleteLater();

  return  res;

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
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL, "提示", "无法打开文件");  
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
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL, "提示", "无法打开文件");  
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
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_page_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
    return false;
  }

  if (ui.treeWidget_comment_id->topLevelItemCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
    return false;
  }

  if (ui.tableWidget_account_id->rowCount() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请导入账号")); 
    return false;
  }
  
  for (int i = 0; i < ui.treeWidget_comment_id->topLevelItemCount(); ++i)
  {
    // 第一行不空，第三行空，则未输入有效目标数
    if (!ui.treeWidget_comment_id->topLevelItem(i)->text(0).isEmpty() &&
      ui.treeWidget_comment_id->topLevelItem(i)->text(2).isEmpty())
    {
      QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("请输入目标数量，或自动生成目标数量")); 
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

  // "查找csrftoken"
  GumboNode*  node = html_parse.FirstElementNode("name", "csrfmiddlewaretoken");

  if (node != NULL)
  {
    std::string csrf_token = HtmlParseUtils::GetAttributeValue(node, "value");
    m_csrf_token = QString::fromStdString(csrf_token);
    return true;
  }

  return false;
}

int autobots_toutiao::GetRandom()
{
  // [m n]之间的随机数
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
      //EmitMsgStatusBar(QStringLiteral("数字有错误"));
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
		offset = m_id_dongtaiid.size();
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

			// 查找data节点
			if (obj.contains("data"))
			{
				QJsonObject data_obj = obj.take("data").toObject();

				// 查找comments节点
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
		ui.lineEdit_msg->setText(QStringLiteral("获取动态ID失败"));
		reply->deleteLater();
		return;
	}

	QByteArray rp_data = reply->readAll();

	bool res = ExactComments(rp_data, has_more);

	reply->deleteLater();

	//return res;
}
bool autobots_toutiao::RequestForSina()
{
	QString str_url1 = QString("https://www.toutiao.com/auth/connect/?type=sso&platform=sina_weibo&next=https://sso.toutiao.com/auth/login_success/?service=%1").arg(m_url);;

	QUrl url1(str_url1);

	HttpParamList header_list;
	//header_list.push_back(HttpParamItem("(Request-Line)",	"GET /auth/connect/?type=toutiao&platform=renren_sns HTTP/1.1"));
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
	header_list.push_back(HttpParamItem("Referer", "https://sso.toutiao.com/login/"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(url1, header_list);

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
		QString msg = reply->errorString();
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectSSL(str);
	}
	else
	{
		return false;
	}

}
bool autobots_toutiao::AuthorBySina(const QString& name, const QString& password)
{
	//1.检验验证码
	QString vcode, code_sign;
	SinaData sina_data;
	if (!PreLoginSina(name, sina_data, vcode, code_sign))
	{
		ui.lineEdit_msg->setText(QStringLiteral("获取验证码失败"));
		return false;
	}

	if (sina_data._showpin && vcode.isEmpty())
	{
		return false;
	}

	// 2.登陆
	if (!LoginSina(sina_data, name, password, vcode))
	{
		ui.lineEdit_msg->setText(QStringLiteral("登录失败"));
		return false;
	}

	// 3 授权
	if (!AuthorizeSina(sina_data))
	{
		// 再次尝试
		if (!AuthorizeSina(sina_data))
		{
			ui.lineEdit_msg->setText(QStringLiteral("授权失败"));
			return false;
		}
	}
	return true;
}

void autobots_toutiao::Logout()
{
	QString str_url_1 = "https://sso.toutiao.com/logout/";

	QUrl url_1(str_url_1);

	HttpParamList header_list1;
	header_list1.push_back(HttpParamItem("Accept", "application/json, text/javascript, */*; q=0.01"));
	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list1.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

	//header_list1.push_back(HttpParamItem("Cache-Control", "no-cache"));
	////header_list.push_back(HttpParamItem("X-CSRFToken", "20c9e1fc22618a31cbfcd42218e96dd0"));
	header_list1.push_back(HttpParamItem("Host", "sso.toutiao.com"));
	header_list1.push_back(HttpParamItem("Referer", m_url));
	header_list1.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply_1 = network->GetRequest_ssl(url_1, header_list1);

	QTime _t;
	_t.start();

	while (reply_1 && !reply_1->isFinished())
	{
		QCoreApplication::processEvents();

		if (_t.elapsed() >= 2000) {
			break;
		}
	}
}


bool autobots_toutiao::ProcessRedirectSSL(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	if (!str.contains("https://", Qt::CaseInsensitive))
	{
		return false;
	}

	QNetworkReply* reply = network->GetRequest_ssl(QUrl(str), header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply->readAll();
	QString t = data;

	return GetPostId(data);

	//   if (n == 302 || n == 301)
	//   {
	//     // 重定向
	//     QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	// 
	//     QUrl red_url = redirectionTarget.toUrl();
	// 
	//     QString str = red_url.toString();
	// 
	//     return ProcessRedirectGet(str);
	//   }
	//   else
	//   {
	//     return false;
	//   }
}

bool autobots_toutiao::ProcessRedirectGet(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply->readAll();

	return GetPostId(data);

}

bool autobots_toutiao::GetPostId(const QByteArray& arr)
{
	// name="client_id" value="2504490989"
	// name="appkey62" value="42iQjj"
	// name="state" value="f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D"

	HtmlParseUtils html_parse(arr.data(), arr.length());

	// "查找appkey62"
	GumboNode* node = html_parse.FirstElementNode("name", "appkey62");
	QString temp1;
	if (node != NULL)
	{
		std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString str = QString::fromStdString(post_from_id);

		if (!str.isEmpty())
		{
			m_appkey = str;
		}
	}

	// "查找client_id"
	node = html_parse.FirstElementNode("name", "client_id");
	if (node != NULL)
	{
		std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString temp = QString::fromStdString(client_id);

		if (!temp.isEmpty())
		{
			m_client_id = temp;
		}
	}

	node = html_parse.FirstElementNode("name", "state");
	if (node != NULL)
	{
		std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString temp = QString::fromStdString(client_id);

		if (!temp.isEmpty())
		{
			m_state = temp;
		}
	}

	return true;
}

bool autobots_toutiao::GetUserId(const QByteArray& arr)
{
	// name="client_id" value="2504490989"
	// name="appkey62" value="42iQjj"
	// name="state" value="f4b2dd6diaFhGKFwqnNpbmFfd2VpYm-hc6ChcgKhdgKhaQChaKt0b3V0aWFvLmNvbaFtAKFuwA%3D%3D"

	HtmlParseUtils html_parse(arr.data(), arr.length());

	// "查找uid"
	GumboNode* node = html_parse.FirstElementNode("name", "uid");
	QString temp1;
	if (node != NULL)
	{
		std::string post_from_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString str = QString::fromStdString(post_from_id);

		if (!str.isEmpty())
		{
			m_uid = str;
		}
	}

	// "查找verifyToken"
	node = html_parse.FirstElementNode("name", "verifyToken");
	if (node != NULL)
	{
		std::string client_id = HtmlParseUtils::GetAttributeValue(node, "value");

		QString temp = QString::fromStdString(client_id);

		if (!temp.isEmpty())
		{
			m_verifyToken = temp;
		}
	}

	return true;
}

bool autobots_toutiao::ProcessRedirectLoginGet(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Host", "api.snssdk.com"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

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
		QString s = reply->errorString();
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str2 = red_url.toString();

		return ProcessRedirectLoginGet2(str2);
	}
	else
	{
		return false;
	}
}

bool autobots_toutiao::ProcessRedirectLoginGet2(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Host", "sso.toutiao.com"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str2 = red_url.toString();

		return ProcessRedirectLoginGet3(str2);
	}
	else
	{
		return false;
	}
}


bool autobots_toutiao::ProcessRedirectLoginGet3(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();


	if (n == 302)
	{
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str2 = red_url.toString();

		return ProcessRedirectLoginGet4(str2);
	}

	QByteArray data = reply->readAll();


	return (n == 200) ? true : false;
}

bool autobots_toutiao::ProcessRedirectLoginGet4(const QString& str)
{
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Host", "www.toutiao.com"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network->GetRequest(QUrl(str), header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray data = reply->readAll();


	return true;
}


bool autobots_toutiao::PreLoginSina(const QString& name, SinaData& data,
	QString& vcode, QString& code_sign)
{
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QString str_time = QString::number(time);

	// 获取用户名
	sina_encrypt* encryptor = sina_encrypt::GetInstance();
	QString encrypted_name = encryptor->EncryptName(name);

	QString str_url1 = QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.18)&_=%2").arg(encrypted_name, str_time);
	QUrl url1(str_url1);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://service.zol.com.cn/user/api/sina/callback.php&response_type=code").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
	header_list.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply = network->GetRequest_ssl(url1, header_list);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray str = reply->readAll();

	bool res = GetPreLoginResult(str, data);

	reply->deleteLater();

	if (!res)
	{
		return false;
	}

	if (!data._showpin) // 不需要验证码
	{
		return true;
	}

	int n_rand = rand() % 9;
	double d = 0.987654321235647 / n_rand;
	d = d * 100000000;

	QString str_r = QString::number(d, 'f', 0);

	// 获取验证码
	//https://login.sina.com.cn/cgi/pin.php?r=54763124&s=0&p=gz-db24c75a1d5b6a51bea82087efb1a2ce12a5
	QString str_url2 = QString("https://login.sina.com.cn/cgi/pin.php?r=%1&s=0&p=%2").arg(str_r, data._pcid);
	QUrl url2(str_url2);
	HttpParamList header_list2;
	header_list2.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//  header_list2.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list2.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list2.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list2.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
	header_list2.push_back(HttpParamItem("Referer", str_temp));

	QNetworkReply* reply2 = network->GetRequest_ssl(url2, header_list2);

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

	QByteArray rp_data = reply2->readAll();

	//   QVariant var = reply2->header(QNetworkRequest::ContentTypeHeader);
	// 
	//   QString type = var.toString();

	QImage image_ = QImage::fromData(rp_data);
	if (m_code_online)
	{
		// 在线验证
		//bool result = image_.save("e:\\1.jpg");
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		int res = obj->GetRecResults(rp_data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);
		//obj->ReportError("bestsalt", code_sign); 
	}
	else
	{
		// 手动验证
		ValidateDlg dlg(this, image_);
		if (dlg.exec() == QDialog::Accepted)
		{
			vcode = dlg.getCode();
		}
	}

	return true;
}


bool autobots_toutiao::GetPreLoginResult(const QByteArray& str, SinaData& data)
{
	//sinaSSOController.preloginCallBack({"retcode":0,"servertime":1451282884,
	//"pcid":"gz-db24c75a1d5b6a51bea82087efb1a2ce12a5","nonce":"J2Z37I",
	//"pubkey":"EB2A38.....",
	//"rsakv":"1330428213","is_openlock":0,"showpin":1,"exectime":10})
	QString temp = str;

	int first = temp.indexOf("(");
	QString temp2 = temp.right(temp.length() - first - 1);
	int last = temp2.lastIndexOf(")");
	temp2 = temp2.left(last);

	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(temp2.toLocal8Bit(), &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("retcode"))
			{
				QJsonValue name_value = obj.take("retcode");
				int d = name_value.toDouble();
				if (d != 0)
				{
					return false;
				}
			}
			if (obj.contains("servertime"))
			{
				QJsonValue name_value = obj.take("servertime");
				double d = name_value.toDouble();
				data._servertime = QString::number(d, 'f', 0);

				if (data._servertime.isEmpty())
				{
					return false;
				}

			}
			if (obj.contains("nonce"))
			{
				QJsonValue name_value = obj.take("nonce");
				data._nonce = name_value.toString();
				if (data._nonce.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("pubkey"))
			{
				QJsonValue name_value = obj.take("pubkey");
				data._pubkey = name_value.toString();
				if (data._pubkey.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("rsakv"))
			{
				QJsonValue name_value = obj.take("rsakv");
				data._rsakv = name_value.toString();
				if (data._rsakv.isEmpty())
				{
					return false;
				}
			}
			if (obj.contains("pcid"))
			{
				QJsonValue name_value = obj.take("pcid");
				data._pcid = name_value.toString();
			}
			if (obj.contains("showpin"))
			{
				QJsonValue name_value = obj.take("showpin");
				int i = name_value.toDouble();
				data._showpin = (i == 0) ? false : true;
			}

			return true;
		}
	}

	return false;
}

bool autobots_toutiao::LoginSina(SinaData& sina_data, const
	QString& name, const QString& password, const QString& vcode)
{
	qint64 time = QDateTime::currentMSecsSinceEpoch();
	QString str_time = QString::number(time);

	// 获取用户名
	sina_encrypt* encryptor = sina_encrypt::GetInstance();
	QString encrypted_name = encryptor->EncryptName(name);
	QString encrypted_pwd = encryptor->EncryptPassword(password, sina_data._servertime, sina_data._nonce, sina_data._pubkey);

	QString str_login_url; //= QString("https://login.sina.com.cn/sso/prelogin.php?entry=openapi&callback=sinaSSOController.preloginCallBack&su=%1&rsakt=mod&checkpin=1&client=ssologin.js(v1.4.15)&_=%2").arg(encrypted_name, str_time);

	if (sina_data._showpin)
	{
		// 需要验证码
		str_login_url = QString("https://login.sina.com.cn/sso/login.php?client=ssologin.js(v1.4.18)&_=%1&openapilogin=qrcode").arg(str_time);
		//str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&pcid=%1&ct=1800&s=1&vsnf=1&vsnval=&door=%2&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
		//  sina_data._pcid, vcode, m_appkey, encrypted_name, sina_data._servertime,sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
	}
	else
	{
		// 无需验证码
		str_login_url = QString("https://login.sina.com.cn/sso/login.php?entry=openapi&gateway=1&from=&savestate=0&useticket=1&pagerefer=&ct=1800&s=1&vsnf=1&vsnval=&door=&appkey=%3&su=%4&service=miniblog&servertime=%5&nonce=%6&pwencode=rsa2&rsakv=%7&sp=%8&sr=1192*670&encoding=UTF-8&cdult=2&domain=weibo.com&prelt=140&returntype=TEXT&callback=sinaSSOController.loginCallBack&client=ssologin.js(v1.4.15)&_=%9").arg(
			m_appkey, encrypted_name, sina_data._servertime, sina_data._nonce, sina_data._rsakv, encrypted_pwd, str_time);
	}

	QUrl url1(str_login_url);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "login.sina.com.cn"));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));
	header_list.push_back(HttpParamItem("Referer", str_temp));

	HttpParamList post_data;
	//post_data.push_back(HttpParamItem("action", "login"));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("cdult", "2"));
	post_data.push_back(HttpParamItem("ct", "1800"));
	post_data.push_back(HttpParamItem("domain", "weibo.com"));
	post_data.push_back(HttpParamItem("door", vcode));
	post_data.push_back(HttpParamItem("encoding", "UTF-8"));
	post_data.push_back(HttpParamItem("entry", "openapi"));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("gateway", "1"));
	post_data.push_back(HttpParamItem("nonce", sina_data._nonce));
	post_data.push_back(HttpParamItem("pagerefer", "https://sso.toutiao.com/login/"));
	post_data.push_back(HttpParamItem("pcid", sina_data._pcid));
	post_data.push_back(HttpParamItem("prelt", "1235"));
	post_data.push_back(HttpParamItem("pwencode", "rsa2"));
	post_data.push_back(HttpParamItem("returntype", "TEXT"));
	post_data.push_back(HttpParamItem("rsakv", sina_data._rsakv));
	post_data.push_back(HttpParamItem("s", "1"));
	post_data.push_back(HttpParamItem("savestate", "0"));
	post_data.push_back(HttpParamItem("servertime", sina_data._servertime));
	post_data.push_back(HttpParamItem("service", "miniblog"));
	post_data.push_back(HttpParamItem("sp", encrypted_pwd));
	post_data.push_back(HttpParamItem("sr", "1366*768"));
	post_data.push_back(HttpParamItem("su", encrypted_name));
	post_data.push_back(HttpParamItem("useticket", "1"));
	post_data.push_back(HttpParamItem("vsnf", "1"));
	post_data.push_back(HttpParamItem("vsnval", ""));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n != 200)
	{
		return false;
	}

	QByteArray rp_data = reply->readAll();

	bool res = GetLoginResult(rp_data, sina_data);

	reply->deleteLater();

	return res;
}

bool autobots_toutiao::GetLoginResult(const QByteArray& str, SinaData& data)
{


	QJsonParseError json_error;
	QJsonDocument parse_doucment = QJsonDocument::fromJson(str, &json_error);
	if (json_error.error == QJsonParseError::NoError)
	{
		if (parse_doucment.isObject())
		{
			QJsonObject obj = parse_doucment.object();
			if (obj.contains("retcode"))
			{
				QJsonValue name_value = obj.take("retcode");
				int d = name_value.toDouble();
				if (d != 0)
				{
					return false;
				}
			}
			if (obj.contains("ticket"))
			{
				QJsonValue name_value = obj.take("ticket");
				data._ticket = name_value.toString();
				if (data._ticket.isEmpty())
				{
					return false;
				}
			}
			return true;
		}
	}

	return false;
}

bool autobots_toutiao::AuthorizeSina(const SinaData& sina_data)
{
	QString str_url_author = "https://api.weibo.com/oauth2/authorize";

	QUrl url1(str_url_author);

	QString str_temp = QString("https://api.weibo.com/2/oauth2/authorize?client_id=%1&response_type=code&display=default&redirect_uri=http%3A%2F%2Fapi.snssdk.com%2Fauth%2Flogin_success%2F&from=&with_cookie=").arg(m_client_id);
	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
	header_list.push_back(HttpParamItem("Referer", str_temp));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("action", "login"));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("client_id", m_client_id));
	post_data.push_back(HttpParamItem("display", "default"));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("isLoginSina", ""));
	post_data.push_back(HttpParamItem("passwd", ""));
	post_data.push_back(HttpParamItem("quick_auth", "null"));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("regCallback", str_temp));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("scope", ""));
	post_data.push_back(HttpParamItem("state", m_state));
	//post_data.push_back(HttpParamItem("switchLogin","0"));
	post_data.push_back(HttpParamItem("ticket", sina_data._ticket));
	post_data.push_back(HttpParamItem("userId", ""));
	post_data.push_back(HttpParamItem("url", "https://api.weibo.com/oauth2/authorize"));
	post_data.push_back(HttpParamItem("verifyToken", "null"));
	post_data.push_back(HttpParamItem("visible", "0"));
	post_data.push_back(HttpParamItem("withOfficalAccount", "null"));
	post_data.push_back(HttpParamItem("withOfficalFlag", "0"));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectLoginGet(str);
	}
	else if (n == 200)
	{
		QByteArray data = reply->readAll();
		QString t = data;
		GetUserId(data);
		return AuthorizeSina2(sina_data);
	}
	else
	{
		QByteArray data = reply->readAll();
		QString t = data;
		return false;
	}
}

bool autobots_toutiao::AuthorizeSina2(const SinaData& sina_data)
{
	QString str_url_author = "https://api.weibo.com/oauth2/authorize";

	QUrl url1(str_url_author);

	QString str_temp = QString("https://api.weibo.com/oauth2/authorize?client_id=%1&response_type=code&display=desktop&state=%2&redirect_uri=http://api.snssdk.com/auth/login_success/").arg(m_client_id, m_state);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding","gzip, deflate"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-CN"));
	header_list.push_back(HttpParamItem("Host", "api.weibo.com"));
	header_list.push_back(HttpParamItem("Referer", str_temp));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("action", "authorize"));
	post_data.push_back(HttpParamItem("appkey62", m_appkey));
	post_data.push_back(HttpParamItem("client_id", m_client_id));
	post_data.push_back(HttpParamItem("display", "default"));
	post_data.push_back(HttpParamItem("from", ""));
	post_data.push_back(HttpParamItem("isLoginSina", ""));
	post_data.push_back(HttpParamItem("passwd", ""));
	post_data.push_back(HttpParamItem("quick_auth", "null"));
	post_data.push_back(HttpParamItem("redirect_uri", "http://api.snssdk.com/auth/login_success/"));
	post_data.push_back(HttpParamItem("regCallback", str_temp));
	post_data.push_back(HttpParamItem("response_type", "code"));
	post_data.push_back(HttpParamItem("scope", ""));
	post_data.push_back(HttpParamItem("state", m_state));
	//post_data.push_back(HttpParamItem("switchLogin","0"));
	post_data.push_back(HttpParamItem("ticket", sina_data._ticket));
	post_data.push_back(HttpParamItem("uid", m_uid));
	post_data.push_back(HttpParamItem("url", "https://api.weibo.com/oauth2/authorize"));
	post_data.push_back(HttpParamItem("verifyToken", m_verifyToken));
	post_data.push_back(HttpParamItem("visible", "0"));
	post_data.push_back(HttpParamItem("withOfficalAccount", "null"));
	post_data.push_back(HttpParamItem("withOfficalFlag", "0"));

	QNetworkReply* reply = network->PostRequest_ssl(url1, header_list, post_data);

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

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 302 || n == 301)
	{
		// 重定向
		QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);

		QUrl red_url = redirectionTarget.toUrl();

		QString str = red_url.toString();

		return ProcessRedirectLoginGet(str);
	}
	else
	{
		QByteArray data = reply->readAll();
		QString t = data;
		return false;
	}
}
