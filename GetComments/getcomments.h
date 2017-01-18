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
	bool GetSubComments(int& n, QStringList& cmts);
    bool ExactComments(const QByteArray& rp_data, QStringList& cmts);
    bool WriteToFile(const QStringList& comments);
private:
    QString m_group_id; // ����id
	QString m_item_id; // ����id
    int m_count;  // ��ȡ����
    QString m_file_path; // Ĭ�ϱ����ļ���λ��

    Ui::GetCommentsClass ui;
    
};

#endif // GETCOMMENTS_H
