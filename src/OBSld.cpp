#include <thanatos/OBSld.h>
#include <cstdio>

uint32_t OBSld::decompress(void* src, uint32_t srcSize, void* dest, uint32_t destSize)
{
	uint16_t* s = (uint16_t*) src;
	uint16_t* d = (uint16_t*) dest;
	uint32_t sPos = 0;
	uint32_t sSize = srcSize / 2;
	uint32_t dPos = 0;
	uint32_t dSize = destSize / 2;
	uint32_t bLeft = 0;
	uint16_t flag = 0;
	while (sPos < sSize && s)
	{
		if (bLeft == 0)
		{
			flag = s[sPos];
			sPos++;
			bLeft = 16;
		}
		
		if (flag & 0x8000)
		{
			uint16_t mode = s[sPos];
			sPos++;
			
			uint16_t count = mode >> 0xb;
			if (count == 0)
			{
				count = s[sPos];
				sPos++;
			}
			
			uint16_t offset = mode & 0x7ff;
			if (offset == 0)
			{
				for (int i = 0; i < count; i++)
				{
					if (d)
						d[dPos] = 0;
					dPos++;
				}
			}
			else
			{
				uint32_t pos = dPos - offset;
				for (int i = 0; i < count; i++)
				{
					if (d)
						d[dPos] = d[pos + i];
					dPos++;
				}
			}
		}
		else
		{
			uint16_t val = s[sPos];
			sPos++;
			if (d)
				d[dPos] = val;
			dPos++;
		}
		flag = flag << 1;
		bLeft--;
	}
	return dPos * 2;
}

#define CMP_PUSH_REF_TOKEN(offset, count) *d = ((offset) & 0x7FF | (((count) & 0x1F) << 0xb)); d++;
#define CMP_PUSH_VALUE(value) *d = (value); d++;

/*
	Could be improved
	Compression rate isn't too good :)
*/
uint32_t OBSld::compress(void* src, uint32_t srcSize, void* dst, uint32_t destSize)
{
	uint16_t* s = (uint16_t*)src;
	uint16_t* d = (uint16_t*)dst;
	uint16_t* sEnd = s + (srcSize / 2);
	uint16_t* dEnd = d + (destSize / 2);
	
	uint16_t* sc = s;
	int tokenId = 0;
	uint16_t* flag = d;
	*flag = 0;
	d += 1;
	while (sc < sEnd && d < dEnd)
	{
		int64_t dif = ((int64_t)sc - (int64_t)s) / 2;
		int offset = (int)(dif < 2047 ? dif : 2047);
		while (*sc != *(sc - offset) && offset > 0)
			offset--;

		if (offset > 0)
		{
			int count = 0;
			while (*(sc + count) == *((sc - offset) + count) && count < 31)
				count++;

			CMP_PUSH_REF_TOKEN(offset, count);
			sc += count;
			*flag = (*flag) | (1 << (15 - tokenId));
			tokenId++;
		}
		else
		{
			CMP_PUSH_VALUE(*sc);
			sc += 1;
			tokenId++;
		}
		
		if (tokenId > 15)
		{
			tokenId = 0;
			flag = d;
			*flag = 0;
			d += 1;
		}
	}
	
	while (tokenId < 15)
	{
		CMP_PUSH_VALUE(0);
		tokenId++;
	}
	return (uint32_t)((d - ((uint16_t*)dst)) * 2);
}