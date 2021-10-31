#include <thanatos/OBSld.h>
#include <thanatos/OBTypes.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

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
					{
						d[dPos] = 0;
					}
					dPos++;
				}
			}
			else
			{
				uint32_t pos = dPos - offset;
				for (int i = 0; i < count; i++)
				{
					if (d)
					{
						d[dPos] = d[pos + i];
					}
					dPos++;
				}
			}
		}
		else
		{
			uint16_t val = s[sPos];
			sPos++;
			if (d)
			{
				d[dPos] = val;
			}
			dPos++;
		}
		flag = flag << 1;
		bLeft--;
	}
	return dPos * 2;
}

struct SearchInfo
{
	uint16_t* begin;
	uint16_t* end;
	uint32_t offset;
	uint32_t length;
};

static int __OBSldSearch(SearchInfo* info, uint16_t* pos)
{
	info->length = 0;
	uint16_t* src = info->begin;
	if ((pos - src) > 0x7FF)
		src = pos - 0x7FF;

	uint16_t* nextSrc = src;
	while (src < pos)
	{
		nextSrc += 1;
		if (*src == *pos)
		{
			uint16_t* nextPos = pos + 1;
			uint32_t length = 1;
			int64_t dif = (int64_t) (nextSrc - nextPos);
			while (length < 0xFFFF)
			{
				uint16_t val = *nextPos;
				nextPos += 1;
				if ((info->end <= nextPos) || *(nextPos + dif - 1) != val)
					break;
				length += 1;
			}

			if (length > info->length)
			{
				info->length = length;
				info->offset = (int32_t) (pos - src);
				if (length == 0xFFFF || 
					length == (int32_t) (info->end - pos))
					break;
			}
		}
		src += 1;
	}

	if (info->length > 1)
	{
		return 1;
	}
	else
		return 0;
};

uint32_t OBSld::compress(void* src, uint32_t srcSize, void* dst, uint32_t destSize)
{
	if (!destSize)
		return 0;
	uint16_t* s = (uint16_t*) src;
	uint16_t* sEnd = (uint16_t*) ((char*)src + srcSize);
	uint16_t* d = (uint16_t*) dst;
	uint16_t* dEnd = (uint16_t*) ((char*)dst + destSize);
	
	uint16_t* srcPos = s + 1;
	uint16_t* dstPos = d + 2;
	uint16_t* flag = d;
	d[1] = *s;

	uint16_t f = 0x4000;
	uint32_t v = 0;

	SearchInfo sInfo;
	sInfo.begin = s;
	sInfo.end = sEnd;

	while (true)
	{
		if (f == 0)
		{
			*flag = v;
			f = 0x8000;
			v = 0;
			flag = dstPos;
			dstPos += 1;
		}
		if (__OBSldSearch(&sInfo, srcPos) != 0)
		{
			v |= f;
			if (sInfo.length < 0x20)
			{
				srcPos += sInfo.length;
				*dstPos = (uint16_t) ((sInfo.length << 0xB) | (sInfo.offset & 0x7FF));
				dstPos += 1;
			}
			else
			{
				if (sInfo.length > 0x7FF)
					sInfo.length = 0x7FF;
				srcPos += sInfo.length;
				dstPos[0] = sInfo.offset & 0x7FF;
				dstPos[1] = sInfo.length;
				dstPos += 2;
			}
		}
		else
		{
			*dstPos = *srcPos;
			dstPos += 1;
			srcPos += 1;
		}

		f >>= 1;
		uint16_t pv = v;
		if (sEnd <= srcPos)
		{
			if (f == 0)
			{
				*flag = pv;
				pv = 0x8000;
				flag = dstPos;
				dstPos += 1;
			}
			else
			{
				pv = pv | f;
			}
			*dstPos = 0;
			dstPos[1] = 0;
			*flag = pv;
			return (uint32_t) ((dstPos - d) + 1) * 2;
		}
	}
}