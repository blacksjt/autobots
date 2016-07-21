#ifndef VLIDATECODEONLINE_H
#define VLIDATECODEONLINE_H

#include "vlidatecodeonline_global.h"
#include <QLibrary>

class VLIDATECODEONLINE_EXPORT VlidateCodeOnLine
{
public:
    
    ~VlidateCodeOnLine();
private:
  VlidateCodeOnLine();

public:
  static VlidateCodeOnLine* GetInstance();

//   QString GetRecResults(const QString& path, const QString& user_name, const QString& password
//     , const QString& author_account);

  int GetRecResults(const QByteArray& data, const QString& user_name, const QString& password
    , const QString& author_account, QString& vcode, QString& code_sign);

  void ReportError(const QString& user_name, const QString& code_sign);

private:
  int AnalysisResult(const QString& temp, QString& vcode, QString& code_sign);
  void GetRecFun();
private:
  void* m_lprecbytefun;
  void* m_lperrorfun;
  QLibrary m_library;
};

#endif // VLIDATECODEONLINE_H
