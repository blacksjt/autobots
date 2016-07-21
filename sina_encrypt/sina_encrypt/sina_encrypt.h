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

    // �����û���
    QString EncryptName(const QString& name);

    // �������� 
    QString EncryptPassword(const QString& password, const QString& server_time, 
      const QString& nonce, const QString& pubkey);
    
private:
    sina_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js�ļ�ȫ��
  QString m_js_content; // js����
};

#endif // SINA_ENCRYPT_H
