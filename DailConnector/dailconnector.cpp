#include "dailconnector.h"
#include <windows.h>
#include <ras.h>

#pragma comment(lib, "Rasapi32.lib")

DailConnector::DailConnector()
{

}

DailConnector::DailConnector(const QString& dila_name, 
  const QString& user, const QString& pwd)
  : m_dial_name(dila_name), m_user_name(user), m_password(pwd)
{
  
}

DailConnector::~DailConnector()
{

}

bool DailConnector::Connect(QString& msg)
{
  if (m_dial_name.isEmpty() || m_user_name.isEmpty() || m_password.isEmpty())
  {
    msg = QStringLiteral("������Ϣ������������");
    return false;
  }

  RASDIALPARAMS rasParam;
  ZeroMemory(&rasParam, sizeof(RASDIALPARAMS));
  rasParam.dwSize = sizeof (RASDIALPARAMS);

  wchar_t connectName[NNLEN+1] = L"", userName[UNLEN + 1] = L"", userPass[PWLEN+1] = L"";
  m_dial_name.toWCharArray(connectName);
  m_user_name.toWCharArray(userName);
  m_password.toWCharArray(userPass);
  wcscpy(rasParam.szEntryName, connectName );
  wcscpy(rasParam.szUserName , userName);
  wcscpy(rasParam.szPassword , userPass);

  HRASCONN handle = NULL; // ע�⣬�ǵó�ʼ����
  int res = RasDial(0,0, &rasParam, 0, 0, &handle);

  if (res != ERROR_SUCCESS) 
  {
    msg = QStringLiteral("����ʧ�ܣ������:") + QString::number(res);
    //updataMsg(QStringLiteral("����ʧ��:") + QString::number(res));
    //qDebug()<<"dial error:"<< res;

    // ���� ���öϿ����ӵķ��������ٴ�����
    // ǰ����Ҫ�����û��˺�������������ȷ��
    RasHangUp (handle);
    return false; // ���ش��� ����������
  }
  else
  {
    msg = GetIP((void*)handle);
    return true;
  }
}

bool DailConnector::DisConnect()
{
  //updataMsg(QStringLiteral("���ڼ����������"));
  // ͨ��ö�ٻ�ȡ��ǰ���ӵ�ADSL�ľ��
  RASCONN rasconnArr[20]; // ��Ŷ���ṹ��
  DWORD dwRasConSize =  20* sizeof(RASCONN);
  DWORD dwRasConCount = 0;
  rasconnArr[0].dwSize = sizeof (RASCONN);

  if (!RasEnumConnections(rasconnArr, &dwRasConSize, &dwRasConCount)) 
  {
    for (int i =0; i< (int)dwRasConCount; ++i) 
    {
      const wchar_t* temp_ = reinterpret_cast<const wchar_t *>(m_dial_name.utf16());
      if (wcscmp(rasconnArr[i].szEntryName, temp_) == 0)
      {
        //updataMsg(QStringLiteral("���ڹҶ���������"));
        DWORD ret = RasHangUp(rasconnArr[i].hrasconn);
        if (ret == ERROR_SUCCESS)
        {
          //updataMsg(QStringLiteral("���������ѹҶ�"));
        }
        Sleep(1000);
        return true;
      }
    }
  }

  //updataMsg(QStringLiteral("û����������"));
  return false;
}

bool DailConnector::IsConnected()
{
  // ͨ��ö�ٻ�ȡ��ǰ���ӵ�ADSL�ľ��
  RASCONN rasconnArr[20]; // ��Ŷ���ṹ��
  DWORD dwRasConSize =  20* sizeof(RASCONN);
  DWORD dwRasConCount = 0;
  rasconnArr[0].dwSize = sizeof (RASCONN);

  if (!RasEnumConnections(rasconnArr, &dwRasConSize, &dwRasConCount)) 
  {
    for (int i =0; i< (int)dwRasConCount; ++i) 
    {
      const wchar_t* temp_ = reinterpret_cast<const wchar_t *>(m_dial_name.utf16());
      if (wcscmp(rasconnArr[i].szEntryName, temp_) == 0)
      {
        return true;
      }
    }
  }
  return false;
}

void DailConnector::SetDialInfo(const QString& dila_name, const QString& user, 
  const QString& pwd)
{
  m_dial_name = dila_name;
  m_user_name = user;
  m_password = pwd;
}

bool DailConnector::ReConnect(QString& msg)
{
  DisConnect();

  while (IsConnected())
  {
    DisConnect();
    Sleep(2000);
  }

  return Connect(msg);
}

QString DailConnector::GetIP(void* handle)
{
  HRASCONN hr = (HRASCONN)handle;
  RASPPPIP rip;
  DWORD ll,ret;
  ret = RasGetProjectionInfo(hr,RASP_PppIp,&rip,(LPDWORD)(&ll));

  if (ret != 0)
  {
    // ����
    return "";
  }
  else
  {
    QString str = QString::fromWCharArray(rip.szIpAddress);
    return str;
  }
}
