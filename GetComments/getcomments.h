#ifndef GETCOMMENTS_H
#define GETCOMMENTS_H

#include <QtWidgets/QMainWindow>
#include "ui_getcomments.h"


// ������ֻ������toutiao������������ȡ

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
    QString m_id; // ����id
    int m_count;  // ��ȡ����
    QString m_file_path; // Ĭ�ϱ����ļ���λ��

    Ui::GetCommentsClass ui;
    
};

#endif // GETCOMMENTS_H
