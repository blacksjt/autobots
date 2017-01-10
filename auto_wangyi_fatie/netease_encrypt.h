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

    // �����û���
    //QString EncryptName(const QString& name);

    // �������� 
    QString EncryptPassword(const QString& password);
    
private:
    netease_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js�ļ�ȫ��
  QString m_js_content; // js����
};

#endif // NETEASE_ENCRYPT_H
