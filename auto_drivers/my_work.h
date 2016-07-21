#ifndef MY_WORK_HHHHHH
#define MY_WORK_HHHHHH

#include <QtCore>
#include <QtNetwork>

class MyWork : public QObject
{
  Q_OBJECT
public:
  MyWork();
  MyWork(const QString& url, const QString& ref, 
    const QString& id, const QStringList& commentlist);
  ~MyWork();

  inline void SetParameters(const QString& url, const QString& ref, 
    const QString& id, const QStringList& commentlist)
  {
    m_url = url;
    m_referer =ref;
    m_news_id = id;
    m_comment_list = commentlist;
  }

private:
  void MyWork::work_run();
  void waitForSeconds(int n);

  public slots:
    void onWorkStart();
    void onWorkStop();
    
private:
  bool m_b_run;
  QNetworkAccessManager m_manager;
  QString m_url;  // URL
  QString m_referer; // referer
  QString m_news_id;  // ÐÂÎÅID
  QStringList m_comment_list;  // ÆÀÂÛID
};


#endif //MY_WORK_HHHHHH