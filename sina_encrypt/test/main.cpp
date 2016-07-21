
#include <QApplication>
#include "sina_encrypt.h"
#include "qq_encrypt.h"
#include "QUrl"


int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

//     QString name = "bestsalt@163.com";
// 
//     sina_encrypt* encryptor = sina_encrypt::GetInstance();
// 
//     QString af_name = encryptor->EncryptName(name);
// 
//     QString su = encryptor->EncryptPassword("801027", "1451198731", "27S0Z5",
//       "EB2A38568661887FA180BDDB5CABD5F21C7BFD59C090CB2D245A87AC253062882729293E5506350508E7F9AA3BB77F4333231490F915F6D63C55FE2F08A49B353F444AD3993CACC02DB784ABBB8E42A9B1BBFFFB38BE18D78E87A0E41B9B8F73A928EE0CCEE1F6739884B9777E4FE9E88A1BBE495927AC4A799B3181D6442443");


    qq_encrypt* encryptor = qq_encrypt::GetInstance();

    QString user = "13500017785";
    QString password = "gao92534";
    QString verify_c = "!LSI";

    QString p = encryptor->EncryptPassword(password,"",verify_c);

    return a.exec();
}
