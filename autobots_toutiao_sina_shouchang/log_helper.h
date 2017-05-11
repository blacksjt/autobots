#ifndef LOG_HELPER_H
#define LOG_HELPER_H

#include <QString>

//#include "yilicart_global.h"

class LogHelper
{
public:
    static void PrintInformations(const QString& text);
    static void PrintWarings(const QString& text);
    static void PrintErrors(const QString& text);
};

#endif // LOG_HELPER_H
