#ifndef QQ_ENCRYPT_H
#define QQ_ENCRYPT_H

#include "qq_encrypt_global.h"
#include <QString>

class QScriptEngine;

class QQ_ENCRYPT_EXPORT qq_encrypt 
{
public:
    
    ~qq_encrypt();

    static qq_encrypt* GetInstance();

    // 加密用户名
    //QString EncryptName(const QString& name);

    // 加密密码 
    QString EncryptPassword(const QString& password, const QString& salt, 
      const QString& verify_code);
    
private:
    qq_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js文件全名
  QString m_js_content; // js内容
};

#endif // SINA_ENCRYPT_H
