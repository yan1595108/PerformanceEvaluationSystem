#include "CommonFunction.h"

//求最大值以及所在位置
void Max(double * pData,int nLenData,double &dMaxVal,int &nMaxValIndex)
{
	dMaxVal = *pData;
	int i;
	for(i=1;i<nLenData;i++)
	{
		if(dMaxVal < *(pData + i))
		{
			dMaxVal = *(pData + i);
			nMaxValIndex = i;
		}
	}
}

//求最小值以及所在位置
void Min(double * pData,int nLenData,double &dMinVal,int &nMinValIndex)
{
	dMinVal = *pData;
	int i;
	for(i=1;i<nLenData;i++)
	{
		if(dMinVal > *(pData + i))
		{
			dMinVal = *(pData + i);
			nMinValIndex = i;
		}
	}
}

//计算校验和
unsigned char GetChecksum(unsigned char *pData,int nDataSize)
{
	unsigned int Checksum = 0;
	while(nDataSize)
	{
		Checksum += *pData++;
		nDataSize--;
	}
	while(Checksum>>8)
	{
		Checksum = (Checksum>>8) + (Checksum&0x00ff);
	}
	return (unsigned char)(~Checksum);
}