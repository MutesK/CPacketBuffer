#pragma once

#include <Windows.h>

/*---------------------------------------------------------------


��Ʈ��ũ ��Ŷ�� Ŭ����.
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
public:

	/*---------------------------------------------------------------

	----------------------------------------------------------------*/
	enum en_PACKET
	{
		eBUFFER_DEFAULT = 3000		// ��Ŷ�� �⺻ ���� ������.
	};

	//////////////////////////////////////////////////////////////////////////
	// ������, �ı���.
	//
	// Return:
	//////////////////////////////////////////////////////////////////////////
	CPacketBuffer();
	CPacketBuffer(int iBufferSize);

	virtual	~CPacketBuffer();


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
	void	Clear(void);


	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)��Ŷ ���� ������ ���.
	//////////////////////////////////////////////////////////////////////////
	int		GetBufferSize(void) { return m_iBufferSize; }
	//////////////////////////////////////////////////////////////////////////
	// ���� ������� ������ ���.
	//
	// Parameters: ����.
	// Return: (int)������� ����Ÿ ������.
	//////////////////////////////////////////////////////////////////////////
	int		GetDataSize(void) { return m_iDataSize; }



	//////////////////////////////////////////////////////////////////////////
	// ���� ������ ���.
	//
	// Parameters: ����.
	// Return: (char *)���� ������.
	//////////////////////////////////////////////////////////////////////////
	char	*GetBufferPtr(void) { return m_chpBuffer; }


	char	*GetReadBufferPtr(void) { return m_chpReadPos;  }

	char	*GetWriteBufferPtr(void) { return m_chpWritePos; }
	//////////////////////////////////////////////////////////////////////////
	// ���� Pos �̵�. (�����̵��� �ȵ�)
	// GetBufferPtr �Լ��� �̿��Ͽ� �ܺο��� ������ ���� ������ ������ ��� ���. 
	//
	// Parameters: (int) �̵� ������.
	// Return: (int) �̵��� ������.
	//////////////////////////////////////////////////////////////////////////
	int		MoveWritePos(int iSize);
	int		MoveReadPos(int iSize);






	/* ============================================================================= */
	// ������ ���۷�����.
	/* ============================================================================= */
	CPacketBuffer	&operator = (CPacketBuffer &clSrCPacketBuffer);

	//////////////////////////////////////////////////////////////////////////
	// �ֱ�.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	/*
	CPacketBuffer	&operator << (BYTE byValue);
	CPacketBuffer	&operator << (char chValue);

	CPacketBuffer	&operator << (short shValue);
	CPacketBuffer	&operator << (WORD wValue);

	CPacketBuffer	&operator << (int iValue);
	CPacketBuffer	&operator << (DWORD dwValue);
	CPacketBuffer	&operator << (float fValue);

	CPacketBuffer	&operator << (__int64 iValue);
	CPacketBuffer	&operator << (double dValue);
	*/

	template<typename T>
	CPacketBuffer& operator<<(T& Value)
	{
		PutData(reinterpret_cast<char *>(&Value), sizeof(T));
		return *this;
	}

	//////////////////////////////////////////////////////////////////////////
	// ����.	�� ���� Ÿ�Ը��� ��� ����.
	//////////////////////////////////////////////////////////////////////////
	/*
	CPacketBuffer	&operator >> (BYTE &byValue);
	CPacketBuffer	&operator >> (char &chValue);

	CPacketBuffer	&operator >> (short &shValue);
	CPacketBuffer	&operator >> (WORD &wValue);

	CPacketBuffer	&operator >> (int &iValue);
	CPacketBuffer	&operator >> (DWORD &dwValue);
	CPacketBuffer	&operator >> (float &fValue);

	CPacketBuffer	&operator >> (__int64 &iValue);
	CPacketBuffer	&operator >> (UINT64 &iValue);
	CPacketBuffer	&operator >> (double &dValue);
	*/

	template<typename T>
	CPacketBuffer& operator>>(T& Value)
	{
		GetData(reinterpret_cast<char *>(&Value), sizeof(T));
		return *this;
	}


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




protected:

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


};


