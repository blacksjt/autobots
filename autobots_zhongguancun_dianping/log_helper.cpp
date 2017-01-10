#include <QDateTime>
#include <iostream>
#include <QDebug>

#include "log_helper.h"

QString GetCurrentDateTimeString()
{
    QDateTime t = QDateTime::currentDateTime();
    QString str_time = "[" + t.toString() + "] ";

    return str_time;
}

void LogHelper::PrintInformations( const QString& text )
{
    setlocale(LC_ALL,"chs");
    QString str_texts = GetCurrentDateTimeString() + text;
    //qDebug() << str_texts;
    std::wstring s ;//= str_texts.toStdWString();
    const wchar_t* l = s.c_str();
    wprintf(L"%s\n",l);
}

void LogHelper::PrintWarings( const QString& text )
{
    QString str_texts = GetCurrentDateTimeString() + " Warings: " + text;

    qDebug() << str_texts;
}

void LogHelper::PrintErrors( const QString& text )
{
    QString str_texts = GetCurrentDateTimeString() + " Errors: " + text;

    qDebug() << str_texts;
}

