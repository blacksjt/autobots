#include "netease_encrypt.h"
#include "QFile"
#include <QtScript/QScriptEngine>
#include "QTextStream"
#include <QtWidgets/QMessageBox>
#include "QCoreApplication"
#include "QSslSocket"
#include <QDir>

netease_encrypt::netease_encrypt()
{
  initializeContext();
}

netease_encrypt::~netease_encrypt()
{

}

/*QString netease_encrypt::EncryptName(const QString& name)
{
  QScriptValue global_ob = m_engine->globalObject();

  QScriptValue fun1 = global_ob.property("GetName");
  QScriptValueList args0;  
  args0<< name ;
  QScriptValue su = fun1.call(QScriptValue(), args0);

  if(su.isError()) {//解析js文件是否有错误  
    QMessageBox::critical(0, "Hello Script",  
      QString::fromLatin1("%0:%1: %2")  
      .arg(m_file_name)//文件名  
      .arg(su.property("lineNumber").toInt32())//错误行号  
      .arg(su.toString()));//错误信息  
    return "";  
  } 

  return su.toString();
}*/

QString netease_encrypt::EncryptPassword(const QString& password)
{
  QScriptValue global_ob = m_engine->globalObject();
  QScriptValue func = global_ob.property("getSP");

  if (!func.isValid())
  {
	  return "";
  }

  QScriptValueList args;  
  args << password ;  
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

void netease_encrypt::initializeContext()
{
  m_engine = new QScriptEngine();

  QString PATH = QDir::currentPath(); 

  QString PATH2 = QCoreApplication::applicationDirPath(); 

  // 改为相对路径
  m_file_name = PATH2 + "/" + "netease.js";
  //m_file_name = "E:/projects/myproject/autobots/autobots/netease_encrypt/netease_encrypt/netease_encrypt.js";

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

netease_encrypt* netease_encrypt::GetInstance()
{
  static netease_encrypt* s_instance = new netease_encrypt();
  return s_instance;
}
