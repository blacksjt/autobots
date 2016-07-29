#ifndef AUTO_DIRVERS_FATIE_H
#define AUTO_DIRVERS_FATIE_H

#include <QtWidgets/QMainWindow>
#include "ui_auto_dirvers_fatie.h"
#include "network.h"
#define WINVER 0x0501
class DriverWork
{
public:


public:
	QString getUrl() const { return m_url; }
	void setUrl(QString val) { m_url = val; }
	QString getHost() const { return m_host; }
	void setHost(QString val) { m_host = val; }
	QString getNewsid() const { return m_newsid; }
	void setNewsid(QString val) { m_newsid = val; }
	QStringList getComments() const { return m_comments; }
	void setComments(QStringList val) { m_comments = val; }

private:
	QString m_url;
	QString m_host;
	QString m_newsid;
	QStringList m_comments;
};


class auto_dirvers_fatie : public QMainWindow
{
	Q_OBJECT

public:
	auto_dirvers_fatie(QWidget *parent = 0);
	~auto_dirvers_fatie();
	void initializeConncetions();

private slots:
	void onStart();
	void onStop();
	void onActImportComment();
	void onActClearComments();
private:
	void emitMsgBar(const QString& msg); // 输出信息到状态栏
	bool updateData(); // 提取界面数据到
	void initializeControls();
	void addTabPage();
	void AutoFatie();
	void WaitforSeconds(int nseconds);
	bool GetContent();
	bool doPostFatie(const QString& content);
	bool GetFatieStatus(const QByteArray& rp_data);
	bool getCode(QString& vcode, QString& code_sign);


private:
	//QList<DriverWork> m_work_list;
	//DriverWork m_work;
	Ui::auto_dirvers_fatieClass ui;
	toutiao_network network;
	bool m_control_stop;
	QString m_url;
	QString m_host;
	QString m_newsid;
	QStringList m_comments;
	QList<QListWidgetItem*> m_comment_item_list;
	int m_interval;
};

#endif // AUTO_DIRVERS_FATIE_H
