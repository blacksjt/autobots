/********************************************************************************
** Form generated from reading UI file 'code.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_CODE_H
#define UI_CODE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>

QT_BEGIN_NAMESPACE

class Ui_Dialog_Vlidate
{
public:
    QGridLayout *gridLayout;
    QLabel *label_code;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_code;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton_confirm;
    QPushButton *pushButton_cancel;

    void setupUi(QDialog *Dialog_Vlidate)
    {
        if (Dialog_Vlidate->objectName().isEmpty())
            Dialog_Vlidate->setObjectName(QStringLiteral("Dialog_Vlidate"));
        Dialog_Vlidate->resize(280, 186);
        gridLayout = new QGridLayout(Dialog_Vlidate);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        label_code = new QLabel(Dialog_Vlidate);
        label_code->setObjectName(QStringLiteral("label_code"));

        gridLayout->addWidget(label_code, 0, 0, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setSizeConstraint(QLayout::SetMinimumSize);
        label = new QLabel(Dialog_Vlidate);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit_code = new QLineEdit(Dialog_Vlidate);
        lineEdit_code->setObjectName(QStringLiteral("lineEdit_code"));

        horizontalLayout_2->addWidget(lineEdit_code);


        gridLayout->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        pushButton_confirm = new QPushButton(Dialog_Vlidate);
        pushButton_confirm->setObjectName(QStringLiteral("pushButton_confirm"));

        horizontalLayout->addWidget(pushButton_confirm);

        pushButton_cancel = new QPushButton(Dialog_Vlidate);
        pushButton_cancel->setObjectName(QStringLiteral("pushButton_cancel"));

        horizontalLayout->addWidget(pushButton_cancel);


        gridLayout->addLayout(horizontalLayout, 2, 0, 1, 1);


        retranslateUi(Dialog_Vlidate);

        QMetaObject::connectSlotsByName(Dialog_Vlidate);
    } // setupUi

    void retranslateUi(QDialog *Dialog_Vlidate)
    {
        Dialog_Vlidate->setWindowTitle(QApplication::translate("Dialog_Vlidate", "\351\252\214\350\257\201\347\240\201\350\257\206\345\210\253", 0));
        label_code->setText(QString());
        label->setText(QApplication::translate("Dialog_Vlidate", "\351\252\214\350\257\201\347\240\201", 0));
        pushButton_confirm->setText(QApplication::translate("Dialog_Vlidate", "\347\241\256\345\256\232", 0));
        pushButton_cancel->setText(QApplication::translate("Dialog_Vlidate", "\345\217\226\346\266\210", 0));
    } // retranslateUi

};

namespace Ui {
    class Dialog_Vlidate: public Ui_Dialog_Vlidate {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_CODE_H
