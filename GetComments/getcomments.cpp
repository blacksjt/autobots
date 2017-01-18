#include "getcomments.h"
#include "QStandardPaths"
#include <QFile>
#include "QMessageBox"
#include "QTextStream"
#include "QNetworkAccessManager"
#include "QNetworkRequest"
#include "QNetworkReply"
#include "QJsonDocument"
#include "QJsonObject"
#include "QJsonArray"
GetComments::GetComments(QWidget *parent)
    : QMainWindow(parent)
{
    ui.setupUi(this);
    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));
}

GetComments::~GetComments()
{

}

void GetComments::onStart()
{
    updataData();

    startWork();
}

void GetComments::updataData()
{
    if (ui.lineEdit_news_id->text().isEmpty())
    {
        QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Group ID没有输入"));
        return ;
    }

	if (ui.lineEdit_item_id->text().isEmpty())
	{
		QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Item ID没有输入"));
		return;
	}

    if (ui.spinBox_count->value() < 0)
    {
        QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("提取数量小于0"));
        return ;
    }


	m_group_id = ui.lineEdit_news_id->text();
	m_item_id = ui.lineEdit_item_id->text();
    m_count = ui.spinBox_count->value();
    m_file_path = QStandardPaths::writableLocation(QStandardPaths::DesktopLocation);
    m_file_path = m_file_path + "/" + m_group_id + ".txt";
}

void GetComments::startWork()
{
    QString temp_count = QString::number(m_count);

	int ncount = 0;
	bool has_more = true;
	QStringList comments;

	while (ncount < m_count && has_more)
	{
		has_more = GetSubComments(ncount, comments);
		ncount += 20;
	}

	if (comments.isEmpty())
	{
		ui.lineEdit_msg->setText(QStringLiteral("提取失败"));
		return;
	}

	bool res = WriteToFile(comments);
    if (res)
    {
       ui.lineEdit_msg->setText(QStringLiteral("提取成功，已写入文件：")+m_file_path);
    }
    else
    {
       ui.lineEdit_msg->setText(QStringLiteral("写文件失败"));
    }

}

bool GetComments::GetSubComments(int& current_count, QStringList& cmts)
{
	QString url = QString("http://www.toutiao.com/api/comment/list/?group_id=%1&item_id=%2&offset=%3&count=20")
		.arg(m_group_id, m_item_id, QString::number(current_count));

	QNetworkAccessManager manager;
	QNetworkRequest req;
	req.setUrl(url);
	req.setRawHeader("Cache-Control", "no-cache");
	req.setRawHeader("Accept", "text/javascript, text/html, application/xml, text/xml, */*");
	req.setRawHeader("Connection", "Keep-Alive");
	req.setRawHeader("X-Requested-With", "XMLHttpRequest");
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
	}

	QVariant statusCodeV = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);

	int n = statusCodeV.toInt();

	if (n == 200)
	{
		ui.lineEdit_msg->setText(QStringLiteral("连接成功"));
	}

	QByteArray rp_data = reply->readAll();

	bool res = ExactComments(rp_data, cmts);

	reply->deleteLater();
	return res;
}

bool GetComments::ExactComments(const QByteArray& rp_data, QStringList& comments)
{
    bool res = false;
    //QStringList comments;
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

                if (data_obj.contains("comments"))
                {
                    QJsonArray comments_arry = data_obj.take("comments").toArray();
                    for (int i = 1; i < comments_arry.size(); ++i)
                    {
                        QJsonObject comment_obj = comments_arry.at(i).toObject();
                        if (comment_obj.contains("text"))
                        {
                            QString c = comment_obj.take("text").toString();
                            comments.push_back(c);
                        }
                    }

                }
            }
        }
    }

    if (comments.isEmpty())
    {
        return false;
    }
	else
	{
		return true;
	}

    // 写文件
    //res = WriteToFile(comments);

    //return res;
}

bool GetComments::WriteToFile(const QStringList& comments)
{  
    QFile file(m_file_path);
    
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text))
    {
        ui.lineEdit_msg->setText(m_file_path +":文件打开失败");
        return false;
    }

    QTextStream out(&file);
    
    for each (QString var in comments)
    {
        out << var << endl;
    }

    out.flush();

    file.close();

    return true;
}
