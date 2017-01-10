#ifndef NETEASE_ENCRYPT_H
#define NETEASE_ENCRYPT_H

//#include "sina_encrypt_global.h"
#include <QString>

class QScriptEngine;

class netease_encrypt 
{
public:
    
    ~netease_encrypt();

    static netease_encrypt* GetInstance();

    // 加密用户名
    //QString EncryptName(const QString& name);

    // 加密密码 
    QString EncryptPassword(const QString& password);
    
private:
    netease_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js文件全名
  QString m_js_content; // js内容
};

#endif // NETEASE_ENCRYPT_H
