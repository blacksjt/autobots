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

    // 加密用户名
    //QString EncryptName(const QString& name);

    // 加密密码 
    QString EncryptPassword(const QString& password, const QString& public_key, 
      const QString& hash_code);
    
private:
    pwdEncrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js文件全名
  QString m_js_content; // js内容
};

#endif // SINA_ENCRYPT_H
