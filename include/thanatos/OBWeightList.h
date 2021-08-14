#pragma once
#include "OBObject.h"

struct OBWeight
{
	uint32_t id;
	float weight;
};

inline bool operator==(const OBWeight& l, const OBWeight& r)
{
	return l.id == r.id && l.weight == r.weight;
}

inline bool operator==(const std::vector<OBWeight>& l, const std::vector<OBWeight>& r)
{
	if (l.size() != r.size())
		return false;

	for (int i = 0; i < l.size(); i++)
	{
		if (!(l[i] == r[i]))
			return false;
	}
	
	return true;
}

class OBWeightList : public OBObject
{
public:
	OBWeightList();
	virtual ~OBWeightList();
	virtual void write(std::ostream& stream) override;
	virtual int read(std::istream& stream) override;
	virtual uint32_t getSize() override;
	virtual uint32_t getType() override;

	std::vector<std::vector<OBWeight>> list;
};