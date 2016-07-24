#ifndef REDIAL_DIAG_H
#define REDIAL_DIAG_H

#include <QtWidgets/QMainWindow>
#include "ui_redial_diag.h"
#define WINVER 0x0501
class redial_diag : public QMainWindow
{
    Q_OBJECT

public:
    redial_diag(QWidget *parent = 0);
    ~redial_diag();

private slots:
    void onManual();
    void onAuto();
    void onStopAuto();
	void onChanged(const QString&);
private:
    bool DisConnect();
    void switchIP();
    void updateData();
    void updataMsg(const QString&);
    bool ReConnect();
    bool isConnected();
	void intializeControl();
	
private:
    Ui::redial_diagClass ui;
    bool bAuto ;

private:
  QString mName; // 宽带连接名
  QString mAccount;  // 账号名
  QString mPassword;  // 账号密码
  
};

#endif // REDIAL_DIAG_H
