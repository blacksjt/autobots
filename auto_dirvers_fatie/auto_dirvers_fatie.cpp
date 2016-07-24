#include "auto_dirvers_fatie.h"
#include <QGroupBox>
#include <QListWidget>
#include "QMessageBox"
#include "vlidatecodeonline.h"
#include "dailconnector.h"
#include "QNetworkCookieJar"
#include "QFileDialog"

const int TIMEOUT = 20 * 1000;

auto_dirvers_fatie::auto_dirvers_fatie(QWidget *parent)
	: QMainWindow(parent),m_control_stop(false)
{
	ui.setupUi(this);
	initializeControls(); // 初始化控件
	initializeConncetions(); // 初始化信号槽
}

auto_dirvers_fatie::~auto_dirvers_fatie()
{
	
}

void auto_dirvers_fatie::initializeConncetions()
{
	connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

	connect(ui.pushButton_stop, SIGNAL(clicked()), this, SLOT(onStop()));

	connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

	connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));
}

void auto_dirvers_fatie::onStart()
{
	ui.pushButton_start->setEnabled(false);

	// 1. 获取界面数据
	if (!updateData())
	{
		//emitMsgBar();
		ui.pushButton_start->setEnabled(true);
		return;
	}
	// 2. 开始工作
	AutoFatie();

	ui.pushButton_start->setEnabled(true);

}

void auto_dirvers_fatie::onStop()
{
	emitMsgBar(QStringLiteral("停止中.."));
	m_control_stop = true;
}

void auto_dirvers_fatie::emitMsgBar(const QString& msg)
{
	ui.statusBar->showMessage(msg);
}

bool auto_dirvers_fatie::updateData()
{
	//1. 检查空间输入
	if (ui.lineEdit_url1->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入"));
		return false;
	}

	if (ui.lineEdit_host1->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("主机host没有输入"));
		return false;
	}

	if (ui.lineEdit_news_id1->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入"));
		return false;
	}

	if (ui.listWidget_comment_id1->count() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID"));
		return false;
	}

	//2. 获取控件值，存入
// 	DriverWork work;
// 	work.setUrl(ui.lineEdit_url1->text());
// 	work.setHost(ui.lineEdit_host1->text());
// 	work.setNewsid(ui.lineEdit_news_id1->text());
	m_comments.clear();
	m_comment_item_list.clear();
 	m_url = ui.lineEdit_url1->text();
 	m_host = ui.lineEdit_host1->text();
 	m_newsid = ui.lineEdit_news_id1->text();
	m_interval = ui.spinBox_interval->value();
	int n = ui.listWidget_comment_id1->count();
	
	for (int i = 0; i < n; ++i)
	{
		QListWidgetItem* item = ui.listWidget_comment_id1->item(i);

		if (item->checkState() != Qt::Checked)
		{
			QString text = item->text();
			m_comments.push_back(text);
			m_comment_item_list.push_back(item);
		}
	}

	//work.setComments(comments);

	return true;
}

void auto_dirvers_fatie::initializeControls()
{
	//addTabPage(); // 添加一个tab页面
}

void auto_dirvers_fatie::addTabPage()
{
	int n = ui.tabWidget_comments->count() + 1;
	QString tabname = QStringLiteral("新闻")+ QString::number(n);

	QWidget *tab = new QWidget(this);
	ui.tabWidget_comments->addTab(tab, tabname);

	QGridLayout* gridLayout_3 = new QGridLayout(tab);
	gridLayout_3->setSpacing(6);
	gridLayout_3->setContentsMargins(11, 11, 11, 11);
	gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));

	QLabel* label_3 = new QLabel(tab);
	label_3->setObjectName(QStringLiteral("label_3"));

	QHBoxLayout* horizontalLayout_5 = new QHBoxLayout();
	horizontalLayout_5->setSpacing(6);
	horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
	QHBoxLayout* horizontalLayout = new QHBoxLayout();
	horizontalLayout->setSpacing(6);
	horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
	label_3 = new QLabel(tab);
	label_3->setObjectName(QStringLiteral("label_3"));
	label_3->setText(QStringLiteral("网页URL"));

	horizontalLayout->addWidget(label_3);

	QLineEdit* lineEdit_url1 = new QLineEdit(tab);
	lineEdit_url1->setObjectName(QStringLiteral("lineEdit_url1"));
	lineEdit_url1->setEnabled(true);

	horizontalLayout->addWidget(lineEdit_url1);


	horizontalLayout_5->addLayout(horizontalLayout);


	gridLayout_3->addLayout(horizontalLayout_5, 0, 0, 1, 1);

	QHBoxLayout* horizontalLayout_6 = new QHBoxLayout();
	horizontalLayout_6->setSpacing(6);
	horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
	QHBoxLayout* horizontalLayout_3 = new QHBoxLayout();
	horizontalLayout_3->setSpacing(6);
	horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
	QLabel* label_2 = new QLabel(tab);
	label_2->setObjectName(QStringLiteral("label_2"));
	label_2->setText(QStringLiteral("新闻ID"));

	horizontalLayout_3->addWidget(label_2);

	QLineEdit* lineEdit_newsid = new QLineEdit(tab);
	lineEdit_newsid->setObjectName(QStringLiteral("lineEdit_news_id1"));
	lineEdit_newsid->setEnabled(true);

	horizontalLayout_3->addWidget(lineEdit_newsid);


	horizontalLayout_6->addLayout(horizontalLayout_3);


	gridLayout_3->addLayout(horizontalLayout_6, 1, 0, 1, 1);

	QGroupBox* groupBox = new QGroupBox(tab);
	groupBox->setObjectName(QStringLiteral("groupBox"));
	groupBox->setTitle(QStringLiteral("评论内容"));
	QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
	sizePolicy.setHorizontalStretch(6);
	sizePolicy.setVerticalStretch(0);
	sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
	groupBox->setSizePolicy(sizePolicy);
	QGridLayout* gridLayout_2 = new QGridLayout(groupBox);
	gridLayout_2->setSpacing(6);
	gridLayout_2->setContentsMargins(11, 11, 11, 11);
	gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
	QListWidget* listWidget_comment_id1 = new QListWidget(groupBox);
	listWidget_comment_id1->setObjectName(QStringLiteral("listWidget_comment_id1"));
	QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
	sizePolicy1.setHorizontalStretch(2);
	sizePolicy1.setVerticalStretch(0);
	sizePolicy1.setHeightForWidth(listWidget_comment_id1->sizePolicy().hasHeightForWidth());
	listWidget_comment_id1->setSizePolicy(sizePolicy1);

	gridLayout_2->addWidget(listWidget_comment_id1, 0, 0, 1, 1);


	gridLayout_3->addWidget(groupBox, 2, 0, 1, 1);
	
	//ui.tabWidget_comments->count();
}


