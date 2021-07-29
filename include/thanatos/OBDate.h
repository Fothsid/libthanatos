#pragma once
#include "OBObject.h"

class OBDate : public OBObject
{
public:
	OBDate();
	virtual ~OBDate();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::string asString();	

	int year;
	int month;
	int day;
};