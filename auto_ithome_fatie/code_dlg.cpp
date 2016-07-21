#include "code_dlg.h"

ValidateDlg::ValidateDlg()
{
  setupUi(this);

  

  connect(pushButton_confirm, SIGNAL(clicked()), this, SLOT(onConfirm()));
  connect(pushButton_cancel, SIGNAL(clicked()), this, SLOT(onCancel()));
}

ValidateDlg::ValidateDlg(QWidget *parent,const QImage& img)
  :QDialog(parent)
{
  setupUi(this);
  connect(pushButton_confirm, SIGNAL(clicked()), this, SLOT(onConfirm()));
  connect(pushButton_cancel, SIGNAL(clicked()), this, SLOT(onCancel()));

  label_code->setPixmap(QPixmap::fromImage(img));
}


void ValidateDlg::onConfirm()
{
  mCode = lineEdit_code->text();
  this->accept();
}

void ValidateDlg::onCancel()
{
  this->reject();
}

ValidateDlg::~ValidateDlg()
{

}


