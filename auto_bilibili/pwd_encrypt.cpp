#include "pwd_encrypt.h"
#include "QFile"
#include <QtScript/QScriptEngine>
#include "QTextStream"
#include <QtWidgets/QMessageBox>
#include "QCoreApplication"
#include "QSslSocket"
#include <QDir>

pwdEncrypt::pwdEncrypt()
{
  initializeContext();
}

pwdEncrypt::~pwdEncrypt()
{

}

QString pwdEncrypt::EncryptPassword(const QString& password, const QString& public_key, 
  const QString& hash_code)
{
  QScriptValue global_ob = m_engine->globalObject();
  QScriptValue func = global_ob.property("GetSp");

  QScriptValueList args;  
  args << public_key << hash_code << password;  
  QScriptValue v = func.call(QScriptValue(), args);

  if(v.isError()) {//����js�ļ��Ƿ��д���  
    QMessageBox::critical(0, "Hello Script",  
      QString::fromLatin1("%0:%1: %2")  
      .arg(m_file_name)//�ļ���  
      .arg(v.property("lineNumber").toInt32())//�����к�  
      .arg(v.toString()));//������Ϣ  
    return "";  
  } 

  return v.toString();
}

void pwdEncrypt::initializeContext()
{
  m_engine = new QScriptEngine();

  QString PATH2 = QCoreApplication::applicationDirPath(); 

  // ��Ϊ���·��
  m_file_name = PATH2 + "/" + "jsEncrypt_bili.js";
  //m_file_name = QStringLiteral("I:\\logRecord\\��Ѷ\\qq_encrypt.js");
  //m_file_name = "E:/projects/myproject/autobots/autobots/sina_encrypt/sina_encrypt/sina_encrypt.js";

  QFile scriptFile(m_file_name);  
  //m_file_name = scriptFile.fileName();
  if (!scriptFile.open(QIODevice::ReadOnly))
  {
    QMessageBox::critical(0, "js file error", QStringLiteral("js�ű��ļ�����")+m_file_name);
    return ;
  }
  QTextStream stream(&scriptFile);  
  m_js_content = stream.readAll();  
  scriptFile.close();  

  QScriptValue result = m_engine->evaluate(m_js_content, m_file_name);

  if(result.isError()) {//����js�ļ��Ƿ��д���  
    QMessageBox::critical(0, "Hello Script",  
      QString::fromLatin1("%0:%1: %2")  
      .arg(m_file_name)//�ļ���  
      .arg(result.property("lineNumber").toInt32())//�����к�  
      .arg(result.toString()));//������Ϣ  
    return ;  
  }  
}

pwdEncrypt* pwdEncrypt::GetInstance()
{
  static pwdEncrypt* s_instance = new pwdEncrypt();
  return s_instance;
}
