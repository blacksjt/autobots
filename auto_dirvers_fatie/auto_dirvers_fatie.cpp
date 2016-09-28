#include "auto_dirvers_fatie.h"
#include <QGroupBox>
#include <QListWidget>
#include "QMessageBox"
#include "vlidatecodeonline.h"
#include "dailconnector.h"
#include "QNetworkCookieJar"
#include "QFileDialog"

const int TIMEOUT = 20 * 1000;

QString GetTimeStr()
{
	qint64 n = QDateTime::currentMSecsSinceEpoch()/1000;

	return QString::number(n);
}

auto_dirvers_fatie::auto_dirvers_fatie(QWidget *parent)
	: QMainWindow(parent),m_control_stop(false)
{
	ui.setupUi(this);
	initializeControls(); // ��ʼ���ؼ�
	initializeConncetions(); // ��ʼ���źŲ�
}

auto_dirvers_fatie::~auto_dirvers_fatie()
{
	
}

void auto_dirvers_fatie::initializeConncetions()
{
	connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

	connect(ui.pushButton_stop, SIGNAL(clicked()), this, SLOT(onStop()));

	connect(ui.action_from_txt, SIGNAL(triggered()), this, SLOT(onActFromTxt()));

	connect(ui.action_clear_accounts, SIGNAL(triggered()), this, SLOT(onActClearAccounts()));

	connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

	connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));

	connect(ui.comboBox_adsl, SIGNAL(currentIndexChanged(const QString&)), this, SLOT(onChanged(const QString&)));

	connect(ui.lineEdit_url1, SIGNAL(textChanged(const QString &)), this, SLOT(onInputUrl1(const QString &)));
	connect(ui.lineEdit_url2, SIGNAL(textChanged(const QString &)), this, SLOT(onInputUrl2(const QString &)));
	connect(ui.lineEdit_url3, SIGNAL(textChanged(const QString &)), this, SLOT(onInputUrl3(const QString &)));

	intialVPN();
}

void auto_dirvers_fatie::onStart()
{
	ui.pushButton_start->setEnabled(false);

	// 1. ��ȡ��������
	if (!updateData())
	{
		ui.pushButton_start->setEnabled(true);
		return;
	}

	// 2. ��ʼ����
	AutoFatie();

	ui.pushButton_start->setEnabled(true);

}

void auto_dirvers_fatie::onStop()
{
	emitMsgBar(QStringLiteral("ֹͣ��.."));
	m_control_stop = true;
}

void auto_dirvers_fatie::emitMsgBar(const QString& msg)
{
	ui.statusBar->showMessage(msg);
}

