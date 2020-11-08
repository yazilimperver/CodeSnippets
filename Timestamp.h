#ifndef TIMESTAMP_H
#define TIMESTAMP_H

#include <cstdint>
#include <string>

class Timestamp
{
public:
	// Bu fonksiyon mevcut zamani epocha gore milisaniye cinsinden doner
	static uint64_t getTimestampInMsec();

	// Asagidaki fonksiyon mevcut zamani (: Tue Sep 27 14:21:13 2011) formatinda doner
	static std::string getTimeString();

	// Asagidaki fonksiyon mevcut zamani sadece rakamlar ile zaman etiketi seklinde doner
	static std::string getDetailedTimeString();

	// Girilen epoch'tan bu yana gecen milisaniye icin getDetailedTimeString formatinda zaman etiketini doner
	static std::string getDetailedTimeString(uint64_t msec);
};

#endif
