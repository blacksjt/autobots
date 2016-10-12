#ifndef AUTOBOTS_TOUTIAO_H
#define AUTOBOTS_TOUTIAO_H

#include <QtWidgets/QMainWindow>
#include <QtNetwork>
#include <QMap>
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
  //void onDiggsucceed(double, int);
  void onComputeNumber();
  void onCountAll();//计算总点赞数量
private:
  int LogInbySina();
  bool DoAction();
  void GetUerInfo();
  bool GetContent();
  void GetConnection();
  //void LoginRequest();
  bool RequestForRenren();
  //void CodeCheckForRenren();
  bool AuthorByRenren(const QString& name, const QString& password);
  void UpdateData();
  void initialize();
  void Logout();
  bool CheckInput();
  void AddToTree(const QString& text);
  bool UpdateDiggCount(const QByteArray& data, int&);

  bool GetCsrfToken(const QByteArray& arr);
  bool ProcessRedirectSSL(const QString& str);
  bool ProcessRedirectGet(const QString& str);
  bool GetPostId(const QByteArray& arr);
  int ProcessRedirectLoginGet(const QString& str);
  bool ProcessRedirectLoginGet2(const QString& str);
  bool NeedValidateCode(const QString& name, QString& vcode, QString& code_sign);
  int GetRandom();
  
  int ProcessRedirectLoginGetTemp(const QString& str);
  int ProcessRedirectLoginGetTemp2(const QString& str);
  void WaitforSeconds(int nseconds);
  bool GetDongtaiIDMap();

  bool ExactComments(const QByteArray & rp_data, bool& has_more);
  void GetIDList(bool& has_more, int offset);
private:
    Ui::autobots_toutiaoClass ui;
    toutiao_network network;
    bool control_status ;
    QString m_client_id;
    QString m_post_id;
    QString m_csrf_token;
    //bool m_bDoAction;
    bool m_code_online;
private:
	QMap<QString, QString> m_id_dongtaiid;
private:
    QString m_url;
    QString m_group_id;
	QString m_item_id;
    QList<QTreeWidgetItem*> m_comment_list;
    AccountList m_account_list;
    //QList<int> m_account_row_list;
};

#endif // AUTOBOTS_TOUTIAO_H
