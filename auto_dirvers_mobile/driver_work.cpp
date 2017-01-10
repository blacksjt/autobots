#include "driver_work.h"
#include <QtNetwork>
#include "network.h"
#include <QTreeWidgetItem>
#include <QListWidgetItem>
#include "md5.h"

const int TIMEOUT = 20 * 1000;

DriverWork::DriverWork()
{
	m_current_index = 0;
}

DriverWork::DriverWork(const DriverWork& other)
{
	this->m_url = other.m_url;
	this->m_host = other.m_host;
	this->m_newsid= other.m_newsid;
	this->m_comments= other.m_comments;
	this->m_comment_item_list= other.m_comment_item_list;
	this->network = other.network;
	this->m_current_index= other.m_current_index;
}

QList<QListWidgetItem*>& DriverWork::Comment_item_list()
{
	return m_comment_item_list;
}

void DriverWork::setComment_item_list(const QList<QListWidgetItem*>& val)
{
	m_comment_item_list = val;
}

void DriverWork::addComment(const QString& cmt)
{
	m_comments.push_back(cmt);
}

void DriverWork::addCommentItem( QListWidgetItem* item)
{
	m_comment_item_list.push_back(item);
}

bool DriverWork::checkContent()
{
	if (m_url.isEmpty() || m_host.isEmpty() || m_newsid.isEmpty()
		|| m_comments.isEmpty())
	{
		return false;
	}

	return true;
}

bool DriverWork::isFinished()
{
	return m_current_index >= m_comments.size();
}

bool DriverWork::doWork()
{
	//刷新页面
	//OpenNews();

	if (m_current_index >= m_comments.size())
	{
		return false;
	}

	QString str_url = "http://dt.kkeji.com/api/2/comments/postcomments";

	QUrl url1;
	url1.setUrl(str_url);

	HttpParamList header_list;
	header_list.push_back(HttpParamItem("Accept", "*/*"));
	header_list.push_back(HttpParamItem("Connection", "Keep-Alive"));
	//header_list.push_back(HttpParamItem("Accept-Encoding", "deflate"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	header_list.push_back(HttpParamItem("Accept-Language", "zh-Hans-CN;q=1"));
	header_list.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded"));
	header_list.push_back(HttpParamItem("Cache-Control", "no-cache"));
	//header_list.push_back(HttpParamItem("X-Requested-With", "XMLHttpRequest"));
	header_list.push_back(HttpParamItem("Host", "dt.kkeji.com"));
	//header_list.push_back(HttpParamItem("Referer", m_url));
	header_list.push_back(HttpParamItem("User-Agent", S_UA));

    /*article_id	510546
        ausername
        brand	Apple
        cfrom	iphone
        content	好弱的卡罗拉
        dev	iPhone6
        model	6
        password	4cf7e4ce68e24b7f179971bf2b480354
        rid	0
        sdk	9.3.5
        sign	572098795
        type
        udid	5507240564344282355
        uid	1215734
        username	gycsjf123
        uuid
        version	3.0.6
        xaid	20ff00569e1795a434ee41a361fc98545a63313a*/


	HttpParamList post_data;
	post_data.push_back(HttpParamItem("article_id", m_newsid));
	post_data.push_back(HttpParamItem("ausername", ""));
	post_data.push_back(HttpParamItem("brand", "Apple"));
    post_data.push_back(HttpParamItem("cfrom", "iphone"));
	post_data.push_back(HttpParamItem("content", m_comments[m_current_index++]));
    post_data.push_back(HttpParamItem("dev", "iPhone6"));
    post_data.push_back(HttpParamItem("model", "6"));
	post_data.push_back(HttpParamItem("password", QString::fromStdString(md5("610520"))));
    post_data.push_back(HttpParamItem("rid", "0"));
    post_data.push_back(HttpParamItem("sdk", "9.3.5"));
    post_data.push_back(HttpParamItem("sign", "572098795"));
    post_data.push_back(HttpParamItem("type", ""));
    post_data.push_back(HttpParamItem("udid", "5507240564344282355"));
    post_data.push_back(HttpParamItem("uid", "1215734"));
    post_data.push_back(HttpParamItem("username", "gycsjf123"));
	post_data.push_back(HttpParamItem("uuid", ""));
	post_data.push_back(HttpParamItem("version", "3.0.6"));
    post_data.push_back(HttpParamItem("xaid", "20ff00569e1795a434ee41a361fc98545a63313a"));

	QNetworkReply* reply = network->PostRequest(url1, header_list, post_data);

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
        QString msg = reply->readAll();
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
		m_comment_item_list[m_current_index - 1]->setCheckState(Qt::Checked);
		return true;
	}
	else
	{
		return false;
	}
}

bool DriverWork::OpenNews()
{
	QUrl url_1;
	url_1.setUrl(m_url);
	HttpParamList header_list1;
	header_list1.push_back(HttpParamItem("Accept", "text/html, application/xhtml+xml, */*"));
	header_list1.push_back(HttpParamItem("Connection", "Keep-Alive"));
	header_list1.push_back(HttpParamItem("Accept-Encoding", "gzip, deflate"));
	header_list1.push_back(HttpParamItem("Accept-Language", "zh-cn"));
	header_list1.push_back(HttpParamItem("Content-Type", "application/x-www-form-urlencoded; charset=UTF-8"));
	header_list1.push_back(HttpParamItem("DNT", "1"));
	//header_list1.push_back(HttpParamItem("Cache-Control", "max-age=0"));
	header_list1.push_back(HttpParamItem("Host", m_host)); // mhost
	header_list1.push_back(HttpParamItem("User-Agent", S_UA));

	QNetworkReply* rp = network->GetRequest(url_1, header_list1);

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

	QVariant statusCodeV = rp->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	bool res = statusCodeV.toInt() == 200 ? true : false;

	if (rp != NULL)
		rp->deleteLater();

	return  res;
}
