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

  void onClientIdReday(QString _id);
  void onVotesucceed();
  void onResultChanged(double count);

private:
  int LogInbySina();
  //QNetworkReply* LogInbyRenren(const QString& name, const QString& password);
  int DoAction();
  void GetUerInfo();
  void GetContent();
  //void LoginRequest();
  void UpdateData();
  void initialize();
  void Logout();

  void AddToTree(const QString& text);
  void GetVoteStatus();
  QString GetValidateCode(QString& pid);
  void SubmitVote(const QString& str, const QString& pid);
  QString GetCookieString();
private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;
    QString m_client_id;
    QString m_post_id;
    QString m_csrf_token;
    bool m_doVote;

private:
  bool m_b_csrf; // m_b_csrf״̬
  bool m_b_post_id; // m_post_id״̬
  bool m_code_by_manual;  
private:
    QString m_url;
    QString m_group_id;
    //QStringList m_comment_list;
    QString m_vote_id;
    QString m_vote_order; // 投第几个
    AccountList m_account_list;
};

#endif // AUTOBOTS_TOUTIAO_H