void auto_dirvers_fatie::AutoFatie()
{
	DailConnector connector("VPN", "ycc1", "111");

	for (int i = 0; i < m_comments.size(); ++i)
	{
		if (m_control_stop)
		{
			break;
		}

		QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

		network.GetManager().setCookieJar(cookie);

		emitMsgBar(QStringLiteral("拨号中..."));
		QString dial_msg;
		while (!connector.ReConnect(dial_msg))
		{
			emitMsgBar(QStringLiteral("拨号中失败：") + dial_msg);
			WaitforSeconds(3);
			emitMsgBar(QStringLiteral("拨号中..."));
		}

		emitMsgBar(QStringLiteral("拨号成功..."));

		int ncount = 0;
		while (!GetContent() && ncount < 15)
		{
			connector.ReConnect(dial_msg);
			WaitforSeconds(3);
			ncount++;
		}

		if (ncount >= 15)
		{
			//ui.lineEdit_msg->setText(QStringLiteral("网站连接失败...,请检查网络连接"));
			emitMsgBar(QStringLiteral("网站连接失败...,请检查网络连接"));
			continue;;
		}

		// 尝试登陆
// 		bool login_status = false;
// 		while (m_account_order < m_account_list.size())
// 		{
// 			AccountParam ac = m_account_list.at(m_account_order);
// 
// 			if (!LoginWY(ac._id, ac._password))
// 			{
// 				ui.lineEdit_msg->setText(QStringLiteral("登陆失败..."));
// 				//m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
// 				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255, 0, 0, 180));
// 				m_account_order++;
// 				continue;
// 			}
// 			else
// 			{
// 				ui.lineEdit_msg->setText(QStringLiteral("登陆成功"));
// 				m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
// 				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0, 255, 0, 180));
// 				m_account_order++;
// 				login_status = true;
// 				break;
// 			}
// 		}
// 
// 		if (!login_status)
// 		{
// 			ui.lineEdit_msg->setText(QStringLiteral("1、网络无连接 \n 2、号登陆不成功，无法继续"));
// 			return;
// 		}

		QString msg;
		msg.setNum(i + 1);

		QElapsedTimer t2;
		t2.start();
		while (t2.elapsed() < 1000)
			QCoreApplication::processEvents();

		if (doPostFatie(m_comments[i]))
		{
			m_comment_item_list[i]->setCheckState(Qt::Checked);
			emitMsgBar(QStringLiteral("第") + msg + QStringLiteral("条,已完成"));
		}
		else
		{
			emitMsgBar(QStringLiteral("第") + msg + QStringLiteral("条,失败"));
		}

		if (i != m_comments.size() - 1)
		{
			WaitforSeconds(m_interval);
		}

		cookie->deleteLater();
	}

	connector.DisConnect();
	emitMsgBar(QStringLiteral("已完成"));
}

void auto_dirvers_fatie::WaitforSeconds(int nseconds)
{
	QTime t2;
	t2.restart();
	while (t2.elapsed() < nseconds * 1000)
		QCoreApplication::processEvents();
}


