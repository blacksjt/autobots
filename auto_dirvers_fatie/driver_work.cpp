#include "driver_work.h"
#include <QtNetwork>
#include "network.h"
#include <QTreeWidgetItem>
#include <QListWidgetItem>

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
	//Ë¢ÐÂÒ³Ãæ
	OpenNews();

	if (m_current_index >= m_comments.size())
	{
		return false;
	}

	QString str_url = "http://comment8.mydrivers.com/Post.aspx";

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

	HttpParamList post_data;
	post_data.push_back(HttpParamItem("ausername", ""));
	post_data.push_back(HttpParamItem("checkcode", ""));
	post_data.push_back(HttpParamItem("cid", "1"));
	post_data.push_back(HttpParamItem("content", m_comments[m_current_index++]));
	post_data.push_back(HttpParamItem("rid", "0"));
	post_data.push_back(HttpParamItem("tid", m_newsid));
	post_data.push_back(HttpParamItem("usertype", "1"));

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
	header_list1.push_back(HttpParamItem("User-Agent", "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)"));

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
