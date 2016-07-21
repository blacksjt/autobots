#ifndef AUTOBOTS_TOUTIAO_H
#define AUTOBOTS_TOUTIAO_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include "ui_auto_ithome.h"
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

class auto_ithome : public QMainWindow
{
    Q_OBJECT

public:
    auto_ithome(QWidget *parent = 0);
    ~auto_ithome();

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
  //void onDiggsucceed(double, int);

private:
  //int LogInbySina();
  bool DoAction();
  void GetUerInfo();
  bool GetContent();
  void GetConnection();
  //void LoginRequest();
  bool RequestForLogin();
  //void CodeCheckForRenren();
  bool Login(const QString& name, const QString& password);
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();
  void AddToTree(const QString& text);
  bool UpdateDiggCount(const QByteArray& data, int&);
  bool GetLoginResult(const QByteArray& data);
  //bool GetCsrfToken(const QByteArray& arr);
  //bool ProcessRedirectSSL(const QString& str);
  //bool ProcessRedirectGet(const QString& str);
  //bool GetPostId(const QByteArray& arr);
  //bool ProcessRedirectLoginGet(const QString& str);
  //bool ProcessRedirectLoginGet2(const QString& str);
  //bool NeedValidateCode(const QString& name, QString& vcode);

private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;


private:

private:
    QString m_url;
    //QString m_group_id;
    QString m_host;
    QStringList m_comment_list;
    QList<QTreeWidgetItem*> m_comment_item_list;
    AccountList m_account_list;
    QList<int> m_account_row_list;
};

#endif // AUTOBOTS_TOUTIAO_H
