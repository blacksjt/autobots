/********************************************************************************
** Form generated from reading UI file 'auto_smzdm.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTO_SMZDM_H
#define UI_AUTO_SMZDM_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpacerItem>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_auto_smzdmClass
{
public:
    QAction *action_import_commentid;
    QAction *action_clear_comments;
    QWidget *centralWidget;
    QGridLayout *gridLayout_3;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QLineEdit *lineEdit_URL1;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_HOST1;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_3;
    QLineEdit *lineEdit_REFERER;
    QHBoxLayout *horizontalLayout_6;
    QLabel *label_4;
    QLineEdit *lineEdit_news_id;
    QHBoxLayout *horizontalLayout_5;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineEdit_comment_id;
    QPushButton *pushButton_add_comment_id;
    QListWidget *listWidget_comment_id;
    QVBoxLayout *verticalLayout;
    QPushButton *pushButton_clickone;
    QPushButton *pushButton_start;
    QPushButton *pushButton_pause;
    QSpacerItem *verticalSpacer;
    QHBoxLayout *horizontalLayout;
    QLabel *label_1;
    QLineEdit *lineEdit_msg;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *auto_smzdmClass)
    {
        if (auto_smzdmClass->objectName().isEmpty())
            auto_smzdmClass->setObjectName(QStringLiteral("auto_smzdmClass"));
        auto_smzdmClass->resize(393, 529);
        action_import_commentid = new QAction(auto_smzdmClass);
        action_import_commentid->setObjectName(QStringLiteral("action_import_commentid"));
        action_clear_comments = new QAction(auto_smzdmClass);
        action_clear_comments->setObjectName(QStringLiteral("action_clear_comments"));
        centralWidget = new QWidget(auto_smzdmClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_3 = new QGridLayout(centralWidget);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(groupBox);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        lineEdit_URL1 = new QLineEdit(groupBox);
        lineEdit_URL1->setObjectName(QStringLiteral("lineEdit_URL1"));

        horizontalLayout_2->addWidget(lineEdit_URL1);


        gridLayout_2->addLayout(horizontalLayout_2, 0, 0, 1, 1);

        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(groupBox);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        lineEdit_HOST1 = new QLineEdit(groupBox);
        lineEdit_HOST1->setObjectName(QStringLiteral("lineEdit_HOST1"));
        lineEdit_HOST1->setEnabled(false);

        horizontalLayout_3->addWidget(lineEdit_HOST1);


        gridLayout_2->addLayout(horizontalLayout_3, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_3 = new QLabel(groupBox);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_4->addWidget(label_3);

        lineEdit_REFERER = new QLineEdit(groupBox);
        lineEdit_REFERER->setObjectName(QStringLiteral("lineEdit_REFERER"));

        horizontalLayout_4->addWidget(lineEdit_REFERER);


        gridLayout_2->addLayout(horizontalLayout_4, 2, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        label_4 = new QLabel(groupBox);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_6->addWidget(label_4);

        lineEdit_news_id = new QLineEdit(groupBox);
        lineEdit_news_id->setObjectName(QStringLiteral("lineEdit_news_id"));
        lineEdit_news_id->setEnabled(false);

        horizontalLayout_6->addWidget(lineEdit_news_id);


        gridLayout_2->addLayout(horizontalLayout_6, 3, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 0, 0, 1, 1);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(3);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        lineEdit_comment_id = new QLineEdit(groupBox_2);
        lineEdit_comment_id->setObjectName(QStringLiteral("lineEdit_comment_id"));

        horizontalLayout_7->addWidget(lineEdit_comment_id);

        pushButton_add_comment_id = new QPushButton(groupBox_2);
        pushButton_add_comment_id->setObjectName(QStringLiteral("pushButton_add_comment_id"));

        horizontalLayout_7->addWidget(pushButton_add_comment_id);


        gridLayout->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        listWidget_comment_id = new QListWidget(groupBox_2);
        listWidget_comment_id->setObjectName(QStringLiteral("listWidget_comment_id"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listWidget_comment_id->sizePolicy().hasHeightForWidth());
        listWidget_comment_id->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(listWidget_comment_id, 1, 0, 1, 1);


        horizontalLayout_5->addWidget(groupBox_2);

        verticalLayout = new QVBoxLayout();
        verticalLayout->setSpacing(6);
        verticalLayout->setObjectName(QStringLiteral("verticalLayout"));
        pushButton_clickone = new QPushButton(centralWidget);
        pushButton_clickone->setObjectName(QStringLiteral("pushButton_clickone"));

        verticalLayout->addWidget(pushButton_clickone);

        pushButton_start = new QPushButton(centralWidget);
        pushButton_start->setObjectName(QStringLiteral("pushButton_start"));

        verticalLayout->addWidget(pushButton_start);

        pushButton_pause = new QPushButton(centralWidget);
        pushButton_pause->setObjectName(QStringLiteral("pushButton_pause"));

        verticalLayout->addWidget(pushButton_pause);

        verticalSpacer = new QSpacerItem(20, 40, QSizePolicy::Minimum, QSizePolicy::Expanding);

        verticalLayout->addItem(verticalSpacer);


        horizontalLayout_5->addLayout(verticalLayout);


        gridLayout_3->addLayout(horizontalLayout_5, 1, 0, 1, 1);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_1 = new QLabel(centralWidget);
        label_1->setObjectName(QStringLiteral("label_1"));

        horizontalLayout->addWidget(label_1);

        lineEdit_msg = new QLineEdit(centralWidget);
        lineEdit_msg->setObjectName(QStringLiteral("lineEdit_msg"));
        lineEdit_msg->setEnabled(false);

        horizontalLayout->addWidget(lineEdit_msg);


        gridLayout_3->addLayout(horizontalLayout, 2, 0, 1, 1);

        auto_smzdmClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(auto_smzdmClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 393, 23));
        auto_smzdmClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(auto_smzdmClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        auto_smzdmClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(auto_smzdmClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        auto_smzdmClass->setStatusBar(statusBar);

        mainToolBar->addAction(action_import_commentid);
        mainToolBar->addAction(action_clear_comments);

        retranslateUi(auto_smzdmClass);

        QMetaObject::connectSlotsByName(auto_smzdmClass);
    } // setupUi

    void retranslateUi(QMainWindow *auto_smzdmClass)
    {
        auto_smzdmClass->setWindowTitle(QApplication::translate("auto_smzdmClass", "\345\244\252\345\271\263\346\264\213\344\271\213\345\256\266\347\202\271\350\265\236", 0));
        action_import_commentid->setText(QApplication::translate("auto_smzdmClass", "\345\257\274\345\205\245\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        action_clear_comments->setText(QApplication::translate("auto_smzdmClass", "\346\270\205\347\251\272\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        groupBox->setTitle(QApplication::translate("auto_smzdmClass", "URL", 0));
        label->setText(QApplication::translate("auto_smzdmClass", "URL", 0));
        lineEdit_URL1->setText(QApplication::translate("auto_smzdmClass", "http://pdcmt.pconline.com.cn/action/CmtComment/updateCmtInfo.jsp", 0));
        label_2->setText(QApplication::translate("auto_smzdmClass", "Host", 0));
        lineEdit_HOST1->setText(QApplication::translate("auto_smzdmClass", "cmt.pconline.com.cn", 0));
        label_3->setText(QApplication::translate("auto_smzdmClass", "\346\226\260\351\227\273\347\275\221\345\235\200", 0));
        label_4->setText(QApplication::translate("auto_smzdmClass", "\346\226\260\351\227\273ID", 0));
        groupBox_2->setTitle(QApplication::translate("auto_smzdmClass", "\350\257\204\350\256\272ID\345\210\227\350\241\250", 0));
        pushButton_add_comment_id->setText(QApplication::translate("auto_smzdmClass", "\346\267\273\345\212\240", 0));
        pushButton_clickone->setText(QApplication::translate("auto_smzdmClass", "\350\277\220\350\241\214\344\270\200\346\254\241", 0));
        pushButton_start->setText(QApplication::translate("auto_smzdmClass", "\345\274\200\345\247\213\350\277\236\347\273\255", 0));
        pushButton_pause->setText(QApplication::translate("auto_smzdmClass", "\345\201\234\346\255\242", 0));
        label_1->setText(QApplication::translate("auto_smzdmClass", "\345\275\223\345\211\215\347\212\266\346\200\201", 0));
    } // retranslateUi

};

namespace Ui {
    class auto_smzdmClass: public Ui_auto_smzdmClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTO_SMZDM_H
