#include "redial_diag.h"
#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QElapsedTimer>
#include <windows.h>
#include <ras.h>
#include "QSettings"
#include "QFile"


#pragma comment(lib, "Rasapi32.lib")

redial_diag::redial_diag(QWidget *parent)
    : QMainWindow(parent)
{
  ui.setupUi(this);
  bAuto =false;

//   QRegExp regx("[0-9]+$");
//   QValidator *validator = new QRegExpValidator(regx, ui.lineEdit_time_interval );
//   ui.lineEdit_time_interval->setValidator( validator );

  connect(ui.pushButton_manul_switch, SIGNAL(clicked()), this, SLOT(onManual()));

  connect(ui.pushButton_start_auto, SIGNAL(clicked()), this, SLOT(onAuto()));

  connect(ui.pushButton_stop_atuoridal, SIGNAL(clicked()), this, SLOT(onStopAuto()));

  connect(ui.comboBox_name, SIGNAL(currentIndexChanged(const QString& )), this, SLOT(onChanged(const QString&)));

  intializeControl();
}

redial_diag::~redial_diag()
{

}

void redial_diag::onManual()
{
  if (ui.comboBox_name->currentText().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带名称"));
    QMessageBox::critical(this, "warning", QStringLiteral("拨号名称未设置"));
    return;
  }

  if (ui.lineEdit_account->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带账号"));
    QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带账号"));
    return;
  }

  if (ui.lineEdit_password->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带密码"));
    QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带密码"));
    return;
  }
  updateData();
  switchIP();
}

void redial_diag::onAuto()
{
  if (ui.comboBox_name->currentText().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带名称"));
    QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带名称"));
    return;
  }

  if (ui.lineEdit_account->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带账号"));
    QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带账号"));
    return;
  }

  if (ui.lineEdit_password->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("请输入宽带密码"));
    QMessageBox::critical(this, "warning", QStringLiteral("请输入宽带密码"));
    return;
  }

  bAuto = true;
  updateData();
  //QString interval = ui.spinBox_interval->value();
  int i_interval = ui.spinBox_interval->value();
  

  while(bAuto)
  {
    switchIP();
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<i_interval*1000)  
      QCoreApplication::processEvents();
  }
}

void redial_diag::onStopAuto()
{
  bAuto = false;
  //ui.lineEdit_msg->setText();
  DisConnect();
  updataMsg(QStringLiteral("已停止"));
}

void redial_diag::switchIP()
{
  // 1. 判断当前是否有已有连接，如果有，则关闭
  updataMsg(QStringLiteral("准备开始"));
  DisConnect();

  while (isConnected())
  {
    DisConnect();
    Sleep(3000);
  }

  updataMsg(QStringLiteral("准备拨号"));
  // 2. 从新拨号
  bool res = ReConnect();

  // 3. 判断拨号状态
  
}

// void redial_diag::switchIP()
// {
//   //执行完即返回0  
//   //int res = system("Rasdial ADSL /DISCONNECT"); 
//   QString cmd1 = "cmd /c Rasdial "+ mName + " /DISCONNECT";
//   int res = WinExec(cmd1.toUtf8().data(), SW_HIDE);
//   //qDebug() << "Disconnect -" << (res == 0 ? "succeed" : "failed"  );
//   QString msg = (res > 31 ? QStringLiteral("成功断开") : QStringLiteral("断开失败"));
//   ui.lineEdit_msg->setText(msg);
// 
//   QElapsedTimer t;
//   t.start();
//   while(t.elapsed()<2000)  
//     QCoreApplication::processEvents();
// 
//   QString cmd2 = "cmd /c Rasdial " + mName + " " 
//     + mAccount + " " + mPassword; 
// 
//   //成功连接返回0 其他为<span style="color:#ff0000">错误码</span>  
//   //res = system(cmd.toLatin1().data());
// 
//   res = WinExec(cmd2.toUtf8().data(), SW_HIDE);
// 
//   msg = (res > 31 ? QStringLiteral("更换成功") : QStringLiteral("更换失败"));
//   ui.lineEdit_msg->setText(msg);
// }

void redial_diag::updateData()
{
  mAccount = ui.lineEdit_account->text();
  mPassword = ui.lineEdit_password->text();
  mName = ui.comboBox_name->currentText();
}

