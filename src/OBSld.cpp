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

#define CMP_PUSH_REF_TOKEN(x, y) *d = ((x) & 0x7FF | (((y) & 0x1F) << 0xb)); d++;
#define CMP_PUSH_VALUE(v) *d = (v); d++;
#define CMP_TAG_AS_REFERENCE() *flag = (*flag) | (1 << (15 - tokenId));
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define MIN(a, b) ((a) < (b) ? (a) : (b))

uint32_t OBSld::compress(void* src, uint32_t srcSize, void* dst, uint32_t destSize)
{
	int16_t* table = (int16_t*) malloc(2048*2048*2);
	uint16_t* s = (uint16_t*)src;
	uint16_t* d = (uint16_t*)dst;
	uint16_t* sEnd = s + (srcSize / 2);
	uint16_t* dEnd = d + (destSize / 2);
	int dataSize = srcSize / 2;
	
	uint16_t* sc = s;
	int tokenId = 0;
	uint16_t* flag = d;
	*flag = 0;
	d += 1;
	while (sc < sEnd && d < dEnd)
	{
		int srcPos = sc - s;
		int offset = 0, count = 0;
		int lookStartPos = srcPos - 2047;
		if (lookStartPos < 0)
			lookStartPos = 0;

		int lookPos = srcPos-1;
		while (lookStartPos < lookPos)
		{
			int i = 0;
			while ((i < 0xffff && 
				   (srcPos + i <= dataSize)) && 
				   (s[srcPos + i] == s[lookPos + i]))
				i++;
			if (count < i)
			{
				count = i;
				offset = srcPos - lookPos;
			}
			if (i == 0xffff || (dataSize < srcPos + i))
				break;
			lookPos--;
		}

		if (count < 32)
		{
			if (count > 1)
			{
				CMP_PUSH_REF_TOKEN(offset, count);
				CMP_TAG_AS_REFERENCE()
				sc += count;
			}
			else
			{
				CMP_PUSH_VALUE(*sc);
				sc += 1;
			}
			tokenId++;
		}
		else
		{
			CMP_PUSH_REF_TOKEN(offset, 0);
			CMP_PUSH_VALUE(count);
			CMP_TAG_AS_REFERENCE()
			sc += count;
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
	
	if (tokenId == 0)
	{
		CMP_PUSH_VALUE(0x8000);
		CMP_PUSH_VALUE(0);
		CMP_PUSH_VALUE(0);
		tokenId++;
	}
	else
	{
		CMP_PUSH_VALUE(0);
		CMP_PUSH_VALUE(0);
		*flag = (*flag) | (1 << (15 - tokenId));
	}

	uint32_t resultSize = (uint32_t)((d - ((uint16_t*)dst)) * 2);
#ifdef _DEBUG
	/* Report compression efficiency */
	fprintf(OB_ERROR_OUTPUT, "---=== Compression efficiency report ===---\n");
	fprintf(OB_ERROR_OUTPUT, "* source size:       %d\n", srcSize);
	fprintf(OB_ERROR_OUTPUT, "* compressed size:   %d\n", resultSize);
	fprintf(OB_ERROR_OUTPUT, "* compression ratio: %f\n", ((float)srcSize)/((float)resultSize));
	fprintf(OB_ERROR_OUTPUT, "* space saving:      %f\n", 1.0f-((float)resultSize)/((float)srcSize));
	fprintf(OB_ERROR_OUTPUT, "-------------------------------------------\n");

	FILE* fp = fopen("src.bin", "wb");
	fwrite(src, srcSize, 1, fp);
	fclose(fp);
#endif
	free(table);
	return resultSize;
}