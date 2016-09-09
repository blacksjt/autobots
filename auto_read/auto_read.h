#ifndef AUTO_SMZDM_H
#define AUTO_SMZDM_H

#include <QtWidgets/QMainWindow>
#include "ui_auto_read.h"
#include <QtNetwork>
//class WorkThread;
//class MyWork;
class auto_smzdm : public QMainWindow
{
    Q_OBJECT

public:
    auto_smzdm(QWidget *parent = 0);
    ~auto_smzdm();

    private slots:
      void onStart();
      void onPause();
      //int smzdm_run();
      void onRemoveUrl();
      void onActClearComments();
      void onActImportComment();
      void onAddUrl();
private:
  bool CheckInput();
  void UpdateData();
  public slots:
  void onMsg(const QString&);
  void workRun();
private:
    Ui::auto_smzdmClass ui;
    bool control_status;
    
    //QList<WorkThread*> m_thread_list;
    //QList<MyWork*> m_work_list;
    int m_thread_count;

    //QString m_url;  // URL
    //QString m_referer; // referer
    //QString m_news_id;  // ÐÂÎÅID
    //QStringList m_comment_list;  // ÆÀÂÛID
    QStringList m_url_list;
    //QNetworkAccessManager m_manager;
    
};

#endif // AUTO_SMZDM_H