bool auto_dirvers_fatie::updateData()
{
	//���ԭ����
	m_work_list.clear();
	m_account_list.clear();
	m_account_row_list.clear(); 
	m_account_order = 0;

	if (ui.comboBox_adsl->currentText().isEmpty())
	{
		//ui.lineEdit_msg->setText(QStringLiteral("������������"));
		QMessageBox::critical(this, "warning", QStringLiteral("������������"));
		return false;
	}

	if (ui.lineEdit_account->text().isEmpty())
	{
		//ui.lineEdit_msg->setText(QStringLiteral("���������˺�"));
		QMessageBox::critical(this, "warning", QStringLiteral("���������˺�"));
		return false;
	}

	if (ui.lineEdit_password->text().isEmpty())
	{
		//ui.lineEdit_msg->setText(QStringLiteral("������������"));
		QMessageBox::critical(this, "warning", QStringLiteral("������������"));
		return false;
	}

	//1. ��ӵ�1ƪ
	//1-1 tab1
	if (ui.lineEdit_host1->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("����hostû������"));
		return false;
	}

	if (ui.tableWidget_account_id->rowCount() < 1)
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("�뵼���ʺ�"));
		return false;
	}

	// ¼���ʺ�
	int n_id = ui.tableWidget_account_id->rowCount();
	for (int i = 0; i < n_id; ++i)
	{
		QTableWidgetItem* item = ui.tableWidget_account_id->item(i, 0);
		if (item->checkState() != Qt::Checked)
		{
			QString name = ui.tableWidget_account_id->item(i, 0)->text();
			QString password = ui.tableWidget_account_id->item(i, 1)->text();
			m_account_list.push_back(AccountParam(name, password));
			m_account_row_list.push_back(ui.tableWidget_account_id->item(i, 0));
		}
	}

	m_interval = ui.spinBox_interval->value();

	DriverWork work1;
	work1.setHost(ui.lineEdit_host1->text());
	work1.setUrl(ui.lineEdit_url1->text());
	work1.setNewsid(ui.lineEdit_news_id1->text());
	work1.setNetwork(&network);
	
	//2. �������
	int n_cmt1 = ui.listWidget_comment_id1->count();
	for (int i = 0; i < n_cmt1; ++i)
	{
		QListWidgetItem* item = ui.listWidget_comment_id1->item(i);

		if (item->checkState() != Qt::Checked)
		{
			QString text = item->text();
			work1.addComment(text);
			work1.addCommentItem(item);
		}
	}

	if (!work1.checkContent())
	{
		QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("��һƪ�������벻����"));
		return false;
	}

	m_work_list.push_back(work1);

	//2. ��ӵ�2ƪ
	if (!ui.lineEdit_url2->text().isEmpty())
	{
		DriverWork work2;
		work2.setHost(ui.lineEdit_host1->text());
		work2.setUrl(ui.lineEdit_url2->text());
		work2.setNewsid(ui.lineEdit_news_id2->text());
		work2.setNetwork(&network);

		//2. �������
		int n_cmt2 = ui.listWidget_comment_id2->count();
		for (int i = 0; i < n_cmt2; ++i)
		{
			QListWidgetItem* item = ui.listWidget_comment_id2->item(i);

			if (item->checkState() != Qt::Checked)
			{
				QString text = item->text();
				work2.addComment(text);
				work2.addCommentItem(item);
			}
		}

		if (!work2.checkContent())
		{
			QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("�ڶ�ƪ�������벻����"));
			m_work_list.clear();
			return false;
		}

		m_work_list.push_back(work2);
	}

	//3. ��ӵ�3ƪ
	if (!ui.lineEdit_url3->text().isEmpty())
	{
		DriverWork work3;
		work3.setHost(ui.lineEdit_host1->text());
		work3.setUrl(ui.lineEdit_url3->text());
		work3.setNewsid(ui.lineEdit_news_id3->text());
		work3.setNetwork(&network);

		//2. �������
		int n_cmt3 = ui.listWidget_comment_id3->count();
		for (int i = 0; i < n_cmt3; ++i)
		{
			QListWidgetItem* item = ui.listWidget_comment_id3->item(i);

			if (item->checkState() != Qt::Checked)
			{
				QString text = item->text();
				work3.addComment(text);
				work3.addCommentItem(item);
			}
		}

		if (!work3.checkContent())
		{
			QMessageBox::critical(this, QStringLiteral("��ʾ"), QStringLiteral("����ƪ�������벻����"));
			m_work_list.clear();
			return false;
		}

		m_work_list.push_back(work3);
	}

	return true;
}

void auto_dirvers_fatie::initializeControls()
{
	//addTabPage(); // ���һ��tabҳ��
}



