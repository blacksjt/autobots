#ifndef SINA_ENCRYPT_H
#define SINA_ENCRYPT_H

#include "sina_encrypt_global.h"
#include <QString>

class QScriptEngine;

class SINA_ENCRYPT_EXPORT sina_encrypt 
{
public:
    
    ~sina_encrypt();

    static sina_encrypt* GetInstance();

    // 加密用户名
    QString EncryptName(const QString& name);

    // 加密密码 
    QString EncryptPassword(const QString& password, const QString& server_time, 
      const QString& nonce, const QString& pubkey);
    
private:
    sina_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js文件全名
  QString m_js_content; // js内容
};

#endif // SINA_ENCRYPT_H
