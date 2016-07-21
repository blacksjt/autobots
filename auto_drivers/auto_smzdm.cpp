#include "auto_smzdm.h"
#include <QString>
#include "QFileDialog"
#include "QMessageBox"
#include "QTextStream"
#include "work_thread.h"
#include "my_work.h"

auto_smzdm::auto_smzdm(QWidget *parent)
    : control_status(true), QMainWindow(parent)
{
    m_thread_count = 1;
    ui.setupUi(this);

    connect(ui.pushButton_start, SIGNAL(clicked()), this, SLOT(onStart()));

    connect(ui.pushButton_pause, SIGNAL(clicked()), this, SLOT(onPause()));

    connect(ui.pushButton_add_comment_id, SIGNAL(clicked()), this, SLOT(onAddCommentID()));

    connect(ui.pushButton_clickone, SIGNAL(clicked()), this, SLOT(onOneClick()));

    connect(ui.action_import_commentid, SIGNAL(triggered()), this, SLOT(onActImportComment()));

    connect(ui.action_clear_comments, SIGNAL(triggered()), this, SLOT(onActClearComments()));
}

auto_smzdm::~auto_smzdm()
{
//   for (int i = 0; i < m_work_list.size(); ++i)
//   {
//     if (m_work_list[i] != NULL)
//     {
//       m_work_list[i]->deleteLater();
//       m_work_list[i] = NULL;
//     }
//   }

  for (int i = 0; i < m_thread_list.size(); ++i)
  {
    if (m_thread_list[i] != NULL)
    {
      m_thread_list[i]->disconnect();
      m_thread_list[i]->terminate();
      m_thread_list[i]->deleteLater();
      m_thread_list[i] = NULL;
    }
  }

  //m_work_list.clear();
  m_thread_list.clear();
}

void auto_smzdm::onStart()
{
  if (!CheckInput())
  {
    return;
  }

  control_status = true;
  ui.lineEdit_msg->setText(QStringLiteral("运行中"));

  UpdateData();

//   for (int i = 0 ; i < m_thread_count ; ++i)
//   {
//     MyWork* work = new MyWork(m_url, m_referer, m_news_id, m_comment_list);
//     WorkThread* thread_ = new WorkThread(this);
//     // 开启线程进行工作
//     
//     work->moveToThread(thread_);
//     qDebug() << QThread::currentThreadId();
//     connect(thread_, SIGNAL(started()), work, SLOT(onWorkStart()));
//     //connect(mthread, SIGNAL(finished()), m_work, SLOT(onWorkStop()));
//     m_work_list.push_back(work);
//     m_thread_list.push_back(thread_);
//   }
  qDebug() << "main_id" << QThread::currentThreadId();
  for (int i = 0 ; i < m_thread_count ; ++i)
  {
    //MyWork* work = new MyWork(m_url, m_referer, m_news_id, m_comment_list);
    WorkThread* thread_ = new WorkThread(this);
    connect(thread_, SIGNAL(emitMsg(const QString&)), this, SLOT(onMsg(const QString&)));
    thread_->SetParameters(m_url,m_referer,m_news_id,m_comment_list);
    m_thread_list.push_back(thread_);
  }


  for (int i = 0; i < m_thread_count; ++i)
  {
    m_thread_list[i]->start();
  }
}

void auto_smzdm::onPause()
{
//   for (int i = 0; i < m_work_list.size(); ++i)
//   {
//     if (m_work_list[i] != NULL)
//     {
//       m_work_list[i]->deleteLater();
//       m_work_list[i] = NULL;
//     }
//   }

  for (int i = 0; i < m_thread_list.size(); ++i)
  {
    if (m_thread_list[i] != NULL)
    {
      m_thread_list[i]->disconnect();
      m_thread_list[i]->terminate();
      m_thread_list[i]->deleteLater();
      m_thread_list[i] = NULL;
    }
  }

//  m_work_list.clear();
  m_thread_list.clear();

  //control_status = false;
  ui.lineEdit_msg->setText(QStringLiteral("已停止"));
}


void auto_smzdm::onOneClick()
{
  UpdateData();
//  smzdm_run();
}

void auto_smzdm::onActClearComments()
{
  ui.listWidget_comment_id->clear();
}

void auto_smzdm::onActImportComment()
{
  QString path = QFileDialog::getOpenFileName(this, QStringLiteral("请选择文件路径"), "", QStringLiteral("Txt(*.txt)")); 
  if (path.isEmpty())  //判断用户是否选择了文件
  {
    //emitLogMessage(QStringLiteral("取消打开文件"));
    return;
  }

  QFile file(path);

  if (!file.open(QIODevice::ReadOnly)) {    
    QMessageBox::critical(NULL,  QStringLiteral("提示"), QStringLiteral("无法打开文件"));  
    return ;    
  }  

  QTextStream text_stream(&file);
  //QString str = text_stream.readAll();
  int n_row = 0;
  ui.listWidget_comment_id->clear();

  while(!text_stream.atEnd())
  {
    QString line_str = text_stream.readLine();
    ui.listWidget_comment_id->addItem(new QListWidgetItem(line_str));
  }

  file.close();
}

void auto_smzdm::UpdateData()
{
   m_url = ui.lineEdit_URL1->text();
   m_referer = ui.lineEdit_REFERER->text();
   m_news_id = ui.lineEdit_news_id->text();
   m_comment_list.clear();
   m_thread_count = ui.spinBox_thread_count->value();
 
  int n = ui.listWidget_comment_id->count();

  for (int i = 0 ; i < n; ++i)
  {
    QString text = ui.listWidget_comment_id->item(i)->text();
    if (!text.isEmpty())
    {
      m_comment_list.push_back(text);
    } 
  }
}

bool auto_smzdm::CheckInput()
{
  if (ui.lineEdit_URL1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("URL没有输入")); 
    return false;
  }

  if (ui.lineEdit_HOST1->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("HOST没有输入")); 
    return false;
  }

  if (ui.lineEdit_REFERER->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("Referer没有输入")); 
    return false;
  }

  if (ui.lineEdit_news_id->text().isEmpty())
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("新闻ID没有输入")); 
    return false;
  }

  if (ui.listWidget_comment_id->count() < 1)
  {
    QMessageBox::critical(this, QStringLiteral("提示"), QStringLiteral("没有评论的ID")); 
    return false;
  }

  return true;
}

void auto_smzdm::onAddCommentID()
{

  QString id = ui.lineEdit_comment_id->text();

  ui.lineEdit_comment_id->setText("");

  ui.listWidget_comment_id->addItem(id);

}

void auto_smzdm::onMsg(const QString& msg)
{
  ui.lineEdit_msg->setText(msg);
}
