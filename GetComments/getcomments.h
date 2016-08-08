#ifndef GETCOMMENTS_H
#define GETCOMMENTS_H

#include <QtWidgets/QMainWindow>
#include "ui_getcomments.h"


// 本工具只负责处理toutiao的新闻评论提取

class GetComments : public QMainWindow
{
    Q_OBJECT

public:
    GetComments(QWidget *parent = 0);
    ~GetComments();

private slots:
    void onStart();
private:
    void updataData();
    void startWork();
    bool ExactComments(const QByteArray& rp_data);
    bool WriteToFile(const QStringList& comments);
private:
    QString m_id; // 文章id
    int m_count;  // 提取数量
    QString m_file_path; // 默认保存文件的位置

    Ui::GetCommentsClass ui;
    
};

#endif // GETCOMMENTS_H