bool redial_diag::DisConnect()
{
  updataMsg(QStringLiteral("正在检查已有连接"));
  // 通过枚举获取当前连接的ADSL的句柄
  RASCONN rasconnArr[20]; // 存放多个结构体
  DWORD dwRasConSize =  20* sizeof(RASCONN);
  DWORD dwRasConCount = 0;
  rasconnArr[0].dwSize = sizeof (RASCONN);

  if (!RasEnumConnections(rasconnArr, &dwRasConSize, &dwRasConCount)) 
  {
    for (int i =0; i< (int)dwRasConCount; ++i) 
    {
      const wchar_t* temp_ = reinterpret_cast<const wchar_t *>(mName.utf16());
      if (wcscmp(rasconnArr[i].szEntryName, temp_) == 0)
      {
        updataMsg(QStringLiteral("正在挂断已有连接"));
        DWORD ret = RasHangUp(rasconnArr[i].hrasconn);
        if (ret == ERROR_SUCCESS)
        {
          updataMsg(QStringLiteral("已有连接已挂断"));
        }
        Sleep(1000);
        return true;
      }
    }
  }

  updataMsg(QStringLiteral("没有已有连接"));
  return false;
}

bool redial_diag::isConnected()
{
  // 通过枚举获取当前连接的ADSL的句柄
  RASCONN rasconnArr[20]; // 存放多个结构体
  DWORD dwRasConSize =  20* sizeof(RASCONN);
  DWORD dwRasConCount = 0;
  rasconnArr[0].dwSize = sizeof (RASCONN);

  if (!RasEnumConnections(rasconnArr, &dwRasConSize, &dwRasConCount)) 
  {
    for (int i =0; i< (int)dwRasConCount; ++i) 
    {
      const wchar_t* temp_ = reinterpret_cast<const wchar_t *>(mName.utf16());
      if (wcscmp(rasconnArr[i].szEntryName, temp_) == 0)
      {
        return true;
      }
    }
  }
  return false;
}


void redial_diag::intializeControl()
{
	// 读取配置文件
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("缺少ini配置文件"));
		return;
	}
	
	QSettings setting(inipath, QSettings::IniFormat);
	QStringList groups = setting.childGroups();
	ui.comboBox_name->addItems(groups);

	// 设置默认值
	if (groups.isEmpty())
	{
		QMessageBox::critical(this, "warning", QStringLiteral("配置文件中没有有效值"));
		return;
	}

	int ndefault = setting.value("default").toInt();

	if (ndefault >= groups.size()  || ndefault < 0)
	{
		ndefault = 0;
	}

	QString first = groups.at(ndefault);
	ui.comboBox_name->setCurrentText(first);
	setting.beginGroup(first);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}

void redial_diag::onChanged(const QString& text)
{
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("缺少ini配置文件"));
		return;
	}

	QSettings setting(inipath, QSettings::IniFormat);

	setting.beginGroup(text);
	QString account = setting.value("name").toString();
	QString pwd = setting.value("password").toString();
	ui.lineEdit_account->setText(account);
	ui.lineEdit_password->setText(pwd);
	setting.endGroup();
}

void redial_diag::updataMsg(const QString& msg)
{
  //ui.lineEdit_msg->setText(msg);
  ui.statusBar->showMessage(msg);
}

bool redial_diag::ReConnect()
{
  updataMsg(QStringLiteral("正在拨号"));

  RASDIALPARAMS rasParam;
  ZeroMemory(&rasParam, sizeof(RASDIALPARAMS));
  rasParam.dwSize = sizeof (RASDIALPARAMS);

  wchar_t connectName[NNLEN+1] = L"", userName[UNLEN + 1] = L"", userPass[PWLEN+1] = L"";
  mName.toWCharArray(connectName);
  mAccount.toWCharArray(userName);
  mPassword.toWCharArray(userPass);
  wcscpy(rasParam.szEntryName, connectName );
  wcscpy(rasParam.szUserName , userName);
  wcscpy(rasParam.szPassword , userPass);

  HRASCONN handle = NULL; // 注意，记得初始化。
  int res = RasDial(0,0, &rasParam, 0, 0, &handle);

  if (res != ERROR_SUCCESS) 
  {
    updataMsg(QStringLiteral("拨号失败:") + QString::number(res));
    //qDebug()<<"dial error:"<< res;

    // 出错 采用断开连接的方法尝试再次连接
    // 前提需要保障用户账号名和密码是正确的
    RasHangUp (handle);
    return false; // 返回错误 待会再连接
  }

  updataMsg(QStringLiteral("拨号成功"));
  return true;
}
