#ifndef AUTOBOTS_CODE_DLG_HHH
#define AUTOBOTS_CODE_DLG_HHH

#include <QDialog>
#include "ui_code.h"

class ValidateDlg : public QDialog, public Ui::Dialog_Vlidate
{
  Q_OBJECT
public:
  ValidateDlg(QWidget *parent ,const QImage& img);
  ~ValidateDlg();
  QString getCode(){return mCode;}

private slots:
  void onConfirm();
  void onCancel();

private:
    ValidateDlg();
private:
  QString mCode;
};


#endif // AUTOBOTS_CODE_DLG_HHH