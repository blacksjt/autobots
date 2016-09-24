#ifndef AUTO_SMZDM_H
#define AUTO_SMZDM_H

#include <QtWidgets/QMainWindow>
#include "ui_auto_smzdm.h"
#include <QtNetwork>

class WorkThread;
class auto_smzdm : public QMainWindow
{
    Q_OBJECT

public:
    auto_smzdm(QWidget *parent = 0);
    ~auto_smzdm();

    private slots:
      void onStart();
      void onPause();
      
      void onOneClick();
      void onActClearComments();
      void onActImportComment();
      void onAddCommentID();
      void onMsg(const QString&);
private:
  int smzdm_run();
  bool CheckInput();
  void UpdateData();

private:
    Ui::auto_smzdmClass ui;
    QNetworkAccessManager m_manager;
    WorkThread* mthread;
    QString m_token_url;  // URL
    QString m_referer; // referer
    QString m_host;  // host
    QStringList m_comment_list;  // ÆÀÂÛID
};

#endif // AUTO_SMZDM_H
