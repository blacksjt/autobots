#include "vlidatecodeonline.h"

typedef char* (*lpFun)(const char* strPath,const char* strUser, const char* strPass);
typedef char* (*lprecbytefun)(unsigned char* byte,int len,const char* strUser, const char* strPass);
typedef char* (*lprecbytefunA)(unsigned char* byte,int len,const char* strUser, const char* strPass,const char* author);
typedef void (*reporterrfun)(const char* strUser,const char*  strDaMaworker);   //�������

VlidateCodeOnLine::VlidateCodeOnLine()
{
  m_library.setFileName("FastVerCode.dll");
  m_library.load();
  GetRecFun();
}

VlidateCodeOnLine::~VlidateCodeOnLine()
{

}

// QString VlidateCodeOnLine::GetRecResults(const QString& path, const QString& user_name, const QString& password , const QString& author_account)
// {
// //   lpFun pRec_fun = m_library.resolve("RecYZM_A");
// // 
// //   pRec_fun(path.da);
// 
//   return "";
// }

int VlidateCodeOnLine::GetRecResults(const QByteArray& data, const QString& user_name, 
  const QString& password , const QString& author_account, QString& vcode, QString& code_sign)
{
  //lprecbytefunA pRec_fun = (lprecbytefunA)m_library.resolve("RecByte_A");
  if (m_lprecbytefun != NULL)
  {
    char* t = ((lprecbytefunA)m_lprecbytefun)((unsigned char*)(data.data()), data.length(), 
      (const char*)user_name.toLatin1().data(), 
      (const char*)password.toLatin1().data(),
      (const char*)author_account.toLatin1().data());
    QString temp = QString::fromLocal8Bit(t);
    return AnalysisResult(temp, vcode, code_sign);
  }
  return 9;
}



int VlidateCodeOnLine::AnalysisResult(const QString& temp, QString& vcode, QString& code_sign)
{
// �ɹ�����->��֤����|!|��֤���ʶ
  // ��̨û�����˷���:No Money!
// δע�᷵��:No Reg!
// �ϴ���֤��ʧ��:Error:Put Fail!  
// ʶ��ʱ��:Error:TimeOut!   
// �ϴ���Ч��֤��:Error:empty picture! 
// ����ip:  �ϴ���ip��������ip��һ��!
// ��������:No Money!

  if (temp.contains("|!|"))
  {
    int pos = temp.indexOf("|!|");
    vcode = temp.left(pos);
    pos = temp.lastIndexOf("|");
    code_sign = temp.right(temp.length() - pos - 1);
    return 0;
  }

  if (temp == "No Money!")
  {
    return 1;
  }
  else if(temp == "No Reg!")
  {
    return 2;
  }
  else if(temp == "Error:Put Fail!")
  {
    return 3;
  }
  else if(temp == "TimeOut! ")
  {
    return 4;
  }
  else if(temp == "empty picture!")
  {
    return 5;
  }
  else
  {
    return 6;
  }
}

VlidateCodeOnLine* VlidateCodeOnLine::GetInstance()
{
  static VlidateCodeOnLine* s_obj = new VlidateCodeOnLine();
  return s_obj;
}

void VlidateCodeOnLine::ReportError(const QString& user_name, const QString& code_sign)
{
  //reporterrfun pRec_fun = (reporterrfun)m_library.resolve("ReportError");
  if (m_lperrorfun != NULL)
  {
    ((reporterrfun)m_lperrorfun)((const char*)user_name.toLatin1().data(), (const char*)code_sign.toLatin1().data());
  }
}

void VlidateCodeOnLine::GetRecFun()
{
  m_lprecbytefun = (lprecbytefunA)m_library.resolve("RecByte_A");
  m_lperrorfun = (reporterrfun)m_library.resolve("ReportError");
}