void auto_dirvers_fatie::AutoFatie()
{
	//DailConnector connector("VPN", "ycc1", "111");

	QString adsl = ui.comboBox_adsl->currentText();
	QString account = ui.lineEdit_account->text();
	QString pwd = ui.lineEdit_password->text();

	DailConnector connector(adsl, account, pwd);

	while (!m_control_stop)
	{
		//1. ��VPN
		QNetworkCookieJar* cookie = new QNetworkCookieJar(this);

		network.GetManager().setCookieJar(cookie);

#ifdef NDEBUG
 		emitMsgBar(QStringLiteral("������..."));
 		QString dial_msg;
 		while (!connector.ReConnect(dial_msg))
 		{
 			emitMsgBar(QStringLiteral("������ʧ�ܣ�") + dial_msg);
 			WaitforSeconds(3);
 			emitMsgBar(QStringLiteral("������..."));
 		}
 
 		emitMsgBar(QStringLiteral("���ųɹ�..."));

 		int ncount = 0;
 		while (!GetContent() && ncount < 15)
 		{
 			connector.ReConnect(dial_msg);
 			WaitforSeconds(3);
 			ncount++;
 		}
 
 		if (ncount >= 15)
 		{
 			emitMsgBar(QStringLiteral("��վ����ʧ��...,������������"));
 			break;
 		}
#else
        GetContent();
#endif
		//2. ��¼
		bool login_status = false;
		while (m_account_order < m_account_list.size())
		{
			AccountParam ac = m_account_list.at(m_account_order);
			int ndepth = 1;
			if (!Login(ac._id, ac._password, ndepth))
			{
				emitMsgBar(QStringLiteral("��½ʧ��..."));
				//m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(255, 0, 0, 180));
				m_account_order++;
				continue;
			}
			else
			{
				emitMsgBar(QStringLiteral("��½�ɹ�"));
				m_account_row_list.at(m_account_order)->setCheckState(Qt::Checked);
				ui.tableWidget_account_id->item(m_account_order, 0)->setBackgroundColor(QColor(0, 255, 0, 180));
				m_account_order++;
				login_status = true;
				break;
			}
		}

		if (!login_status)
		{
			emitMsgBar(QStringLiteral("1������������ \n 2���ŵ�½���ɹ����޷�����"));
			return;
		}

		//3. ����
		for (int k = 0; k < m_work_list.size(); ++k)
		{
			if (!m_work_list[k].isFinished())
			{
				if (m_work_list[k].doWork())
				{
					int current_id = m_work_list.at(k).getCurrentIndex();
					QString msg = QStringLiteral("��%1ƪ����%2���������").arg(k+1,current_id);
					emitMsgBar(msg);
				}
				else
				{
					int current_id = m_work_list.at(k).getCurrentIndex();
					QString msg = QStringLiteral("��%1ƪ����%2����ʧ��").arg(k+1,current_id);
					emitMsgBar(msg);
				}

				// ���ȫ����ɣ����˳�
				if (isAllFinished())
				{
					logout();
					cookie->deleteLater();
					connector.DisConnect();
					emitMsgBar(QStringLiteral("�����"));
					return;
				}

				//�ȴ�
				QElapsedTimer t1;
				t1.start();
				while (t1.elapsed() < m_interval*1000)
					QCoreApplication::processEvents();
			}	
		}

		//4. �ǳ�
		logout();

		//5. ɾ��cookies
		cookie->deleteLater();
	}

 	connector.DisConnect();
 	emitMsgBar(QStringLiteral("�����"));
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
 	QString str_url_1 = "http://www.mydrivers.com/";
 
 	QUrl url_1(str_url_1);
 	url_1.setUrl(str_url_1);
 	HttpParamList header_list1;
 	header_list1.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
 	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
 	header_list1.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
 	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
 	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
 	header_list1.push_back(HttpParamItem("DNT", "1"));
 	//header_list1.push_back(HttpParamItem("Cache-Control", "max-age=0"));
 	header_list1.push_back(HttpParamItem("Host", "www.mydrivers.com")); // mhost
 	header_list1.push_back(HttpParamItem("User-Agent", S_UA));
 
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

bool auto_dirvers_fatie::GetFatieStatus(const QByteArray& rp_data)
{
	QString res_data = rp_data;
	int n = res_data.toInt();
	bool res = false;
	switch (n)
	{
	case 5: // 5�ɹ�
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
	QString str_r = QString::number(d, 'f', 17);


	QString str_url = "http://passport.mydrivers.com/modules/yanzhengma.aspx?" + str_r;

	QUrl url1;
	url1.setUrl(str_url);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "image/webp,image/*,*/*;q=0.8"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	//header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("DNT", "1"));
	header_list.push_back(HttpParamItem("Host", "passport.mydrivers.com"));
	header_list.push_back(HttpParamItem("Referer", "http://www.mydrivers.com/"));
	header_list.push_back(HttpParamItem("User-Agent", S_UA));

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
		QString msg = reply->errorString();
		emitMsgBar(msg);
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

	
		// ������֤

	
#ifdef _DEBUG
	QImage image_ = QImage::fromData(rp_data);//(data.data(),100,40,QImage::Format_RGB32);
	image_.save("e:\\1.jpg");
#endif
	
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
	QString path = QFileDialog::getOpenFileName(this, QStringLiteral("��ѡ���ļ�·��"), "", QStringLiteral("Txt(*.txt)"));
	if (path.isEmpty())  //�ж��û��Ƿ�ѡ�����ļ�
	{
		//emitLogMessage(QStringLiteral("ȡ�����ļ�"));
		return;
	}

	QFile file(path);

	if (!file.open(QIODevice::ReadOnly)) {
		QMessageBox::critical(NULL, "��ʾ", "�޷����ļ�");
		return;
	}

	QTextStream text_stream(&file);
	
	if (ui.tabWidget_comments->currentIndex() == 0)
	{
		ui.listWidget_comment_id1->clear();

		while (!text_stream.atEnd())
		{
			QString line_str = text_stream.readLine();
			ui.listWidget_comment_id1->addItem(new QListWidgetItem(line_str));
		}
	}
	else if(ui.tabWidget_comments->currentIndex() == 1)
	{
		ui.listWidget_comment_id2->clear();

		while (!text_stream.atEnd())
		{
			QString line_str = text_stream.readLine();
			ui.listWidget_comment_id2->addItem(new QListWidgetItem(line_str));
		}
	}
	else
	{
		ui.listWidget_comment_id3->clear();

		while (!text_stream.atEnd())
		{
			QString line_str = text_stream.readLine();
			ui.listWidget_comment_id3->addItem(new QListWidgetItem(line_str));
		}
	}


	file.close();
}

void auto_dirvers_fatie::onActClearComments()
{
	ui.listWidget_comment_id1->clear();
	ui.listWidget_comment_id2->clear();
	ui.listWidget_comment_id3->clear();
}


bool auto_dirvers_fatie::Login(const QString& _id, const QString& _password, int depth)
{
	//1.��ȡ��֤��
// 	QNetworkCookieJar* cookiejar = network.GetManager().cookieJar();
// 
// 	QString str_time = GetTimeStr();
// 	QNetworkCookie lpvt_cookie("Hm_lpvt_fa993fdd33f32c39cbb6e7d66096c422", str_time.toLatin1());
// 	lpvt_cookie.setDomain(".mydrivers.com");
// 
// 	cookiejar->insertCookie(lpvt_cookie);
// 
// 	QNetworkCookie lvt_cookie("Hm_lvt_fa993fdd33f32c39cbb6e7d66096c422", str_time.toLatin1());
// 	lpvt_cookie.setDomain(".mydrivers.com");
// 	cookiejar->insertCookie(lvt_cookie);

	//1. ��ȡ��֤��
	QString vcode, vcode_sign;
	if (!getCode(vcode, vcode_sign))
	{
		if (!getCode(vcode, vcode_sign))
		{
			emitMsgBar(QStringLiteral("��ȡ��֤��ʧ��"));
			return false;
		}
	}

	//2. ��¼
	QString str_url = "http://passport.mydrivers.com/comments/check_login.aspx?ReturnUrl=http://www.mydrivers.com/";

	QUrl url1;
	url1.setUrl(str_url);

	QString post_data_str = "password=" + _password + "&username=" + _id + "&yzm=" + vcode;

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	//header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("DNT", "1"));
	header_list.push_back(HttpParamItem("Host", "passport.mydrivers.com"));
	header_list.push_back(HttpParamItem("Referer", "http://www.mydrivers.com/"));
	header_list.push_back(HttpParamItem("User-Agent", S_UA));

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("password", _password));
	post_data.push_back(HttpParamItem("username", _id));
	post_data.push_back(HttpParamItem("yzm", vcode));

	QNetworkReply* reply = network.PostRequest(url1, header_list,post_data);

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

	QString rt_msg = QString::fromLocal8Bit(rp_data);

	reply->deleteLater();

	if (rt_msg.contains("top.location.href")  && !rt_msg.contains("alert"))
	{
		return true;
	}

	if (rt_msg.contains(QStringLiteral("��֤��")))
	{
		VlidateCodeOnLine* obj = VlidateCodeOnLine::GetInstance();
		obj->ReportError("bestsalt", vcode_sign);

		if (depth < 5)
		{
			// ���Ƶݹ����
			return Login(_id, _password, depth++);
		}	
	}

	return false;
}




void auto_dirvers_fatie::onActFromTxt()
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
		return;
	}

	QTextStream text_stream(&file);
	//QString str = text_stream.readAll();
	int n_row = 0;
	ui.tableWidget_account_id->clearContents();
	ui.tableWidget_account_id->setRowCount(0);
	m_account_list.clear();
	m_account_order = 0;
	m_account_row_list.clear();

	while (!text_stream.atEnd())
	{
		QString line_str = text_stream.readLine();

		QStringList str_list = line_str.split("----");

		if (str_list.size() >= 2)
		{
			ui.tableWidget_account_id->setRowCount(ui.tableWidget_account_id->rowCount() + 1);
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

void auto_dirvers_fatie::onActClearAccounts()
{
	ui.tableWidget_account_id->clearContents();
	ui.tableWidget_account_id->setRowCount(0);
	m_account_order = 0;
	m_account_list.clear();
	m_account_row_list.clear();
}

void auto_dirvers_fatie::logout()
{
	QString str_url_1 = "http://passport.mydrivers.com/logout.aspx?returnurl=http://www.mydrivers.com";

	QUrl url_1(str_url_1);
	url_1.setUrl(str_url_1);
	HttpParamList header_list1;
	header_list1.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list1.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list1.push_back(HttpParamItem("DNT", "1"));
	//header_list1.push_back(HttpParamItem("Cache-Control", "max-age=0"));
	header_list1.push_back(HttpParamItem("Host", "passport.mydrivers.com")); // mhost
	header_list1.push_back(HttpParamItem("User-Agent", S_UA));

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

	rp->deleteLater();
}

bool auto_dirvers_fatie::isAllFinished()
{
	for (int i=0 ; i < m_work_list.size(); ++i)
	{
		if (!m_work_list[i].isFinished())
		{
			return false;
		}
	}
	return true;
}

QString auto_dirvers_fatie::GetMatchedText(const QString& text)
{
	//QString s = "http://comment8.mydrivers.com/review/497910-1.htm";
	QString pattern = "^http://.*/([0-9]*)-1.htm$";
	QRegExp regexp(pattern, Qt::CaseInsensitive);

	int pos = text.indexOf(regexp);

	QString s1 = regexp.cap(0);
	QString s2 = regexp.cap(1);

	return s2;
}

void auto_dirvers_fatie::onInputUrl1(const QString & text)
{
	QString news_id = GetMatchedText(text);
	ui.lineEdit_news_id1->setText(news_id);
}
void auto_dirvers_fatie::onInputUrl2(const QString & text)
{
	QString news_id = GetMatchedText(text);
	ui.lineEdit_news_id2->setText(news_id);
}
void auto_dirvers_fatie::onInputUrl3(const QString & text)
{
	QString news_id = GetMatchedText(text);
	ui.lineEdit_news_id3->setText(news_id);
}


void auto_dirvers_fatie::intialVPN()
{
	// ��ȡ�����ļ�
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("ȱ��ini�����ļ�"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);
	QStringList groups = setting.childGroups();
	ui.comboBox_adsl->addItems(groups);

	// ����Ĭ��ֵ
	if (groups.isEmpty())
	{
		QMessageBox::critical(this, "warning", QStringLiteral("�����ļ���û����Чֵ"));
		return;
	}

	int ndefault = setting.value("default").toInt();

	if (ndefault >= groups.size() || ndefault < 0)
	{
		ndefault = 0;
	}

	QString first = groups.at(ndefault);
	ui.comboBox_adsl->setCurrentText(first);
	setting.beginGroup(first);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}

void auto_dirvers_fatie::SaveIniSettings()
{
	// ��ȡ�����ļ�
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("ȱ��ini�����ļ�"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);
	setting.setValue("default", ui.comboBox_adsl->currentIndex());
}

void auto_dirvers_fatie::onChanged(const QString& text)
{
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("ȱ��ini�����ļ�"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);

	setting.beginGroup(text);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}