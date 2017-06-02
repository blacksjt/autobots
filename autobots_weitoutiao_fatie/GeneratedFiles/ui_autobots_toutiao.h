/********************************************************************************
** Form generated from reading UI file 'autobots_toutiao.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTOBOTS_TOUTIAO_H
#define UI_AUTOBOTS_TOUTIAO_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QTextBrowser>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_autobots_toutiaoClass
{
public:
    QAction *action_from_txt;
    QAction *action_import_commentid;
    QAction *action_clear_comments;
    QAction *action_clear_accounts;
    QWidget *centralWidget;
    QGridLayout *gridLayout_4;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_3;
    QLineEdit *lineEdit_url;
    QCheckBox *checkBox_code_online;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_5;
    QSpinBox *spinBox_interval;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *lineEdit_page_id;
    QHBoxLayout *horizontalLayout_4;
    QLabel *label_4;
    QLineEdit *lineEdit_comment_group;
    QHBoxLayout *horizontalLayout_6;
    QGroupBox *groupBox;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_7;
    QLineEdit *lineEdit_comment_id;
    QPushButton *pushButton_add_comment_id;
    QListWidget *listWidget_comment_id;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_2;
    QTableWidget *tableWidget_account_id;
    QTextBrowser *textBrowser_commentid;
    QHBoxLayout *horizontalLayout;
    QGridLayout *gridLayout_3;
    QLabel *label_1;
    QLineEdit *lineEdit_msg;
    QHBoxLayout *horizontalLayout_5;
    QPushButton *pushButton_start;
    QPushButton *pushButton_pause;
    QMenuBar *menuBar;
    QMenu *menu;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *autobots_toutiaoClass)
    {
        if (autobots_toutiaoClass->objectName().isEmpty())
            autobots_toutiaoClass->setObjectName(QStringLiteral("autobots_toutiaoClass"));
        autobots_toutiaoClass->resize(711, 510);
        action_from_txt = new QAction(autobots_toutiaoClass);
        action_from_txt->setObjectName(QStringLiteral("action_from_txt"));
        action_import_commentid = new QAction(autobots_toutiaoClass);
        action_import_commentid->setObjectName(QStringLiteral("action_import_commentid"));
        action_clear_comments = new QAction(autobots_toutiaoClass);
        action_clear_comments->setObjectName(QStringLiteral("action_clear_comments"));
        action_clear_accounts = new QAction(autobots_toutiaoClass);
        action_clear_accounts->setObjectName(QStringLiteral("action_clear_accounts"));
        centralWidget = new QWidget(autobots_toutiaoClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_4 = new QGridLayout(centralWidget);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_3 = new QLabel(centralWidget);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout_3->addWidget(label_3);

        lineEdit_url = new QLineEdit(centralWidget);
        lineEdit_url->setObjectName(QStringLiteral("lineEdit_url"));
        lineEdit_url->setEnabled(false);

        horizontalLayout_3->addWidget(lineEdit_url);

        checkBox_code_online = new QCheckBox(centralWidget);
        checkBox_code_online->setObjectName(QStringLiteral("checkBox_code_online"));
        checkBox_code_online->setChecked(true);

        horizontalLayout_3->addWidget(checkBox_code_online);


        gridLayout_4->addLayout(horizontalLayout_3, 0, 0, 1, 2);

        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_5 = new QLabel(centralWidget);
        label_5->setObjectName(QStringLiteral("label_5"));

        horizontalLayout_8->addWidget(label_5);

        spinBox_interval = new QSpinBox(centralWidget);
        spinBox_interval->setObjectName(QStringLiteral("spinBox_interval"));
        spinBox_interval->setMinimum(5);
        spinBox_interval->setMaximum(600);
        spinBox_interval->setSingleStep(5);
        spinBox_interval->setValue(45);

        horizontalLayout_8->addWidget(spinBox_interval);


        gridLayout_4->addLayout(horizontalLayout_8, 0, 2, 1, 1);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label_2 = new QLabel(centralWidget);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        lineEdit_page_id = new QLineEdit(centralWidget);
        lineEdit_page_id->setObjectName(QStringLiteral("lineEdit_page_id"));
        lineEdit_page_id->setEnabled(true);

        horizontalLayout_2->addWidget(lineEdit_page_id);


        gridLayout_4->addLayout(horizontalLayout_2, 1, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        label_4 = new QLabel(centralWidget);
        label_4->setObjectName(QStringLiteral("label_4"));

        horizontalLayout_4->addWidget(label_4);

        lineEdit_comment_group = new QLineEdit(centralWidget);
        lineEdit_comment_group->setObjectName(QStringLiteral("lineEdit_comment_group"));
        lineEdit_comment_group->setEnabled(true);

        horizontalLayout_4->addWidget(lineEdit_comment_group);


        gridLayout_4->addLayout(horizontalLayout_4, 1, 1, 1, 2);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        groupBox = new QGroupBox(centralWidget);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy.setHorizontalStretch(6);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy);
        gridLayout = new QGridLayout(groupBox);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        lineEdit_comment_id = new QLineEdit(groupBox);
        lineEdit_comment_id->setObjectName(QStringLiteral("lineEdit_comment_id"));

        horizontalLayout_7->addWidget(lineEdit_comment_id);

        pushButton_add_comment_id = new QPushButton(groupBox);
        pushButton_add_comment_id->setObjectName(QStringLiteral("pushButton_add_comment_id"));

        horizontalLayout_7->addWidget(pushButton_add_comment_id);


        gridLayout->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        listWidget_comment_id = new QListWidget(groupBox);
        listWidget_comment_id->setObjectName(QStringLiteral("listWidget_comment_id"));
        QSizePolicy sizePolicy1(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy1.setHorizontalStretch(2);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(listWidget_comment_id->sizePolicy().hasHeightForWidth());
        listWidget_comment_id->setSizePolicy(sizePolicy1);

        gridLayout->addWidget(listWidget_comment_id, 1, 0, 1, 1);


        horizontalLayout_6->addWidget(groupBox);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(4);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy2);
        gridLayout_2 = new QGridLayout(groupBox_2);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        tableWidget_account_id = new QTableWidget(groupBox_2);
        if (tableWidget_account_id->columnCount() < 2)
            tableWidget_account_id->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_account_id->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_account_id->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget_account_id->setObjectName(QStringLiteral("tableWidget_account_id"));

        gridLayout_2->addWidget(tableWidget_account_id, 0, 0, 1, 1);

        textBrowser_commentid = new QTextBrowser(groupBox_2);
        textBrowser_commentid->setObjectName(QStringLiteral("textBrowser_commentid"));

        gridLayout_2->addWidget(textBrowser_commentid, 1, 0, 1, 1);


        horizontalLayout_6->addWidget(groupBox_2);


        gridLayout_4->addLayout(horizontalLayout_6, 2, 0, 1, 3);

        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        gridLayout_3 = new QGridLayout();
        gridLayout_3->setSpacing(6);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        label_1 = new QLabel(centralWidget);
        label_1->setObjectName(QStringLiteral("label_1"));

        gridLayout_3->addWidget(label_1, 0, 0, 1, 1);

        lineEdit_msg = new QLineEdit(centralWidget);
        lineEdit_msg->setObjectName(QStringLiteral("lineEdit_msg"));
        lineEdit_msg->setEnabled(false);

        gridLayout_3->addWidget(lineEdit_msg, 0, 1, 1, 1);


        horizontalLayout->addLayout(gridLayout_3);

        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        pushButton_start = new QPushButton(centralWidget);
        pushButton_start->setObjectName(QStringLiteral("pushButton_start"));

        horizontalLayout_5->addWidget(pushButton_start);

        pushButton_pause = new QPushButton(centralWidget);
        pushButton_pause->setObjectName(QStringLiteral("pushButton_pause"));

        horizontalLayout_5->addWidget(pushButton_pause);


        horizontalLayout->addLayout(horizontalLayout_5);


        gridLayout_4->addLayout(horizontalLayout, 3, 0, 1, 3);

        autobots_toutiaoClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(autobots_toutiaoClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 711, 23));
        menu = new QMenu(menuBar);
        menu->setObjectName(QStringLiteral("menu"));
        autobots_toutiaoClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(autobots_toutiaoClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        autobots_toutiaoClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(autobots_toutiaoClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        autobots_toutiaoClass->setStatusBar(statusBar);

        menuBar->addAction(menu->menuAction());
        menu->addAction(action_from_txt);
        menu->addAction(action_clear_accounts);
        menu->addAction(action_import_commentid);
        menu->addAction(action_clear_comments);
        mainToolBar->addAction(action_from_txt);
        mainToolBar->addAction(action_clear_accounts);
        mainToolBar->addAction(action_import_commentid);
        mainToolBar->addAction(action_clear_comments);

        retranslateUi(autobots_toutiaoClass);

        QMetaObject::connectSlotsByName(autobots_toutiaoClass);
    } // setupUi

    void retranslateUi(QMainWindow *autobots_toutiaoClass)
    {
        autobots_toutiaoClass->setWindowTitle(QApplication::translate("autobots_toutiaoClass", "\345\276\256\345\244\264\346\235\241App\345\217\221\345\270\226\357\274\210\345\276\256\345\215\232\357\274\211", 0));
        action_from_txt->setText(QApplication::translate("autobots_toutiaoClass", "\345\257\274\345\205\245\350\264\246\345\217\267", 0));
#ifndef QT_NO_TOOLTIP
        action_from_txt->setToolTip(QApplication::translate("autobots_toutiaoClass", "txt\346\226\207\346\234\254", 0));
#endif // QT_NO_TOOLTIP
        action_import_commentid->setText(QApplication::translate("autobots_toutiaoClass", "\345\257\274\345\205\245\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        action_clear_comments->setText(QApplication::translate("autobots_toutiaoClass", "\346\270\205\347\251\272\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        action_clear_accounts->setText(QApplication::translate("autobots_toutiaoClass", "\346\270\205\347\251\272\350\264\246\345\217\267\345\210\227\350\241\250", 0));
        label_3->setText(QApplication::translate("autobots_toutiaoClass", "\347\275\221\351\241\265URL", 0));
        checkBox_code_online->setText(QApplication::translate("autobots_toutiaoClass", "\350\207\252\345\212\250\350\257\206\345\210\253\351\252\214\350\257\201\347\240\201", 0));
        label_5->setText(QApplication::translate("autobots_toutiaoClass", "\351\227\264\351\232\224\357\274\210\347\247\222\357\274\211", 0));
        label_2->setText(QApplication::translate("autobots_toutiaoClass", "(group_id)(thread_id)", 0));
        lineEdit_page_id->setText(QString());
        label_4->setText(QApplication::translate("autobots_toutiaoClass", "(forum_id)", 0));
        groupBox->setTitle(QApplication::translate("autobots_toutiaoClass", "\350\257\204\350\256\272\345\206\205\345\256\271", 0));
        pushButton_add_comment_id->setText(QApplication::translate("autobots_toutiaoClass", "\346\267\273\345\212\240", 0));
        groupBox_2->setTitle(QApplication::translate("autobots_toutiaoClass", "\350\264\246\345\217\267\345\210\227\350\241\250", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_account_id->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("autobots_toutiaoClass", "\350\264\246\345\217\267", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_account_id->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("autobots_toutiaoClass", "\345\257\206\347\240\201", 0));
        label_1->setText(QApplication::translate("autobots_toutiaoClass", "\345\275\223\345\211\215\347\212\266\346\200\201", 0));
        pushButton_start->setText(QApplication::translate("autobots_toutiaoClass", "\345\274\200\345\247\213", 0));
        pushButton_pause->setText(QApplication::translate("autobots_toutiaoClass", "\345\201\234\346\255\242", 0));
        menu->setTitle(QApplication::translate("autobots_toutiaoClass", "\345\257\274\345\205\245", 0));
    } // retranslateUi

};

namespace Ui {
    class autobots_toutiaoClass: public Ui_autobots_toutiaoClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTOBOTS_TOUTIAO_H
