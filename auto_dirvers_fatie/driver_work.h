#ifndef DIRVER__WORK__HHH__
#define DIRVER__WORK__HHH__

#include <QString>
#include <QStringList>
#include "network.h"

const QString S_UA = "Mozilla/5.0 (compatible; MSIE 9.0; Windows NT 6.1; WOW64; Trident/5.0)";
//const QString S_UA = "Mozilla/5.0 (iPhone; CPU iPhone OS 9_3_2 like Mac OS X) AppleWebKit/601.1.46 (KHTML, like Gecko) Mobile/13F69";
//const QString S_UA = "Mozilla / 5.0 (Linux; U; Android 4.4.4; zh - cn; MI 3 Build / KTU84P) AppleWebKit / 533.1 (KHTML, like Gecko) Version / 4.0 Mobile Safari / 533.1";

class QListWidgetItem;

class DriverWork
{
public:
	DriverWork();
	DriverWork(const DriverWork&);


public:
	toutiao_network* getNetwork() const { return network; }
	void setNetwork(toutiao_network* val) { network = val; }
	int getCurrentIndex() const { return m_current_index; }
	QString getUrl() const { return m_url; }
	void setUrl(const QString& val) { m_url = val; }
	QString getHost() const { return m_host; }
	void setHost(const QString& val) { m_host = val; }
	QString getNewsid() const { return m_newsid; }
	void setNewsid(const QString& val) { m_newsid = val; }
	QStringList getComments() const { return m_comments; }
	void setComments(const QStringList& val) { m_comments = val; }
	QList<QListWidgetItem*>& Comment_item_list();
	void setComment_item_list(const QList<QListWidgetItem*>& val);

	void addComment(const QString& cmt);
	void addCommentItem( QListWidgetItem*);
	bool checkContent();

	bool isFinished();

	bool doWork();
	bool OpenNews();
private:
	QString m_url;
	QString m_host;
	QString m_newsid;
	QStringList m_comments;
	QList<QListWidgetItem*> m_comment_item_list;
	toutiao_network* network;
	int m_current_index;
};

#endif //DIRVER__WORK__HHH__