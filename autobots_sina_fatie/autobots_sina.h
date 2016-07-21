#ifndef AUTOBOTS_TOUTIAO_H
#define AUTOBOTS_TOUTIAO_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_autobots_toutiao.h"
#include "toutiao_network.h"

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

class autobots_toutiao : public QMainWindow
{
    Q_OBJECT

public:
    autobots_toutiao(QWidget *parent = 0);
    ~autobots_toutiao();

private:

private slots:
  void onStart();
  void onPause();
  //void onPostIdReday(QString);
  void onAddCommentID();
  //void onCsrfTokenReday(QString str);
  //void onLoginsucceed(QString id);
  void onActFromTxt();
  void onActImportComment();
  void onActClearComments();
  void onActClearAccounts();
  void onDeveliver();
  //void onClientIdReday(QString _id);
  //void onFatieSucceed(QString comment, double id);
private:
  int LogInbySina();
  //QNetworkReply* LogInbyRenren(const QString& name, const QString& password);
  bool DoPostFatie(const QString& content);
  void GetUerInfo();
  bool GetContent();
  void GetConnection();
  void LoginRequest();
  bool RequestForLogin();
  void CodeCheckForRenren();
  bool AuthorBySina(const QString& name, const QString& password);
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();
  bool GetCsrfToken(const QByteArray& arr);
  bool ProcessRedirectSSL(const QString& str);
  bool ProcessRedirectGet(const QString& str);
  bool GetPostId(const QByteArray& arr);
  bool ProcessRedirectLoginGet(const QString& str);
  bool ProcessRedirectLoginGet2(const QString& str);
  bool GetFatieStatus(const QByteArray& byte_arr);
  void FatieSucceed(const QString& id);
  bool PreLoginSina(const QString& name,  SinaData& data, QString& vcode, QString& code_sign);
  bool GetPreLoginResult(const QByteArray& str,SinaData& data);
  int GetLoginResult(const QByteArray& str,SinaData& data);
  int LoginSina(SinaData& sina_data, const QString& name, 
    const QString& password,const QString& vcode);
  bool AuthorizeSina(const SinaData& sina_data);
  bool GetQrid(const QByteArray& rp_data, QString& qrid);
  bool DoWork();
  void AutoFatie();
  void EmitMsgStatusBar(const QString& msg);
  void WaitforSeconds(int nseconds);

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
    QString m_chanel;
    int m_interval;
    QStringList m_comment_list;
    QList<QListWidgetItem*> m_comment_item_list;
    AccountList m_account_list;
    QList<QTableWidgetItem*> m_account_row_list;
    int m_account_order;
    int m_comment_order;
};

#endif // AUTOBOTS_TOUTIAO_H
