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
  bool RequestForRenren();
  void CodeCheckForRenren();
  bool AuthorByRenren(const QString& name, const QString& password);
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();
  bool GetCsrfToken(const QByteArray& arr);
  bool ProcessRedirectSSL(const QString& str);
  bool ProcessRedirectGet(const QString& str);
  bool GetPostId(const QByteArray& arr);
  int ProcessRedirectLoginGet(const QString& str);
  bool ProcessRedirectLoginGet2(const QString& str);
  int ProcessRedirectLoginGetTemp(const QString& str);
  int ProcessRedirectLoginGetTemp2(const QString& str);
  bool GetFatieStatus(const QByteArray& byte_arr);
  void FatieSucceed(const QString& comment,const QString& id);
  bool NeedValidateCode(const QString& name, QString& vcode, QString& code_sign);

  void WaitforSeconds(int nseconds);
  bool GetWendaToken(QString& wenda_token);
private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;
    QString m_client_id;
    QString m_post_id;
    QString m_csrf_token;
    bool m_code_online;
private:
    QString m_url;
    QString m_news_id;
    QString m_group_id;
    int m_interval;
    QStringList m_comment_list;
    QList<QListWidgetItem*> m_comment_item_list;
    AccountList m_account_list;
    QList<int> m_account_row_list;
    int m_account_order;
};

#endif // AUTOBOTS_TOUTIAO_H
