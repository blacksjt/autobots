#include "utility_common.h"
namespace Utility {
	static char a[] = { 48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 97, 98, 99, 100, 101, 102 };

	static QString funDigest(const QByteArray& paramArrayOfByte, int paramInt1, int paramInt2)
	{
		if (paramArrayOfByte.isEmpty())
		{
			return "";
		}

		char* arrayOfChar = new char[paramInt2 * 2];
		int i = 0;
		int j = 0;
		while (i < paramInt2)
		{
			int k = paramArrayOfByte[(i + paramInt1)] & 0xFF;
			int m = j + 1;
			arrayOfChar[j] = a[(k >> 4)];
			j = m + 1;
			arrayOfChar[m] = a[(k & 0xF)];
			i += 1;
		}
		QString s = QString(arrayOfChar).left(paramInt2 * 2);

		if (arrayOfChar != NULL)
		{
			delete arrayOfChar;
			arrayOfChar = NULL;
		}

		return s;// new String(arrayOfChar, 0, paramInt2 * 2);
	}

	QString encodeTT(const QString& paramString)
	{

		if (paramString.isEmpty())
		{
			return "";
		}

		int i = 0;

		QByteArray localObject = paramString.toUtf8().data();
		while (i < localObject.length())
		{
			localObject[i] = ((localObject[i] ^ 0x5));
			i += 1;
		}
		QString res = funDigest(localObject, 0, localObject.length());
		return res;
	}


}