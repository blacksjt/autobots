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
    //ui.lineEdit_msg->setText(QStringLiteral("������������"));
    QMessageBox::critical(this, "warning", QStringLiteral("��������δ����"));
    return;
  }

  if (ui.lineEdit_account->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("���������˺�"));
    QMessageBox::critical(this, "warning", QStringLiteral("���������˺�"));
    return;
  }

  if (ui.lineEdit_password->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("������������"));
    QMessageBox::critical(this, "warning", QStringLiteral("������������"));
    return;
  }
  updateData();
  switchIP();
}

void redial_diag::onAuto()
{
  if (ui.comboBox_name->currentText().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("������������"));
    QMessageBox::critical(this, "warning", QStringLiteral("������������"));
    return;
  }

  if (ui.lineEdit_account->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("���������˺�"));
    QMessageBox::critical(this, "warning", QStringLiteral("���������˺�"));
    return;
  }

  if (ui.lineEdit_password->text().isEmpty())
  {
    //ui.lineEdit_msg->setText(QStringLiteral("������������"));
    QMessageBox::critical(this, "warning", QStringLiteral("������������"));
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
  updataMsg(QStringLiteral("��ֹͣ"));
}

void redial_diag::switchIP()
{
  // 1. �жϵ�ǰ�Ƿ����������ӣ�����У���ر�
  updataMsg(QStringLiteral("׼����ʼ"));
  DisConnect();

  while (isConnected())
  {
    DisConnect();
    Sleep(3000);
  }

  updataMsg(QStringLiteral("׼������"));
  // 2. ���²���
  bool res = ReConnect();

  // 3. �жϲ���״̬
  
}

// void redial_diag::switchIP()
// {
//   //ִ���꼴����0  
//   //int res = system("Rasdial ADSL /DISCONNECT"); 
//   QString cmd1 = "cmd /c Rasdial "+ mName + " /DISCONNECT";
//   int res = WinExec(cmd1.toUtf8().data(), SW_HIDE);
//   //qDebug() << "Disconnect -" << (res == 0 ? "succeed" : "failed"  );
//   QString msg = (res > 31 ? QStringLiteral("�ɹ��Ͽ�") : QStringLiteral("�Ͽ�ʧ��"));
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
//   //�ɹ����ӷ���0 ����Ϊ<span style="color:#ff0000">������</span>  
//   //res = system(cmd.toLatin1().data());
// 
//   res = WinExec(cmd2.toUtf8().data(), SW_HIDE);
// 
//   msg = (res > 31 ? QStringLiteral("�����ɹ�") : QStringLiteral("����ʧ��"));
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
  updataMsg(QStringLiteral("���ڼ����������"));
  // ͨ��ö�ٻ�ȡ��ǰ���ӵ�ADSL�ľ��
  RASCONN rasconnArr[20]; // ��Ŷ���ṹ��
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
        updataMsg(QStringLiteral("���ڹҶ���������"));
        DWORD ret = RasHangUp(rasconnArr[i].hrasconn);
        if (ret == ERROR_SUCCESS)
        {
          updataMsg(QStringLiteral("���������ѹҶ�"));
        }
        Sleep(1000);
        return true;
      }
    }
  }

  updataMsg(QStringLiteral("û����������"));
  return false;
}

bool redial_diag::isConnected()
{
  // ͨ��ö�ٻ�ȡ��ǰ���ӵ�ADSL�ľ��
  RASCONN rasconnArr[20]; // ��Ŷ���ṹ��
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
	// ��ȡ�����ļ�
	QString path = QCoreApplication::applicationDirPath();
	QString inipath = path + "/redial_setting.ini";

	if (!QFile::exists(inipath))
	{
		QMessageBox::critical(this, "warning", QStringLiteral("ȱ��ini�����ļ�"));
		return;
	}
	
	QSettings setting(inipath, QSettings::IniFormat);
	QStringList groups = setting.childGroups();
	ui.comboBox_name->addItems(groups);

	// ����Ĭ��ֵ
	if (groups.isEmpty())
	{
		QMessageBox::critical(this, "warning", QStringLiteral("�����ļ���û����Чֵ"));
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
		QMessageBox::critical(this, "warning", QStringLiteral("ȱ��ini�����ļ�"));
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
  updataMsg(QStringLiteral("���ڲ���"));

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

  HRASCONN handle = NULL; // ע�⣬�ǵó�ʼ����
  int res = RasDial(0,0, &rasParam, 0, 0, &handle);

  if (res != ERROR_SUCCESS) 
  {
    updataMsg(QStringLiteral("����ʧ��:") + QString::number(res));
    //qDebug()<<"dial error:"<< res;

    // ���� ���öϿ����ӵķ��������ٴ�����
    // ǰ����Ҫ�����û��˺�������������ȷ��
    RasHangUp (handle);
    return false; // ���ش��� ����������
  }

  updataMsg(QStringLiteral("���ųɹ�"));
  return true;
}
