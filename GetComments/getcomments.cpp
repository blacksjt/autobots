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

QString GetTimeStr()
{
	qint64 n = QDateTime::currentMSecsSinceEpoch();

	return QString::number(n);
}

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
		ncount += 19;
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
	QString url = QString("http://is.snssdk.com/article/v1/tab_comments/?version_code=5.9.8&app_name=news_article&vid=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&device_id=3135986566&channel=App%20Store&resolution=750*1334&aid=13&ab_version=111305,109656,101786,111547,101533,109464,110341,109893,109777,106784,97143,111342,101558,94042,105610,111814,105822,111317,111613,110304,108978,111799,108489,110718,110795,111417,98042,111606,105475&ab_feature=z2&ab_group=z2&openudid=0d919477efbefb99dfe7a02a2df34d9127ecc947&live_sdk_version=1.6.5&idfv=674FB6B1-60E9-4315-88FC-AAC84BEFAB46&ac=WIFI&os_version=9.3.5&ssmix=a&device_platform=iphone&iid=7730017535&ab_client=a1,f2,f7,e1&device_type=iPhone%206&idfa=86E011D2-C2DA-40CB-AB9D-DB1E1F9D668A&aggr_type=1&count=20&group_id=%1&item_id=%2&offset=%3&tab_index=0")
		.arg(m_group_id, m_item_id, QString::number(current_count));

	QNetworkAccessManager manager;
	QNetworkRequest req;
	req.setUrl(url);
	req.setRawHeader("tt-request-time", GetTimeStr().toUtf8().data());
	req.setRawHeader("Accept", "*/*");
	req.setRawHeader("Connection", "Keep-Alive");
	//req.setRawHeader("X-Requested-With", "XMLHttpRequest");
	req.setRawHeader("Accept-Language", "zh-Hans-CN;q=1");
	req.setRawHeader("Host", "is.snssdk.com");
	req.setRawHeader("User-Agent", "News/5.9.8 (iPhone; iOS 9.3.5; Scale/2.00)");

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
				QJsonArray data_obj_arr = obj.take("data").toArray();

				for (int i = 1; i < data_obj_arr.size(); ++i)
				{
					QJsonObject comment_obj = data_obj_arr.at(i).toObject();
					if (comment_obj.contains("comment"))
					{
						QJsonObject comment_ = comment_obj.take("comment").toObject();
						if (comment_.contains("text"))
						{
							QString c = comment_.take("text").toString();
							if (c.contains("//@"))
							{
								c = c.left(c.indexOf("//@"));
							}
							comments.push_back(c);
						}
					}
				}
            }

			if (obj.contains("has_more"))
			{
				res = obj.take("has_more").toBool();
			}
        }
    }

    if (comments.isEmpty())
    {
        return false;
    }
	else
	{
		return res;
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