bool auto_dirvers_fatie::GetContent()
{
	QString str_url_1 = m_url;

	QUrl url_1(str_url_1);
	url_1.setUrl(str_url_1);
	HttpParamList header_list1;
	header_list1.push_back(HttpParamItem("Accept", "text/html,application/xhtml+xml,application/xml;q=0.9,image/webp,*/*;q=0.8"));
	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
//	header_list1.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));

	header_list1.push_back(HttpParamItem("Cache-Control", "max-age=0"));
	header_list1.push_back(HttpParamItem("Host", m_host)); // mhost
	header_list1.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* rp = network.GetRequest(url_1, header_list1);

	QTime _t;
	_t.start();

	bool _timeout = false;

	while (rp && !rp->isFinished())
	{
		QCoreApplication::processEvents();
		if (_t.elapsed() >= 20000) {
			_timeout = true;
			break;
		}
	}

	WaitforSeconds(1);

	QVariant statusCodeV = rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	bool res = statusCodeV.toInt() == 200 ? true : false;

	if (rp != NULL)
		rp->deleteLater();

	return  res;
}

bool auto_dirvers_fatie::doPostFatie(const QString& content)
{
	//1. 获取验证码
	QString vcode, vcode_sign;
	if (!getCode(vcode, vcode_sign))
	{
		if (!getCode(vcode,vcode_sign))
		{
			emitMsgBar(QStringLiteral("获取验证码失败"));
			return false;
		}
		return false;
	}

	QString str_url = "http://comment8.mydrivers.com/Post.aspx";

	QUrl url1;
	url1.setUrl(str_url);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Cache-Control","no-cache"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("Host", m_host));
	header_list.push_back(HttpParamItem("Referer", m_url));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("ausername",""));
	post_data.push_back(HttpParamItem("checkcode",vcode));
	post_data.push_back(HttpParamItem("cid","1"));
	post_data.push_back(HttpParamItem("content",content));
	post_data.push_back(HttpParamItem("rid","0"));
	post_data.push_back(HttpParamItem("tid", m_newsid));
	post_data.push_back(HttpParamItem("usertype", "0"));
											
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

	WaitforSeconds(1);

	if (reply == NULL || (reply->error() != QNetworkReply::NoError) || _timeout)
	{
		return false;
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int status = statusCodeV.toInt();

	if (status != 201 && status != 200)
	{
		return false;
	}

	QByteArray rp_data = reply->readAll();
	reply->deleteLater();
	QString t = rp_data;
	int n = t.toInt();
	bool res = false;

	if (n == 5)
	{
		return true;
	}
	else if(n == 1)
	{
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		obj->ReportError("bestsalt", vcode_sign); 
		return false;
	}
	else
	{
		return false;
	}
}

bool auto_dirvers_fatie::GetFatieStatus(const QByteArray& rp_data)
{
	QString res_data = rp_data;
	int n = res_data.toInt();
	bool res = false;
	switch (n)
	{
	case 5: // 5成功
		res = true;
		break;
	default:
		break;
	}

	return res;
}

bool auto_dirvers_fatie::getCode(QString& vcode, QString& code_sign)
{
	int n = rand() % 9;
	double d = 0.987654321235647 / n;
	QString str_r = QString::number(d, 'f', 15);


	QString str_url = "http://comment8.mydrivers.com/radompage.aspx?" + str_r;

	QUrl url1;
	url1.setUrl(str_url);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("Host", m_host));
	header_list.push_back(HttpParamItem("Referer", m_url));
	header_list.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

	QNetworkReply* reply = network.GetRequest(url1, header_list);

	bool _timeout = false;

	QTime _t;
	_t.start();

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

	int status = statusCodeV.toInt();

	if (status != 201 && status != 200)
	{
		return false;
	}

	QByteArray rp_data = reply->readAll();
	reply->deleteLater();
	//   QVariant var = reply2->header(QNetworkRequest::ContentTypeHeader);
	// 
	//   QString type = var.toString();

	QImage image_ = QImage::fromData(rp_data);//(data.data(),100,40,QImage::Format_RGB32);
		// 在线验证
	bool result = image_.save("e:\\1.jpg");

	VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
	int res = obj->GetRecResults(rp_data, "bestsalt", "hh610520", "bestsalt", vcode, code_sign);

	if (res==0 && !vcode.isEmpty())
	{
		return true;
	}
	//obj->ReportError("bestsalt", code_sign); 
	return false;
}

void auto_dirvers_fatie::onActImportComment()
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
		return;
	}

	QTextStream text_stream(&file);
	//QString str = text_stream.readAll();
	int n_row = 0;
	ui.listWidget_comment_id1->clear();
	m_comment_item_list.clear();
	m_comments.clear();
	//m_comment_order = 0;

	while (!text_stream.atEnd())
	{
		QString line_str = text_stream.readLine();
		ui.listWidget_comment_id1->addItem(new QListWidgetItem(line_str));
	}

	file.close();
}

void auto_dirvers_fatie::onActClearComments()
{
	ui.listWidget_comment_id1->clear();
	m_comments.clear();
	m_comment_item_list.clear();
	//m_comment_order = 0;
}