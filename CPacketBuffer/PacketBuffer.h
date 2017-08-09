#pragma once

#include <iostream>
#include "MemoryPool.h"

/*---------------------------------------------------------------
Made By Mute

��Ʈ��ũ ��Ŷ�� Ŭ����. (����ȭ ����)
�����ϰ� ��Ŷ�� ������� ����Ÿ�� In, Out �Ѵ�. 

- ����.

CPacketBuffer cPacket;

�ֱ�.
clPacket << 40030;	or	clPacket << iValue;	(int �ֱ�)
clPacket << 3;		or	clPacket << byValue;	(BYTE �ֱ�)
clPacket << 1.4;	or	clPacket << fValue;	(float �ֱ�)

����.
clPacket >> iValue;	(int ����)
clPacket >> byValue;	(BYTE ����)
clPacket >> fValue;	(float ����)

!.	���ԵǴ� ����Ÿ FIFO ������ �����ȴ�.
ť�� �ƴϹǷ�, �ֱ�(<<).����(>>) �� ȥ���ؼ� ����ϸ� �ȵȴ�.

----------------------------------------------------------------*/


class CPacketBuffer
{
private:

	/*---------------------------------------------------------------

	----------------------------------------------------------------*/
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1460,		// ��Ŷ�� �⺻ ���� ������.
		eHEADER_SIZE = 5
	};

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Return:
	//////////////////////////////////////////////////////////////////////////

	~CPacketBuffer();


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ  �ı�.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);


	//////////////////////////////////////////////////////////////////////////
	// ��Ŷ û��.
	//
	// Parameters: ����.
	// Return: ����.
	//////////////////////////////////////////////////////////////////////////
	void	ClearBuffer(void);


	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int		GetBufferSize(void) { return m_iBufferSize; }


	int		GetHeaderDataUseSize(void) {
		return m_iHeaderDataSize;
	}


	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void) { return m_chpBuffer; }


	char	*GetReadBufferPtr(void) { return m_chpReadPos;  }

	char	*GetWriteBufferPtr(void) { return m_chpWritePos; }

	char	*GetHeaderPtr(void) { return m_chpHeaderPos; }

	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	//////////////////////////////////////////////////////////////////////////
	//		��� �Լ�
	//		���̷ε忡 �̹� �����͸� �����ڿ� ó���ؾߵȴ�.
	//////////////////////////////////////////////////////////////////////////
	void CommonSizeHeaderAlloc(char *pInData)			// ���߿� �׽�Ʈ�� �ʿ��ϴ�
	{
		if (m_chpBuffer == nullptr)
			return;

		m_chpReadPos -= 5;

		memcpy(m_chpReadPos, pInData, 5);

		m_iDataSize += 5;
	}

	void ShortSizeHeaderAlloc()
	{
		if (m_chpBuffer == nullptr)
			return;

		m_chpHeaderPos = m_chpReadPos - 2;
		char *pIndex = (char *)&m_iDataSize;

		*m_chpHeaderPos = *pIndex;
		*(m_chpHeaderPos + 1) = *(pIndex + 1);

		m_iHeaderDataSize = m_iDataSize + 2;
	}
	  

	void SettingBuffer(int Size = 4096)
	{
		m_bAlloced = true;

		if (m_chpBuffer != nullptr && m_iBufferSize == eBUFFER_DEFAULT)
		{
			ClearBuffer();
		}
		else
		{
			m_chpBuffer = new char[eBUFFER_DEFAULT];
			ZeroMemory(m_chpBuffer, eBUFFER_DEFAULT);
			m_iDataSize = 0;
			m_iBufferSize = eBUFFER_DEFAULT;

			m_chpReadPos = m_chpBuffer;
			m_chpWritePos = m_chpBuffer;
			m_ReferenceCount = 1;
		}

		
	}


	//------------------------------------------------------------
	// ��Ŷ���� / ���� ������.
	//------------------------------------------------------------
	char*	m_chpBuffer;
	int		m_iBufferSize;

	//------------------------------------------------------------
	// ������ ���� ��ġ, ���� ��ġ.
	//------------------------------------------------------------
	char	*m_chpReadPos;
	char	*m_chpWritePos;

	//------------------------------------------------------------
	// ���� ���ۿ� ������� ������.
	//------------------------------------------------------------
	int		m_iDataSize;

	char	*m_chpHeaderPos;
	int		m_iHeaderDataSize;

	//------------------------------------------------------------
	// ����ȭ ���ۿ� ���۷��� ī��Ʈ
	//------------------------------------------------------------
	LONG64	m_ReferenceCount;


	bool	m_bAlloced;

	friend class CLanServer;

/////////////////////////////////////////////////////////////////////////////////////////
public:
	CPacketBuffer();
	CPacketBuffer(int iBufferSize);

	//////////////////////////////////////////////////////////////////////////
	// ���۷��� ī��Ʈ ����
	//////////////////////////////////////////////////////////////////////////
	void AddRefCount();

	//////////////////////////////////////////////////////////////////////////
	// ���۷��� ī��Ʈ �����ϰ� ���� 0�̵Ǹ� �����ϴ� �Լ�
	//////////////////////////////////////////////////////////////////////////
	void Free();


	/* ============================================================================= */
	// ������ ���۷�����.
	/* ============================================================================= */
	CPacketBuffer	&operator = (CPacketBuffer &clSrCPacketBuffer);

	template<typename T>
	CPacketBuffer& operator<<(T& Value)
	{
		PutData(reinterpret_cast<char *>(&Value), sizeof(T));
		return *this;
	}


	template<typename T>
	CPacketBuffer& operator >> (T& Value)
	{
		GetData(reinterpret_cast<char *>(&Value), sizeof(T));
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)������� ����Ÿ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseSize(void) { return m_iDataSize; }

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ���.
	//
	// Parameters: (char *)Dest ������. (int)Size.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// ����Ÿ ����.
	//
	// Parameters: (char *)Src ������. (int)SrcSize.
	// Return: (int)������ ������.
	//////////////////////////////////////////////////////////////////////////
	int		PutData(char *chpSrc, int iSrcSize);


	int		PeekData(char *chrSrc, int iSrcSize);


	static CMemoryPool<CPacketBuffer> PacketPool;


	static CPacketBuffer* Alloc()
	{
		CPacketBuffer * pRet = PacketPool.Alloc();
		pRet->SettingBuffer();

		return pRet;
	}

	static void Free(CPacketBuffer *pBuffer)
	{
		pBuffer->Free();
	}
};


