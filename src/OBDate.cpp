#include <cstdint>
#include <thanatos/OBDate.h>

OBDate::OBDate()
{
}

OBDate::~OBDate()
{
}

void OBDate::write(std::ostream& stream)
{
	OBChkHeader header = { OBType::Date, 
		                   1, 
		                   16 };
    
	stream.write((char*)&header, sizeof(OBChkHeader));
	uint32_t value = 0;
	value |= day << 8;
	value |= month << 16;
	value |= year << 24;
	stream.write((char*)&value, sizeof(uint32_t));
}

int OBDate::read(std::istream& stream)
{
	OBChkHeader header;
	stream.read((char*)&header, sizeof(OBChkHeader));

	if (header.type != OBType::Date)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBDate] Unexpected chunk type encountered. (%X)\n", header.type);
		return 0;
	}

	if (header.size != 16 || header.count != 1)
	{
		fprintf(OB_ERROR_OUTPUT, "[OBDate] Incorrect size or/and count. (%X)\n", header.type);
		return 0;
	}

	uint32_t value;
	stream.read((char*)&value, sizeof(uint32_t));

	day = (value >> 8) & 0xFF;
	month = (value >> 16) & 0xFF;
	year = (value >> 24) & 0xFF;

	return 1;
}

uint32_t OBDate::getSize()
{
	return 16;
}

uint32_t OBDate::getType()
{
	return OBType::Date;
}

std::string OBDate::asString()
{
	if (year > 255)
		return "Year value higher than 255";
	if (month > 12)
		return "Month value higher than 12";
	if (day > 31)
		return "Day value higher than 31";
	static const char* months[] =
	{
		"",
		"Jan",
		"Feb",
		"Mar",
		"Apr",
		"May",
		"Jun",
		"Jul",
		"Aug",
		"Sep",
		"Oct",
		"Nov",
		"Dec"
	};	

	char buffer[12];
	snprintf(buffer, 12, "%s %02d 2%03d", months[month], day, year);

	return std::string(buffer);
}