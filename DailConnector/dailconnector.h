#ifndef DAILCONNECTOR_H
#define DAILCONNECTOR_H

#include "dailconnector_global.h"
#include <QString>

class DAILCONNECTOR_EXPORT DailConnector
{
public:
    DailConnector();
    DailConnector(const QString& dila_name, const QString& user, const QString& pwd);
    ~DailConnector();

    //断开连接
    bool DisConnect();

    //拨号连接, 如果成功，则msg返回IP地址，如果失败，返回错误信息
    bool Connect(QString& msg);

    // 断开重新连接
    bool ReConnect(QString& msg);

    //连接状态
    bool IsConnected();

    void SetDialInfo(const QString& dila_name, const QString& user, const QString& pwd);
private:
    QString GetIP(void* handle);
private:
  QString m_dial_name;  // 拨号名 如ADSL、VPN、宽带连接
  QString m_user_name;  // 账户名
  QString m_password;   // 密码

};

#endif // DAILCONNECTOR_H
