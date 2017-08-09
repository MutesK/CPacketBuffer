#pragma once

#include <iostream>
#include "MemoryPool.h"

/*---------------------------------------------------------------
Made By Mute

네트워크 패킷용 클래스. (직렬화 버퍼)
간편하게 패킷에 순서대로 데이타를 In, Out 한다. 

- 사용법.

CPacketBuffer cPacket;

넣기.
clPacket << 40030;	or	clPacket << iValue;	(int 넣기)
clPacket << 3;		or	clPacket << byValue;	(BYTE 넣기)
clPacket << 1.4;	or	clPacket << fValue;	(float 넣기)

빼기.
clPacket >> iValue;	(int 빼기)
clPacket >> byValue;	(BYTE 빼기)
clPacket >> fValue;	(float 빼기)

!.	삽입되는 데이타 FIFO 순서로 관리된다.
큐가 아니므로, 넣기(<<).빼기(>>) 를 혼합해서 사용하면 안된다.

----------------------------------------------------------------*/


class CPacketBuffer
{
private:

	/*---------------------------------------------------------------

	----------------------------------------------------------------*/
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 1460,		// 패킷의 기본 버퍼 사이즈.
		eHEADER_SIZE = 5
	};

	//////////////////////////////////////////////////////////////////////////
	// 생성자, 파괴자.
	//
	// Return:
	//////////////////////////////////////////////////////////////////////////

	~CPacketBuffer();


	//////////////////////////////////////////////////////////////////////////
	// 패킷  파괴.
	//
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	Release(void);


	//////////////////////////////////////////////////////////////////////////
	// 패킷 청소.
	//
	// Parameters: 없음.
	// Return: 없음.
	//////////////////////////////////////////////////////////////////////////
	void	ClearBuffer(void);


	//////////////////////////////////////////////////////////////////////////
	// 버퍼 사이즈 얻기.
	//
	// Parameters: 없음.
	// Return: (int)패킷 버퍼 사이즈 얻기.
	//////////////////////////////////////////////////////////////////////////
	int		GetBufferSize(void) { return m_iBufferSize; }


	int		GetHeaderDataUseSize(void) {
		return m_iHeaderDataSize;
	}


	//////////////////////////////////////////////////////////////////////////
	// 버퍼 포인터 얻기.
	//
	// Parameters: 없음.
	// Return: (char *)버퍼 포인터.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void) { return m_chpBuffer; }


	char	*GetReadBufferPtr(void) { return m_chpReadPos;  }

	char	*GetWriteBufferPtr(void) { return m_chpWritePos; }

	char	*GetHeaderPtr(void) { return m_chpHeaderPos; }

	//////////////////////////////////////////////////////////////////////////
	// 버퍼 Pos 이동. (음수이동은 안됨)
	// GetBufferPtr 함수를 이용하여 외부에서 강제로 버퍼 내용을 수정할 경우 사용. 
	//
	// Parameters: (int) 이동 사이즈.
	// Return: (int) 이동된 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);

	//////////////////////////////////////////////////////////////////////////
	//		헤더 함수
	//		페이로드에 이미 데이터를 넣은뒤에 처리해야된다.
	//////////////////////////////////////////////////////////////////////////
	void CommonSizeHeaderAlloc(char *pInData)			// 나중에 테스트가 필요하다
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
	// 패킷버퍼 / 버퍼 사이즈.
	//------------------------------------------------------------
	char*	m_chpBuffer;
	int		m_iBufferSize;

	//------------------------------------------------------------
	// 버퍼의 읽을 위치, 넣을 위치.
	//------------------------------------------------------------
	char	*m_chpReadPos;
	char	*m_chpWritePos;

	//------------------------------------------------------------
	// 현재 버퍼에 사용중인 사이즈.
	//------------------------------------------------------------
	int		m_iDataSize;

	char	*m_chpHeaderPos;
	int		m_iHeaderDataSize;

	//------------------------------------------------------------
	// 직렬화 버퍼용 레퍼런스 카운트
	//------------------------------------------------------------
	LONG64	m_ReferenceCount;


	bool	m_bAlloced;

	friend class CLanServer;

/////////////////////////////////////////////////////////////////////////////////////////
public:
	CPacketBuffer();
	CPacketBuffer(int iBufferSize);

	//////////////////////////////////////////////////////////////////////////
	// 레퍼런스 카운트 증가
	//////////////////////////////////////////////////////////////////////////
	void AddRefCount();

	//////////////////////////////////////////////////////////////////////////
	// 레퍼런스 카운트 감소하고 만약 0이되면 해제하는 함수
	//////////////////////////////////////////////////////////////////////////
	void Free();


	/* ============================================================================= */
	// 연산자 오퍼레이터.
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
	// 현재 사용중인 사이즈 얻기.
	//
	// Parameters: 없음.
	// Return: (int)사용중인 데이타 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetUseSize(void) { return m_iDataSize; }

	//////////////////////////////////////////////////////////////////////////
	// 데이타 얻기.
	//
	// Parameters: (char *)Dest 포인터. (int)Size.
	// Return: (int)복사한 사이즈.
	//////////////////////////////////////////////////////////////////////////
	int		GetData(char *chpDest, int iSize);

	//////////////////////////////////////////////////////////////////////////
	// 데이타 삽입.
	//
	// Parameters: (char *)Src 포인터. (int)SrcSize.
	// Return: (int)복사한 사이즈.
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


