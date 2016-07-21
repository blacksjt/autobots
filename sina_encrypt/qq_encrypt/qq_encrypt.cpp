#include "qq_encrypt.h"
#include "QFile"
#include <QtScript/QScriptEngine>
#include "QTextStream"
#include <QtWidgets/QMessageBox>
#include "QCoreApplication"
#include "QSslSocket"
#include <QDir>

qq_encrypt::qq_encrypt()
{
  initializeContext();
}

qq_encrypt::~qq_encrypt()
{

}

QString qq_encrypt::EncryptPassword(const QString& password, const QString& salt, 
  const QString& verify_code)
{
  QScriptValue global_ob = m_engine->globalObject();
  QScriptValue func = global_ob.property("GetPwd");
//  QScriptValue v = m_engine->evaluate("$.Encryption.getEncryption('a123456', 'd34@', 'xwkp')");
   QScriptValueList args;  
   args << password << salt << verify_code;  
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

void qq_encrypt::initializeContext()
{
  m_engine = new QScriptEngine();

  QString PATH2 = QCoreApplication::applicationDirPath(); 

  // 改为相对路径
  m_file_name = PATH2 + "/" + "qq_encrypt.js";
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

qq_encrypt* qq_encrypt::GetInstance()
{
  static qq_encrypt* s_instance = new qq_encrypt();
  return s_instance;
}
