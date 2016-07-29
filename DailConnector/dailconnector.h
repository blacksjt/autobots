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

    //�Ͽ�����
    bool DisConnect();

    //��������, ����ɹ�����msg����IP��ַ�����ʧ�ܣ����ش�����Ϣ
    bool Connect(QString& msg);

    // �Ͽ���������
    bool ReConnect(QString& msg);

    //����״̬
    bool IsConnected();

    void SetDialInfo(const QString& dila_name, const QString& user, const QString& pwd);
private:
    QString GetIP(void* handle);
private:
  QString m_dial_name;  // ������ ��ADSL��VPN���������
  QString m_user_name;  // �˻���
  QString m_password;   // ����

};

#endif // DAILCONNECTOR_H
