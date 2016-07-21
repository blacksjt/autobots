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

  if(v.isError()) {//解析js文件是否有错误  
    QMessageBox::critical(0, "Hello Script",  
      QString::fromLatin1("%0:%1: %2")  
      .arg(m_file_name)//文件名  
      .arg(v.property("lineNumber").toInt32())//错误行号  
      .arg(v.toString()));//错误信息  
    return "";  
  } 

  return v.toString();
}

void pwdEncrypt::initializeContext()
{
  m_engine = new QScriptEngine();

  QString PATH2 = QCoreApplication::applicationDirPath(); 

  // 改为相对路径
  m_file_name = PATH2 + "/" + "jsEncrypt_bili.js";
  //m_file_name = QStringLiteral("I:\\logRecord\\腾讯\\qq_encrypt.js");
  //m_file_name = "E:/projects/myproject/autobots/autobots/sina_encrypt/sina_encrypt/sina_encrypt.js";

  QFile scriptFile(m_file_name);  
  //m_file_name = scriptFile.fileName();
  if (!scriptFile.open(QIODevice::ReadOnly))
  {
    QMessageBox::critical(0, "js file error", QStringLiteral("js脚本文件错误")+m_file_name);
    return ;
  }
  QTextStream stream(&scriptFile);  
  m_js_content = stream.readAll();  
  scriptFile.close();  

  QScriptValue result = m_engine->evaluate(m_js_content, m_file_name);

  if(result.isError()) {//解析js文件是否有错误  
    QMessageBox::critical(0, "Hello Script",  
      QString::fromLatin1("%0:%1: %2")  
      .arg(m_file_name)//文件名  
      .arg(result.property("lineNumber").toInt32())//错误行号  
      .arg(result.toString()));//错误信息  
    return ;  
  }  
}

pwdEncrypt* pwdEncrypt::GetInstance()
{
  static pwdEncrypt* s_instance = new pwdEncrypt();
  return s_instance;
}
