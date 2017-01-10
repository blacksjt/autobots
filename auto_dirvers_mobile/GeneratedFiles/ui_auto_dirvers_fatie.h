/********************************************************************************
** Form generated from reading UI file 'auto_dirvers_fatie.ui'
**
** Created by: Qt User Interface Compiler version 5.7.0
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_AUTO_DIRVERS_FATIE_H
#define UI_AUTO_DIRVERS_FATIE_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QComboBox>
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
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QTabWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QToolBar>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_auto_dirvers_fatieClass
{
public:
    QAction *action_from_txt;
    QAction *action_import_commentid;
    QAction *action_clear_comments;
    QAction *action_clear_accounts;
    QWidget *centralWidget;
    QGridLayout *gridLayout_6;
    QHBoxLayout *horizontalLayout_10;
    QHBoxLayout *horizontalLayout_8;
    QLabel *label_6;
    QLineEdit *lineEdit_host1;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label;
    QSpinBox *spinBox_interval;
    QHBoxLayout *horizontalLayout_17;
    QHBoxLayout *horizontalLayout_15;
    QLabel *label_11;
    QComboBox *comboBox_adsl;
    QHBoxLayout *horizontalLayout_16;
    QLabel *label_12;
    QLineEdit *lineEdit_account;
    QHBoxLayout *horizontalLayout_18;
    QLabel *label_13;
    QLineEdit *lineEdit_password;
    QGroupBox *groupBox_2;
    QGridLayout *gridLayout_4;
    QTableWidget *tableWidget_account_id;
    QTabWidget *tabWidget_comments;
    QWidget *tab;
    QGridLayout *gridLayout_3;
    QHBoxLayout *horizontalLayout_5;
    QHBoxLayout *horizontalLayout;
    QLabel *label_3;
    QLineEdit *lineEdit_url1;
    QHBoxLayout *horizontalLayout_6;
    QHBoxLayout *horizontalLayout_3;
    QLabel *label_2;
    QLineEdit *lineEdit_news_id1;
    QGroupBox *groupBox;
    QGridLayout *gridLayout_2;
    QListWidget *listWidget_comment_id1;
    QWidget *tab_2;
    QGridLayout *gridLayout;
    QHBoxLayout *horizontalLayout_13;
    QLabel *label_8;
    QLineEdit *lineEdit_url2;
    QHBoxLayout *horizontalLayout_14;
    QLabel *label_7;
    QLineEdit *lineEdit_news_id2;
    QGroupBox *groupBox_4;
    QGridLayout *gridLayout_8;
    QListWidget *listWidget_comment_id2;
    QWidget *tab_3;
    QGridLayout *gridLayout_5;
    QHBoxLayout *horizontalLayout_7;
    QLabel *label_9;
    QLineEdit *lineEdit_url3;
    QHBoxLayout *horizontalLayout_9;
    QLabel *label_10;
    QLineEdit *lineEdit_news_id3;
    QGroupBox *groupBox_5;
    QGridLayout *gridLayout_10;
    QListWidget *listWidget_comment_id3;
    QHBoxLayout *horizontalLayout_4;
    QPushButton *pushButton_start;
    QPushButton *pushButton_stop;
    QMenuBar *menuBar;
    QToolBar *mainToolBar;
    QStatusBar *statusBar;

    void setupUi(QMainWindow *auto_dirvers_fatieClass)
    {
        if (auto_dirvers_fatieClass->objectName().isEmpty())
            auto_dirvers_fatieClass->setObjectName(QStringLiteral("auto_dirvers_fatieClass"));
        auto_dirvers_fatieClass->resize(471, 645);
        action_from_txt = new QAction(auto_dirvers_fatieClass);
        action_from_txt->setObjectName(QStringLiteral("action_from_txt"));
        action_import_commentid = new QAction(auto_dirvers_fatieClass);
        action_import_commentid->setObjectName(QStringLiteral("action_import_commentid"));
        action_clear_comments = new QAction(auto_dirvers_fatieClass);
        action_clear_comments->setObjectName(QStringLiteral("action_clear_comments"));
        action_clear_accounts = new QAction(auto_dirvers_fatieClass);
        action_clear_accounts->setObjectName(QStringLiteral("action_clear_accounts"));
        centralWidget = new QWidget(auto_dirvers_fatieClass);
        centralWidget->setObjectName(QStringLiteral("centralWidget"));
        gridLayout_6 = new QGridLayout(centralWidget);
        gridLayout_6->setSpacing(6);
        gridLayout_6->setContentsMargins(11, 11, 11, 11);
        gridLayout_6->setObjectName(QStringLiteral("gridLayout_6"));
        horizontalLayout_10 = new QHBoxLayout();
        horizontalLayout_10->setSpacing(6);
        horizontalLayout_10->setObjectName(QStringLiteral("horizontalLayout_10"));
        horizontalLayout_8 = new QHBoxLayout();
        horizontalLayout_8->setSpacing(6);
        horizontalLayout_8->setObjectName(QStringLiteral("horizontalLayout_8"));
        label_6 = new QLabel(centralWidget);
        label_6->setObjectName(QStringLiteral("label_6"));

        horizontalLayout_8->addWidget(label_6);

        lineEdit_host1 = new QLineEdit(centralWidget);
        lineEdit_host1->setObjectName(QStringLiteral("lineEdit_host1"));
        lineEdit_host1->setEnabled(true);

        horizontalLayout_8->addWidget(lineEdit_host1);


        horizontalLayout_10->addLayout(horizontalLayout_8);

        horizontalLayout_2 = new QHBoxLayout();
        horizontalLayout_2->setSpacing(6);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        label = new QLabel(centralWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout_2->addWidget(label);

        spinBox_interval = new QSpinBox(centralWidget);
        spinBox_interval->setObjectName(QStringLiteral("spinBox_interval"));
        spinBox_interval->setMinimum(1);
        spinBox_interval->setMaximum(1000);
        spinBox_interval->setSingleStep(5);
        spinBox_interval->setValue(15);

        horizontalLayout_2->addWidget(spinBox_interval);


        horizontalLayout_10->addLayout(horizontalLayout_2);


        gridLayout_6->addLayout(horizontalLayout_10, 0, 0, 1, 1);

        horizontalLayout_17 = new QHBoxLayout();
        horizontalLayout_17->setSpacing(6);
        horizontalLayout_17->setObjectName(QStringLiteral("horizontalLayout_17"));
        horizontalLayout_15 = new QHBoxLayout();
        horizontalLayout_15->setSpacing(6);
        horizontalLayout_15->setObjectName(QStringLiteral("horizontalLayout_15"));
        label_11 = new QLabel(centralWidget);
        label_11->setObjectName(QStringLiteral("label_11"));

        horizontalLayout_15->addWidget(label_11);

        comboBox_adsl = new QComboBox(centralWidget);
        comboBox_adsl->setObjectName(QStringLiteral("comboBox_adsl"));
        QSizePolicy sizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        sizePolicy.setHeightForWidth(comboBox_adsl->sizePolicy().hasHeightForWidth());
        comboBox_adsl->setSizePolicy(sizePolicy);
        comboBox_adsl->setEditable(true);

        horizontalLayout_15->addWidget(comboBox_adsl);


        horizontalLayout_17->addLayout(horizontalLayout_15);

        horizontalLayout_16 = new QHBoxLayout();
        horizontalLayout_16->setSpacing(6);
        horizontalLayout_16->setObjectName(QStringLiteral("horizontalLayout_16"));
        label_12 = new QLabel(centralWidget);
        label_12->setObjectName(QStringLiteral("label_12"));

        horizontalLayout_16->addWidget(label_12);

        lineEdit_account = new QLineEdit(centralWidget);
        lineEdit_account->setObjectName(QStringLiteral("lineEdit_account"));

        horizontalLayout_16->addWidget(lineEdit_account);


        horizontalLayout_17->addLayout(horizontalLayout_16);

        horizontalLayout_18 = new QHBoxLayout();
        horizontalLayout_18->setSpacing(6);
        horizontalLayout_18->setObjectName(QStringLiteral("horizontalLayout_18"));
        label_13 = new QLabel(centralWidget);
        label_13->setObjectName(QStringLiteral("label_13"));

        horizontalLayout_18->addWidget(label_13);

        lineEdit_password = new QLineEdit(centralWidget);
        lineEdit_password->setObjectName(QStringLiteral("lineEdit_password"));

        horizontalLayout_18->addWidget(lineEdit_password);


        horizontalLayout_17->addLayout(horizontalLayout_18);


        gridLayout_6->addLayout(horizontalLayout_17, 1, 0, 1, 1);

        groupBox_2 = new QGroupBox(centralWidget);
        groupBox_2->setObjectName(QStringLiteral("groupBox_2"));
        QSizePolicy sizePolicy1(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy1.setHorizontalStretch(4);
        sizePolicy1.setVerticalStretch(0);
        sizePolicy1.setHeightForWidth(groupBox_2->sizePolicy().hasHeightForWidth());
        groupBox_2->setSizePolicy(sizePolicy1);
        gridLayout_4 = new QGridLayout(groupBox_2);
        gridLayout_4->setSpacing(6);
        gridLayout_4->setContentsMargins(11, 11, 11, 11);
        gridLayout_4->setObjectName(QStringLiteral("gridLayout_4"));
        tableWidget_account_id = new QTableWidget(groupBox_2);
        if (tableWidget_account_id->columnCount() < 2)
            tableWidget_account_id->setColumnCount(2);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        tableWidget_account_id->setHorizontalHeaderItem(0, __qtablewidgetitem);
        QTableWidgetItem *__qtablewidgetitem1 = new QTableWidgetItem();
        tableWidget_account_id->setHorizontalHeaderItem(1, __qtablewidgetitem1);
        tableWidget_account_id->setObjectName(QStringLiteral("tableWidget_account_id"));

        gridLayout_4->addWidget(tableWidget_account_id, 0, 0, 1, 1);


        gridLayout_6->addWidget(groupBox_2, 2, 0, 1, 1);

        tabWidget_comments = new QTabWidget(centralWidget);
        tabWidget_comments->setObjectName(QStringLiteral("tabWidget_comments"));
        tab = new QWidget();
        tab->setObjectName(QStringLiteral("tab"));
        gridLayout_3 = new QGridLayout(tab);
        gridLayout_3->setSpacing(6);
        gridLayout_3->setContentsMargins(11, 11, 11, 11);
        gridLayout_3->setObjectName(QStringLiteral("gridLayout_3"));
        horizontalLayout_5 = new QHBoxLayout();
        horizontalLayout_5->setSpacing(6);
        horizontalLayout_5->setObjectName(QStringLiteral("horizontalLayout_5"));
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setSpacing(6);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        label_3 = new QLabel(tab);
        label_3->setObjectName(QStringLiteral("label_3"));

        horizontalLayout->addWidget(label_3);

        lineEdit_url1 = new QLineEdit(tab);
        lineEdit_url1->setObjectName(QStringLiteral("lineEdit_url1"));
        lineEdit_url1->setEnabled(true);

        horizontalLayout->addWidget(lineEdit_url1);


        horizontalLayout_5->addLayout(horizontalLayout);


        gridLayout_3->addLayout(horizontalLayout_5, 0, 0, 1, 1);

        horizontalLayout_6 = new QHBoxLayout();
        horizontalLayout_6->setSpacing(6);
        horizontalLayout_6->setObjectName(QStringLiteral("horizontalLayout_6"));
        horizontalLayout_3 = new QHBoxLayout();
        horizontalLayout_3->setSpacing(6);
        horizontalLayout_3->setObjectName(QStringLiteral("horizontalLayout_3"));
        label_2 = new QLabel(tab);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_3->addWidget(label_2);

        lineEdit_news_id1 = new QLineEdit(tab);
        lineEdit_news_id1->setObjectName(QStringLiteral("lineEdit_news_id1"));
        lineEdit_news_id1->setEnabled(true);

        horizontalLayout_3->addWidget(lineEdit_news_id1);


        horizontalLayout_6->addLayout(horizontalLayout_3);


        gridLayout_3->addLayout(horizontalLayout_6, 1, 0, 1, 1);

        groupBox = new QGroupBox(tab);
        groupBox->setObjectName(QStringLiteral("groupBox"));
        QSizePolicy sizePolicy2(QSizePolicy::Preferred, QSizePolicy::Preferred);
        sizePolicy2.setHorizontalStretch(6);
        sizePolicy2.setVerticalStretch(0);
        sizePolicy2.setHeightForWidth(groupBox->sizePolicy().hasHeightForWidth());
        groupBox->setSizePolicy(sizePolicy2);
        gridLayout_2 = new QGridLayout(groupBox);
        gridLayout_2->setSpacing(6);
        gridLayout_2->setContentsMargins(11, 11, 11, 11);
        gridLayout_2->setObjectName(QStringLiteral("gridLayout_2"));
        listWidget_comment_id1 = new QListWidget(groupBox);
        listWidget_comment_id1->setObjectName(QStringLiteral("listWidget_comment_id1"));
        QSizePolicy sizePolicy3(QSizePolicy::Expanding, QSizePolicy::Expanding);
        sizePolicy3.setHorizontalStretch(2);
        sizePolicy3.setVerticalStretch(0);
        sizePolicy3.setHeightForWidth(listWidget_comment_id1->sizePolicy().hasHeightForWidth());
        listWidget_comment_id1->setSizePolicy(sizePolicy3);

        gridLayout_2->addWidget(listWidget_comment_id1, 0, 0, 1, 1);


        gridLayout_3->addWidget(groupBox, 2, 0, 1, 1);

        tabWidget_comments->addTab(tab, QString());
        tab_2 = new QWidget();
        tab_2->setObjectName(QStringLiteral("tab_2"));
        gridLayout = new QGridLayout(tab_2);
        gridLayout->setSpacing(6);
        gridLayout->setContentsMargins(11, 11, 11, 11);
        gridLayout->setObjectName(QStringLiteral("gridLayout"));
        horizontalLayout_13 = new QHBoxLayout();
        horizontalLayout_13->setSpacing(6);
        horizontalLayout_13->setObjectName(QStringLiteral("horizontalLayout_13"));
        label_8 = new QLabel(tab_2);
        label_8->setObjectName(QStringLiteral("label_8"));

        horizontalLayout_13->addWidget(label_8);

        lineEdit_url2 = new QLineEdit(tab_2);
        lineEdit_url2->setObjectName(QStringLiteral("lineEdit_url2"));
        lineEdit_url2->setEnabled(true);

        horizontalLayout_13->addWidget(lineEdit_url2);


        gridLayout->addLayout(horizontalLayout_13, 0, 0, 1, 1);

        horizontalLayout_14 = new QHBoxLayout();
        horizontalLayout_14->setSpacing(6);
        horizontalLayout_14->setObjectName(QStringLiteral("horizontalLayout_14"));
        label_7 = new QLabel(tab_2);
        label_7->setObjectName(QStringLiteral("label_7"));

        horizontalLayout_14->addWidget(label_7);

        lineEdit_news_id2 = new QLineEdit(tab_2);
        lineEdit_news_id2->setObjectName(QStringLiteral("lineEdit_news_id2"));
        lineEdit_news_id2->setEnabled(true);

        horizontalLayout_14->addWidget(lineEdit_news_id2);


        gridLayout->addLayout(horizontalLayout_14, 1, 0, 1, 1);

        groupBox_4 = new QGroupBox(tab_2);
        groupBox_4->setObjectName(QStringLiteral("groupBox_4"));
        sizePolicy2.setHeightForWidth(groupBox_4->sizePolicy().hasHeightForWidth());
        groupBox_4->setSizePolicy(sizePolicy2);
        gridLayout_8 = new QGridLayout(groupBox_4);
        gridLayout_8->setSpacing(6);
        gridLayout_8->setContentsMargins(11, 11, 11, 11);
        gridLayout_8->setObjectName(QStringLiteral("gridLayout_8"));
        listWidget_comment_id2 = new QListWidget(groupBox_4);
        listWidget_comment_id2->setObjectName(QStringLiteral("listWidget_comment_id2"));
        sizePolicy3.setHeightForWidth(listWidget_comment_id2->sizePolicy().hasHeightForWidth());
        listWidget_comment_id2->setSizePolicy(sizePolicy3);

        gridLayout_8->addWidget(listWidget_comment_id2, 0, 0, 1, 1);


        gridLayout->addWidget(groupBox_4, 2, 0, 1, 1);

        tabWidget_comments->addTab(tab_2, QString());
        tab_3 = new QWidget();
        tab_3->setObjectName(QStringLiteral("tab_3"));
        gridLayout_5 = new QGridLayout(tab_3);
        gridLayout_5->setSpacing(6);
        gridLayout_5->setContentsMargins(11, 11, 11, 11);
        gridLayout_5->setObjectName(QStringLiteral("gridLayout_5"));
        horizontalLayout_7 = new QHBoxLayout();
        horizontalLayout_7->setSpacing(6);
        horizontalLayout_7->setObjectName(QStringLiteral("horizontalLayout_7"));
        label_9 = new QLabel(tab_3);
        label_9->setObjectName(QStringLiteral("label_9"));

        horizontalLayout_7->addWidget(label_9);

        lineEdit_url3 = new QLineEdit(tab_3);
        lineEdit_url3->setObjectName(QStringLiteral("lineEdit_url3"));
        lineEdit_url3->setEnabled(true);

        horizontalLayout_7->addWidget(lineEdit_url3);


        gridLayout_5->addLayout(horizontalLayout_7, 0, 0, 1, 1);

        horizontalLayout_9 = new QHBoxLayout();
        horizontalLayout_9->setSpacing(6);
        horizontalLayout_9->setObjectName(QStringLiteral("horizontalLayout_9"));
        label_10 = new QLabel(tab_3);
        label_10->setObjectName(QStringLiteral("label_10"));

        horizontalLayout_9->addWidget(label_10);

        lineEdit_news_id3 = new QLineEdit(tab_3);
        lineEdit_news_id3->setObjectName(QStringLiteral("lineEdit_news_id3"));
        lineEdit_news_id3->setEnabled(true);

        horizontalLayout_9->addWidget(lineEdit_news_id3);


        gridLayout_5->addLayout(horizontalLayout_9, 1, 0, 1, 1);

        groupBox_5 = new QGroupBox(tab_3);
        groupBox_5->setObjectName(QStringLiteral("groupBox_5"));
        sizePolicy2.setHeightForWidth(groupBox_5->sizePolicy().hasHeightForWidth());
        groupBox_5->setSizePolicy(sizePolicy2);
        gridLayout_10 = new QGridLayout(groupBox_5);
        gridLayout_10->setSpacing(6);
        gridLayout_10->setContentsMargins(11, 11, 11, 11);
        gridLayout_10->setObjectName(QStringLiteral("gridLayout_10"));
        listWidget_comment_id3 = new QListWidget(groupBox_5);
        listWidget_comment_id3->setObjectName(QStringLiteral("listWidget_comment_id3"));
        sizePolicy3.setHeightForWidth(listWidget_comment_id3->sizePolicy().hasHeightForWidth());
        listWidget_comment_id3->setSizePolicy(sizePolicy3);

        gridLayout_10->addWidget(listWidget_comment_id3, 0, 0, 1, 1);


        gridLayout_5->addWidget(groupBox_5, 2, 0, 1, 1);

        tabWidget_comments->addTab(tab_3, QString());

        gridLayout_6->addWidget(tabWidget_comments, 3, 0, 1, 1);

        horizontalLayout_4 = new QHBoxLayout();
        horizontalLayout_4->setSpacing(6);
        horizontalLayout_4->setObjectName(QStringLiteral("horizontalLayout_4"));
        pushButton_start = new QPushButton(centralWidget);
        pushButton_start->setObjectName(QStringLiteral("pushButton_start"));

        horizontalLayout_4->addWidget(pushButton_start);

        pushButton_stop = new QPushButton(centralWidget);
        pushButton_stop->setObjectName(QStringLiteral("pushButton_stop"));

        horizontalLayout_4->addWidget(pushButton_stop);


        gridLayout_6->addLayout(horizontalLayout_4, 4, 0, 1, 1);

        auto_dirvers_fatieClass->setCentralWidget(centralWidget);
        menuBar = new QMenuBar(auto_dirvers_fatieClass);
        menuBar->setObjectName(QStringLiteral("menuBar"));
        menuBar->setGeometry(QRect(0, 0, 471, 23));
        auto_dirvers_fatieClass->setMenuBar(menuBar);
        mainToolBar = new QToolBar(auto_dirvers_fatieClass);
        mainToolBar->setObjectName(QStringLiteral("mainToolBar"));
        auto_dirvers_fatieClass->addToolBar(Qt::TopToolBarArea, mainToolBar);
        statusBar = new QStatusBar(auto_dirvers_fatieClass);
        statusBar->setObjectName(QStringLiteral("statusBar"));
        auto_dirvers_fatieClass->setStatusBar(statusBar);

        mainToolBar->addAction(action_from_txt);
        mainToolBar->addAction(action_clear_accounts);
        mainToolBar->addAction(action_import_commentid);
        mainToolBar->addAction(action_clear_comments);

        retranslateUi(auto_dirvers_fatieClass);

        tabWidget_comments->setCurrentIndex(0);


        QMetaObject::connectSlotsByName(auto_dirvers_fatieClass);
    } // setupUi

    void retranslateUi(QMainWindow *auto_dirvers_fatieClass)
    {
        auto_dirvers_fatieClass->setWindowTitle(QApplication::translate("auto_dirvers_fatieClass", "\351\251\261\345\212\250\344\271\213\345\256\266\345\217\221\345\270\226", 0));
        action_from_txt->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\257\274\345\205\245\350\264\246\345\217\267", 0));
#ifndef QT_NO_TOOLTIP
        action_from_txt->setToolTip(QApplication::translate("auto_dirvers_fatieClass", "txt\346\226\207\346\234\254", 0));
#endif // QT_NO_TOOLTIP
        action_import_commentid->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\257\274\345\205\245\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        action_clear_comments->setText(QApplication::translate("auto_dirvers_fatieClass", "\346\270\205\347\251\272\350\257\204\350\256\272\345\210\227\350\241\250", 0));
        action_clear_accounts->setText(QApplication::translate("auto_dirvers_fatieClass", "\346\270\205\347\251\272\350\264\246\345\217\267\345\210\227\350\241\250", 0));
        label_6->setText(QApplication::translate("auto_dirvers_fatieClass", "\344\270\273\346\234\272\357\274\210HOST\357\274\211", 0));
        lineEdit_host1->setText(QApplication::translate("auto_dirvers_fatieClass", "comment8.mydrivers.com", 0));
        label->setText(QApplication::translate("auto_dirvers_fatieClass", "\351\227\264\351\232\224\346\227\266\351\227\264", 0));
        label_11->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\256\275\345\270\246\345\220\215\347\247\260", 0));
        label_12->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\256\275\345\270\246\350\264\246\345\217\267", 0));
        lineEdit_account->setText(QString());
        label_13->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\256\275\345\270\246\345\257\206\347\240\201", 0));
        lineEdit_password->setText(QString());
        groupBox_2->setTitle(QApplication::translate("auto_dirvers_fatieClass", "\350\264\246\345\217\267\345\210\227\350\241\250", 0));
        QTableWidgetItem *___qtablewidgetitem = tableWidget_account_id->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QApplication::translate("auto_dirvers_fatieClass", "\350\264\246\345\217\267", 0));
        QTableWidgetItem *___qtablewidgetitem1 = tableWidget_account_id->horizontalHeaderItem(1);
        ___qtablewidgetitem1->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\257\206\347\240\201", 0));
        label_3->setText(QApplication::translate("auto_dirvers_fatieClass", "\347\275\221\351\241\265URL", 0));
        label_2->setText(QApplication::translate("auto_dirvers_fatieClass", "\346\226\260\351\227\273ID", 0));
        lineEdit_news_id1->setText(QString());
        groupBox->setTitle(QApplication::translate("auto_dirvers_fatieClass", "\350\257\204\350\256\272\345\206\205\345\256\271", 0));
        tabWidget_comments->setTabText(tabWidget_comments->indexOf(tab), QApplication::translate("auto_dirvers_fatieClass", "\347\254\254\344\270\200\347\257\207", 0));
        label_8->setText(QApplication::translate("auto_dirvers_fatieClass", "\347\275\221\351\241\265URL", 0));
        label_7->setText(QApplication::translate("auto_dirvers_fatieClass", "\346\226\260\351\227\273ID", 0));
        lineEdit_news_id2->setText(QString());
        groupBox_4->setTitle(QApplication::translate("auto_dirvers_fatieClass", "\350\257\204\350\256\272\345\206\205\345\256\271", 0));
        tabWidget_comments->setTabText(tabWidget_comments->indexOf(tab_2), QApplication::translate("auto_dirvers_fatieClass", "\347\254\254\344\272\214\347\257\207", 0));
        label_9->setText(QApplication::translate("auto_dirvers_fatieClass", "\347\275\221\351\241\265URL", 0));
        label_10->setText(QApplication::translate("auto_dirvers_fatieClass", "\346\226\260\351\227\273ID", 0));
        lineEdit_news_id3->setText(QString());
        groupBox_5->setTitle(QApplication::translate("auto_dirvers_fatieClass", "\350\257\204\350\256\272\345\206\205\345\256\271", 0));
        tabWidget_comments->setTabText(tabWidget_comments->indexOf(tab_3), QApplication::translate("auto_dirvers_fatieClass", "\347\254\254\344\270\211\347\257\207", 0));
        pushButton_start->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\274\200\345\247\213", 0));
        pushButton_stop->setText(QApplication::translate("auto_dirvers_fatieClass", "\345\201\234\346\255\242", 0));
    } // retranslateUi

};

namespace Ui {
    class auto_dirvers_fatieClass: public Ui_auto_dirvers_fatieClass {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_AUTO_DIRVERS_FATIE_H
