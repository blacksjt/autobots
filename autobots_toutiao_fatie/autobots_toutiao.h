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
  void onPostIdReday(QString);
  void onAddCommentID();
  void onCsrfTokenReday(QString str);
  void onLoginsucceed(QString id);
  void onActFromTxt();
  void onActImportComment();
  void onActClearComments();
  void onActClearAccounts();
  void onClientIdReday(QString _id);
  void onFatieSucceed(QString comment, double id);
private:
  int LogInbySina();
  QNetworkReply* LogInbyRenren(const QString& name, const QString& password);
  int DoPostFatie();
  void GetUerInfo();
  QNetworkReply* GetContent();
  void GetConnection();
  void LoginRequest();
  QNetworkReply* RequestForRenren();
  void CodeCheckForRenren();
  QNetworkReply* AuthorByRenren(const QString& name, const QString& password);
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();

private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;
    QString m_client_id;
    QString m_post_id;
    QString m_csrf_token;

private:
   bool m_b_csrf; // m_b_csrf״̬
   bool m_b_post_id; // m_post_id״̬
private:
    QString m_url;
    QString m_news_id;
    QString m_group_id;
    QStringList m_comment_list;
    AccountList m_account_list;
    int m_comment_order;
    int m_account_order;
};

#endif // AUTOBOTS_TOUTIAO_H
