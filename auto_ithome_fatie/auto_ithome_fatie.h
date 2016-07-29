#ifndef AUTOBOTS_TOUTIAO_H
#define AUTOBOTS_TOUTIAO_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_autobots_toutiao.h"
#include "toutiao_network.h"
#define WINVER 0x0501
struct AccountParam
{
public:
  AccountParam(QString name, QString password)
  {
    _id = name;
    _password = password;
  }

public:
  QString _id;
  QString _password;
};

typedef QList<AccountParam> AccountList;

struct SinaData
{
public:
  QString _servertime;
  QString _pcid;
  QString _nonce;
  QString _pubkey;
  QString _rsakv;
  QString _ticket;
  bool    _showpin;
};

class auto_ithome_fatie : public QMainWindow
{
    Q_OBJECT

public:
	auto_ithome_fatie(QWidget *parent = 0);
    ~auto_ithome_fatie();

private:

private slots:
  void onStart();
  void onPause();
  void onAddCommentID();
  void onActFromTxt();
  void onActImportComment();
  void onActClearComments();
  void onActClearAccounts();
  void onDeveliver();
private:
  bool DoPostFatie(const QString& content);
  bool GetContent();
  bool RequestForLogin();
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();
  bool GetFatieStatus(const QByteArray& byte_arr);
  void FatieSucceed(const QString& id);
  int GetLoginResult(const QByteArray& data);
  bool DoWork();
  void AutoFatie();
  void EmitMsgStatusBar(const QString& msg);
  void WaitforSeconds(int nseconds);


  bool Login(const QString& name, const QString& password);

private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;
    
    QString m_post_id;
    QString m_csrf_token;

    QString m_client_id;
    QString m_appkey;
    QString m_state;

    bool m_code_online;
    bool m_auto_change_ip;
private:
    QString m_host;
    QString m_url;
    QString m_news_id;
    //QString m_chanel;
    int m_interval;
    QStringList m_comment_list;
    QList<QListWidgetItem*> m_comment_item_list;
    AccountList m_account_list;
    QList<QTableWidgetItem*> m_account_row_list;
    int m_account_order;
    int m_comment_order;
};

#endif // AUTOBOTS_TOUTIAO_H
