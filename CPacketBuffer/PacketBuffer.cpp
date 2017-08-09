
#include "PacketBuffer.h"

CMemoryPool<CPacketBuffer> CPacketBuffer::PacketPool;


CPacketBuffer::CPacketBuffer()
{
	m_chpBuffer = new char[eBUFFER_DEFAULT];
	ZeroMemory(m_chpBuffer, eBUFFER_DEFAULT);
	m_iDataSize = 0;
	m_iBufferSize = eBUFFER_DEFAULT;

	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
	m_ReferenceCount = 1;
}

CPacketBuffer::CPacketBuffer(int iBufferSize)
{
	m_chpBuffer = new char[iBufferSize];
	ZeroMemory(m_chpBuffer, iBufferSize);
	m_iDataSize = 0;
	m_iBufferSize = iBufferSize;


	m_chpReadPos = m_chpBuffer;
	m_chpWritePos = m_chpBuffer;
	m_ReferenceCount = 1;
}

CPacketBuffer::~CPacketBuffer()
{
	Release();
}


void CPacketBuffer::Release(void)
{
	ClearBuffer();
}
void	CPacketBuffer::ClearBuffer(void)
{
	m_chpReadPos = nullptr;
	m_chpWritePos = nullptr;

	m_chpReadPos = m_chpWritePos = m_chpBuffer + eHEADER_SIZE;

	m_iDataSize = 0;
	m_ReferenceCount = 1;
}

int	CPacketBuffer::MoveWritePos(int iSize)
{
	if (iSize < 0)
		return 0;

	int Usage = m_iBufferSize - (m_chpWritePos - m_chpReadPos);
	int CopiedSize = max(min(iSize, Usage), 0);
	m_chpWritePos += CopiedSize;
	m_iDataSize += CopiedSize;

	return CopiedSize;
}

int	CPacketBuffer::MoveReadPos(int iSize)
{
	if (iSize < 0)
		return 0;

	if (iSize > m_iDataSize)
		return 0;

	m_chpReadPos += iSize;
	m_iDataSize -= iSize;

	if (m_iDataSize == 0)
		ClearBuffer();

	return iSize;
}

int	CPacketBuffer::PeekData(char *chrSrc, int iSrcSize)
{
	int Usage = m_chpWritePos - m_chpReadPos;
	if (Usage == 0)
		return 0;

	int GetDataSize = max(min(iSrcSize, Usage), 0 );

	memcpy_s(chrSrc, GetDataSize, m_chpReadPos, GetDataSize);

	return GetDataSize;
}
int	CPacketBuffer::GetData(char *chpDest, int iSize)
{
	if (iSize > m_iDataSize)
		return 0;

	memcpy(chpDest, m_chpReadPos, iSize);
	m_chpReadPos += iSize;
	m_iDataSize -= iSize;

	if (m_iDataSize == 0)
		ClearBuffer();

	return iSize;
}

int	CPacketBuffer::PutData(char *chpSrc, int iSrcSize)
{
	if (m_chpWritePos + iSrcSize > m_chpBuffer + m_iBufferSize)
		return 0;

	memcpy(m_chpWritePos, chpSrc , iSrcSize);
	m_chpWritePos += iSrcSize;
	m_iDataSize += iSrcSize;
	
	return iSrcSize;
}

CPacketBuffer& CPacketBuffer::operator=(CPacketBuffer &clSrCPacketBuffer)
{
	if (m_chpBuffer != nullptr)
	{
		delete[] m_chpBuffer;
		m_chpBuffer = nullptr;
	}

	this->m_iDataSize = clSrCPacketBuffer.m_iDataSize;
	this->m_iBufferSize = clSrCPacketBuffer.m_iBufferSize;

	this->m_chpBuffer = new char[m_iBufferSize];
	
	memcpy(this->m_chpBuffer, clSrCPacketBuffer.m_chpBuffer, m_iBufferSize);

	int ReadIndex = clSrCPacketBuffer.m_chpReadPos - clSrCPacketBuffer.m_chpBuffer;
	this->m_chpReadPos = m_chpBuffer + ReadIndex;

	int WriteIndex = clSrCPacketBuffer.m_chpWritePos - clSrCPacketBuffer.m_chpBuffer;
	this->m_chpWritePos = m_chpBuffer + WriteIndex;

	return *this;
}

void CPacketBuffer::AddRefCount()
{
	InterlockedIncrement64(&m_ReferenceCount);
}

void CPacketBuffer::Free()
{
	if (InterlockedDecrement64(&m_ReferenceCount) == 0)
	{
		m_bAlloced = false;
		PacketPool.Free(this);
	}
}
/*
CPacketBuffer& CPacketBuffer::operator<<(BYTE byValue)
{
	PutData((char *)&byValue, 1);
	return *this;
}

CPacketBuffer& CPacketBuffer::operator<<(char chValue)
{
	PutData((char *)&chValue, 1);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator << (short shValue)
{
	PutData((char *)&shValue, 2);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator << (WORD wValue)
{
	PutData((char *)&wValue, 2);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator << (int iValue)
{
	PutData((char *)&iValue, 4);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator << (DWORD dwValue)
{
	PutData((char *)&dwValue, 4);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator << (float fValue)
{
	floatdata f;
	f.a= fValue;

	PutData(f.value, 4);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator << (__int64 iValue)
{
	PutData((char *)&iValue, 8);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator << (double dValue)
{
	doubledata d;
	d.a = dValue;

	PutData(d.value, 8);
	return *this;
}


CPacketBuffer	&CPacketBuffer::operator >> (BYTE &byValue)
{
	GetData((char *)&byValue, 1);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (char &chValue)
{
	GetData((char *)&chValue, 1);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator >> (short &shValue)
{
	GetData((char *)&shValue, 2);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (WORD &wValue)
{
	GetData((char *)&wValue, 2);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator >> (int &iValue)
{
	GetData((char *)&iValue, 4);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (DWORD &dwValue)
{
	GetData((char *)&dwValue, 4);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (float &fValue)
{
	floatdata f;
	GetData(f.value, 4);

	fValue = f.a;
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (UINT64 &iValue)
{
	GetData((char *)&iValue, 8);
	return *this;
}

CPacketBuffer	&CPacketBuffer::operator >> (__int64 &iValue)
{
	GetData((char *)&iValue, 8);
	return *this;
}
CPacketBuffer	&CPacketBuffer::operator >> (double &dValue)
{
	doubledata d;
	GetData(d.value, 8);

	dValue = d.a;

	return *this;
}
*/