#include <string.h>
#ifndef _MFRC_LIB_H
#define _MFRC_LIB_H


#include "MFRC522.h"

class MFRC_Lib
{
public:
	MFRC_Lib(byte _csPin, byte _rstPin);

	void init();

	bool tag_detected();

	String read_tagNumber();

private:
	MFRC522 mfrc;
};

#endif // !_MFRC_LIB_H








