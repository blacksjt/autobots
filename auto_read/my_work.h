#ifndef MY_WORK_HHHHHH
#define MY_WORK_HHHHHH

#include <QtCore>
#include <QtNetwork>

class MyWork : public QObject
{
  Q_OBJECT
public:
  MyWork();
  MyWork(const QStringList& commentlist);
  ~MyWork();

  inline void SetParameters(const QStringList& url_list)
  {
    m_url_list = url_list;
  }

private:
  void MyWork::work_run();
  void waitForSeconds(int n);

  public slots:
    void onWorkStart();
    void onWorkStop();
    QString GetHost(const QString& str);

private:
  bool m_b_run;
  QNetworkAccessManager m_manager;
//   QString m_url;  // URL
//   QString m_referer; // referer
//   QString m_news_id;  // ÐÂÎÅID
  QStringList m_url_list;  // ÆÀÂÛID
};


#endif //MY_WORK_HHHHHH