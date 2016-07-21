#ifndef QQ_ENCRYPT_H
#define QQ_ENCRYPT_H

//#include "qq_encrypt_global.h"
#include <QString>

class QScriptEngine;

class pwdEncrypt 
{
public:
    
    ~pwdEncrypt();

    static pwdEncrypt* GetInstance();

    // �����û���
    //QString EncryptName(const QString& name);

    // �������� 
    QString EncryptPassword(const QString& password, const QString& public_key, 
      const QString& hash_code);
    
private:
    pwdEncrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js�ļ�ȫ��
  QString m_js_content; // js����
};

#endif // SINA_ENCRYPT_H
