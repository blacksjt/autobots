#ifndef AUTO_SMZDM_H
#define AUTO_SMZDM_H

#include <QtWidgets/QMainWindow>
#include "ui_auto_smzdm.h"
#include <QtNetwork>
#include "../http_base/http_base.h"

class smzdm_network : public HttpBase
{
  Q_OBJECT
public:
  smzdm_network(QObject* parent = 0);
  ~smzdm_network();

protected:
  virtual void ProcessReplyData(QNetworkReply* reply);

protected slots:
    virtual void OnAuthenticationRequired(QNetworkReply* reply, QAuthenticator* authenticator);
    virtual void OnSsllErrors(QNetworkReply* reply, const QList<QSslError>& errors);

private:
};

class auto_smzdm : public QMainWindow
{
    Q_OBJECT

public:
    auto_smzdm(QWidget *parent = 0);
    ~auto_smzdm();

    private slots:
      void onStart();
      void onPause();
      int smzdm_run();
      void onOneClick();
      void onActClearComments();
      void onActImportComment();
      void onAddCommentID();
private:
  bool CheckInput();
  void UpdateData();
  
private:
    Ui::auto_smzdmClass ui;
    bool control_status;
    smzdm_network network;
     
    //QString m_url;  // URL
    QString m_host;
    QString m_referer; // referer
    //QString m_news_id;  // ÐÂÎÅID
    QStringList m_comment_list;  // ÆÀÂÛID
};

#endif // AUTO_SMZDM_H
