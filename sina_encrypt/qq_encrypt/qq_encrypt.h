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

    // �����û���
    //QString EncryptName(const QString& name);

    // �������� 
    QString EncryptPassword(const QString& password, const QString& salt, 
      const QString& verify_code);
    
private:
    qq_encrypt();
    void initializeContext();

private:
/*  QWebView* m_web;*/
  QScriptEngine* m_engine;  
  QString m_file_name; // js�ļ�ȫ��
  QString m_js_content; // js����
};

#endif // SINA_ENCRYPT_H
