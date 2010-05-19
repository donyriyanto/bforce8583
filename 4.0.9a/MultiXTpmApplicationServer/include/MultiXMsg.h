// MultiXMsg.h: interface for the CMultiXMsg class.
//
//////////////////////////////////////////////////////////////////////

/*

MultiX Network Applications Development Toolkit.
Copyright (C) 2007, Moshe Shitrit, Mitug Distributed Systems Ltd., All Rights Reserved.

This file is part of MultiX.

MultiX is free software; you can redistribute it and/or modify it under the terms of the 
GNU General Public License as published by the Free Software Foundation; 
either version 2 of the License, or (at your option) any later version. 

MultiX is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; 
without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. 
See the GNU General Public License for more details.
You should have received a copy of the GNU General Public License along with this program;
if not, write to the 
Free Software Foundation, Inc., 
59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
--------------------------------------------------------------------------------
Author contact information: 
msr@mitug.co.il
--------------------------------------------------------------------------------

*/


#if !defined(AFX_MULTIXMSG_H__B0E8290F_D7C9_48CD_96B6_221E0E66BF5D__INCLUDED_)
#define AFX_MULTIXMSG_H__B0E8290F_D7C9_48CD_96B6_221E0E66BF5D__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class MULTIXWIN32DLL_API	CMultiXMsg  
{

public:
	struct	THdr
	{
		uint32_t		Signature;
		int32_t			MsgCode;
		int32_t			Size;
		byte_t			Flags;
		byte_t			Layer;
 		uint16_t		Crc;
	};


private:


enum	TItemDataType
{
	ItemDataTypeNone		=	0,
	ItemDataTypeBYTE		=	1,
	ItemDataTypeCHAR		=	2,
	ItemDataTypeSTR			=	3,
	ItemDataTypeINT16		=	4,
	ItemDataTypeUINT16	=	5,
	ItemDataTypeINT32		=	6,
	ItemDataTypeUINT32	=	7,
	ItemDataTypeINT64		=	8,
	ItemDataTypeUINT64	=	9
};


private:
	// Data

	CMultiXBufferArray	m_Bufs;

	int						m_CurrentBuf;
	int32_t				m_PutOffset;
	int32_t				m_GetOffset;
	int32_t				m_ItemCode;
	int32_t				m_ItemSize;
	int32_t				m_ItemOffset;
	uchar_t				*m_ItemData;

	bool	m_bHdrValidated;
	bool	m_bMsgOK;
	bool	m_bHdrSwapped;
	bool	m_bDataSwapped;
	bool	m_bLockAllBuffers;






private:
	static	uint16_t	m_CrcTable[];
	int ItemDataType();
	void SwapHdr();
	void SwapDataItems();

	enum	MsgFlags
	{
		MultiXNativeSig	=	0x12345678,
		MultiXForeignSig	=	0x78563412,
		MsgResponseBits		=	0x1,
		MsgResponseShift	=	0,
		MsgPriorityBits		=	0x7,
		MsgPriorityShift	=	1
	};
	struct	THdr	m_Hdr;





	inline	bool		&HdrValidated(){return	m_bHdrValidated;}
	inline	bool		&HdrSwapped(){return	m_bHdrSwapped;}
	inline	bool		&DataSwapped(){return	m_bDataSwapped;}
	int32_t	CurrentPutOffset();
	inline	int32_t	&PutOffset(){return	m_PutOffset;}
	inline	int32_t	&GetOffset(){return	m_GetOffset;}
	inline	int32_t	&ItemCode(){return	m_ItemCode;}
	inline	int32_t	&ItemSize(){return	m_ItemSize;	}
	inline	int32_t	&ItemOffset(){return	m_ItemOffset;	}
	inline	uchar_t	*&ItemData(){return	m_ItemData;	}
	void		Initialize(CMultiXBuffer	&Buf);
	inline	THdr	&Hdr(){return	m_Hdr;}
	inline	CMultiXBuffer	*CurrentBuffer(){return	m_Bufs.GetBuffer(m_CurrentBuf);}

public:

//	void GetItemData(int &RetVal);
	void GetItemData(std::string &RetVal);
	void GetItemData(int32_t &RetVal);
	void GetItemData(uint32_t &RetVal);
	void GetItemData(int16_t &RetVal);
	void GetItemData(uint16_t &RetVal);
	void GetItemData(int64_t &RetVal);
#ifndef	NoUInt64
	void GetItemData(uint64_t &RetVal);
#endif
	void GetItemData(bool &RetVal);
	const char_t * OffsetToPtr(int32_t Offset);
	inline	int32_t	GetPutOffset(){return	m_PutOffset;}
	void SetPriority(int Value);
	int Priority();
	bool IsResponse();
	void SetAsResponse(bool Value);
	void RewindRead();
	std::string & Find(int32_t nItemCode,std::string &Data,bool bSave=true);
	const	uchar_t * Find(int32_t nItemCode,bool	bSave=true);
	void SetMsgOK(bool bValue=true);
	const	uchar_t * GetItemData();
	int32_t GetItemSize();
	int32_t GetItemCode();
	int32_t GetItemOffset();
	bool Next();
	inline	bool	MsgOK(){return	m_bMsgOK;}
	static bool IsValidHdr(CMultiXMsg::THdr &Hdr);
	int Append(const	char_t	*pBuf,int	Length);
	CMultiXMsg(CMultiXBuffer	&Buf);
	CMultiXMsg(const	CMultiXMsg &Original);
	CMultiXMsg(CMultiXApp	&Owner);
	int HdrSize();
	const	CMultiXBufferArray	&BuffersToSend();
	virtual ~CMultiXMsg();

	inline	uint32_t	&Signature(){return	m_Hdr.Signature;}
	inline	byte_t		&Layer(){return	m_Hdr.Layer;}
	inline	int32_t		&MsgCode(){return	m_Hdr.MsgCode;}
	inline	int32_t		&TotalSize(){return	m_Hdr.Size;}
	inline	uint16_t	&Crc(){return	m_Hdr.Crc;}
	inline	int32_t		Int32Align(int32_t N){return	N > 0 ? ((N-1)	&	0x7ffffffc) + 4 : N;}

	int32_t	AddItem(int32_t nItemCode);
	int32_t	AddItem(int32_t nItemCode,const	int32_t &nData);
	int32_t	AddItem(int32_t nItemCode,const	uint32_t &nData);
	int32_t	AddItem(int32_t nItemCode,const	int64_t &nData);
#ifndef	NoUInt64
	int32_t	AddItem(int32_t nItemCode,const	uint64_t &nData);
#endif
	int32_t	AddItem(int32_t nItemCode,const	int16_t &nData);
	int32_t	AddItem(int32_t nItemCode,const	uint16_t &nData);
	int32_t	AddItem(int32_t nItemCode,const char_t  *sData);
	int32_t	AddItem(int32_t	nItemCode,const	char_t *sData,bool	bTrunc);
	int32_t	AddItem(int32_t	nItemCode,const	std::string	&sData);
	int32_t	AddItem(int32_t	nItemCode,CMultiXBuffer	&Buf);
	int32_t	AddItem(int32_t	nItemCode,const	CMultiXBufferArray	&Bufs);
	int32_t	AddItem(int32_t nItemCode,const	uchar_t *pData, int32_t nSize);
	void SetHdrCrc();
	static	uint16_t	ComputeHdrCrc (THdr	&Hdr);
	void	LockBuffers();
	void	UnlockBuffers();






	CMultiXSemaphore	m_SendSem;

protected:	//	functions
	CMultiXMsg(byte_t	Layer,int32_t	MsgCode,CMultiXBuffer	&Buf);

	int32_t	AddItem(int32_t	nItemCode,CMultiXMsg::TItemDataType	ItemDataType,int32_t	nItemSize,const void	*pItemData,int32_t	Dummy);


};

#endif // !defined(AFX_MULTIXMSG_H__B0E8290F_D7C9_48CD_96B6_221E0E66BF5D__INCLUDED_)
