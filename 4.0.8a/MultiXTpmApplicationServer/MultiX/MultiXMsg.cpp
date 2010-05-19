// MultiXMsg.cpp: implementation of the CMultiXMsg class.
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
#include	"StdAfx.h"

uint16_t	CMultiXMsg::m_CrcTable[] =
{
 /* 0x00 - */  0x0000 ,0xc0c1 ,0xc181 ,0x0140 ,0xc301 ,0x03c0 ,0x0280 ,0xc241
 /* 0x08 - */ ,0xc601 ,0x06c0 ,0x0780 ,0xc741 ,0x0500 ,0xc5c1 ,0xc481 ,0x0440
 /* 0x10 - */ ,0xcc01 ,0x0cc0 ,0x0d80 ,0xcd41 ,0x0f00 ,0xcfc1 ,0xce81 ,0x0e40
 /* 0x18 - */ ,0x0a00 ,0xcac1 ,0xcb81 ,0x0b40 ,0xc901 ,0x09c0 ,0x0880 ,0xc841
 /* 0x20 - */ ,0xd801 ,0x18c0 ,0x1980 ,0xd941 ,0x1b00 ,0xdbc1 ,0xda81 ,0x1a40
 /* 0x28 - */ ,0x1e00 ,0xdec1 ,0xdf81 ,0x1f40 ,0xdd01 ,0x1dc0 ,0x1c80 ,0xdc41
 /* 0x30 - */ ,0x1400 ,0xd4c1 ,0xd581 ,0x1540 ,0xd701 ,0x17c0 ,0x1680 ,0xd641
 /* 0x38 - */ ,0xd201 ,0x12c0 ,0x1380 ,0xd341 ,0x1100 ,0xd1c1 ,0xd081 ,0x1040
 /* 0x40 - */ ,0xf001 ,0x30c0 ,0x3180 ,0xf141 ,0x3300 ,0xf3c1 ,0xf281 ,0x3240
 /* 0x48 - */ ,0x3600 ,0xf6c1 ,0xf781 ,0x3740 ,0xf501 ,0x35c0 ,0x3480 ,0xf441
 /* 0x50 - */ ,0x3c00 ,0xfcc1 ,0xfd81 ,0x3d40 ,0xff01 ,0x3fc0 ,0x3e80 ,0xfe41
 /* 0x58 - */ ,0xfa01 ,0x3ac0 ,0x3b80 ,0xfb41 ,0x3900 ,0xf9c1 ,0xf881 ,0x3840
 /* 0x60 - */ ,0x2800 ,0xe8c1 ,0xe981 ,0x2940 ,0xeb01 ,0x2bc0 ,0x2a80 ,0xea41
 /* 0x68 - */ ,0xee01 ,0x2ec0 ,0x2f80 ,0xef41 ,0x2d00 ,0xedc1 ,0xec81 ,0x2c40
 /* 0x70 - */ ,0xe401 ,0x24c0 ,0x2580 ,0xe541 ,0x2700 ,0xe7c1 ,0xe681 ,0x2640
 /* 0x78 - */ ,0x2200 ,0xe2c1 ,0xe381 ,0x2340 ,0xe101 ,0x21c0 ,0x2080 ,0xe041
 /* 0x80 - */ ,0xa001 ,0x60c0 ,0x6180 ,0xa141 ,0x6300 ,0xa3c1 ,0xa281 ,0x6240
 /* 0x88 - */ ,0x6600 ,0xa6c1 ,0xa781 ,0x6740 ,0xa501 ,0x65c0 ,0x6480 ,0xa441
 /* 0x90 - */ ,0x6c00 ,0xacc1 ,0xad81 ,0x6d40 ,0xaf01 ,0x6fc0 ,0x6e80 ,0xae41
 /* 0x98 - */ ,0xaa01 ,0x6ac0 ,0x6b80 ,0xab41 ,0x6900 ,0xa9c1 ,0xa881 ,0x6840
 /* 0xa0 - */ ,0x7800 ,0xb8c1 ,0xb981 ,0x7940 ,0xbb01 ,0x7bc0 ,0x7a80 ,0xba41
 /* 0xa8 - */ ,0xbe01 ,0x7ec0 ,0x7f80 ,0xbf41 ,0x7d00 ,0xbdc1 ,0xbc81 ,0x7c40
 /* 0xb0 - */ ,0xb401 ,0x74c0 ,0x7580 ,0xb541 ,0x7700 ,0xb7c1 ,0xb681 ,0x7640
 /* 0xb8 - */ ,0x7200 ,0xb2c1 ,0xb381 ,0x7340 ,0xb101 ,0x71c0 ,0x7080 ,0xb041
 /* 0xc0 - */ ,0x5000 ,0x90c1 ,0x9181 ,0x5140 ,0x9301 ,0x53c0 ,0x5280 ,0x9241
 /* 0xc8 - */ ,0x9601 ,0x56c0 ,0x5780 ,0x9741 ,0x5500 ,0x95c1 ,0x9481 ,0x5440
 /* 0xd0 - */ ,0x9c01 ,0x5cc0 ,0x5d80 ,0x9d41 ,0x5f00 ,0x9fc1 ,0x9e81 ,0x5e40
 /* 0xd8 - */ ,0x5a00 ,0x9ac1 ,0x9b81 ,0x5b40 ,0x9901 ,0x59c0 ,0x5880 ,0x9841
 /* 0xe0 - */ ,0x8801 ,0x48c0 ,0x4980 ,0x8941 ,0x4b00 ,0x8bc1 ,0x8a81 ,0x4a40
 /* 0xe8 - */ ,0x4e00 ,0x8ec1 ,0x8f81 ,0x4f40 ,0x8d01 ,0x4dc0 ,0x4c80 ,0x8c41
 /* 0xf0 - */ ,0x4400 ,0x84c1 ,0x8581 ,0x4540 ,0x8701 ,0x47c0 ,0x4680 ,0x8641
 /* 0xf8 - */ ,0x8201 ,0x42c0 ,0x4380 ,0x8341 ,0x4100 ,0x81c1 ,0x8081 ,0x4040
};





//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXMsg::CMultiXMsg(byte_t	LayerInd,int32_t	iMsgCode,CMultiXBuffer	&Buf)
{
	Initialize(Buf);
	PutOffset()	=	HdrSize();
	TotalSize()	=	PutOffset();
	CurrentBuffer()->Expand(PutOffset(),true,false);
	Layer()	=	LayerInd;
	MsgCode()	=	iMsgCode;
	Signature()	=	MultiXNativeSig;
}

CMultiXMsg::CMultiXMsg(CMultiXBuffer	&Buf)
{
	Initialize(Buf);
}
CMultiXMsg::CMultiXMsg(CMultiXApp	&Owner)
{
	CMultiXBuffer	*pB	=	Owner.AllocateBuffer();
	Initialize(*pB);
	pB->ReturnBuffer();

}

CMultiXMsg::CMultiXMsg(const CMultiXMsg &Original)
{
	UnlockBuffers();
	for(int	I=0;I<Original.m_Bufs.Count();I++)
		m_Bufs.Add(*Original.m_Bufs.GetBuffer(I));

	m_Hdr							=	Original.m_Hdr;
	m_CurrentBuf			=	Original.m_CurrentBuf		;
	m_PutOffset				=	Original.m_PutOffset			;
	m_GetOffset				=	Original.m_GetOffset			;
	m_ItemCode				=	Original.m_ItemCode			;
	m_ItemSize				=	Original.m_ItemSize			;
	m_ItemOffset			=	Original.m_ItemOffset		;
	m_ItemData				=	Original.m_ItemData			;
	m_bHdrValidated		=	Original.m_bHdrValidated	;
	m_bMsgOK					=	Original.m_bMsgOK				;
	m_bHdrSwapped			=	Original.m_bHdrSwapped		;
	m_bDataSwapped		=	Original.m_bDataSwapped	;
	m_bLockAllBuffers	=	Original.m_bLockAllBuffers;

}


void	CMultiXMsg::Initialize(CMultiXBuffer	&Buf)
{
	memset(&m_Hdr,0,sizeof(m_Hdr));
	m_bLockAllBuffers	=	false;
	
	m_CurrentBuf	=	0;
	m_Bufs.Add(Buf);

	PutOffset()	=	HdrSize();
	if(Buf.Length()	<	HdrSize())
		Buf.Expand(HdrSize(),true,false);
	GetOffset()	=	0;
	HdrValidated()	=	false;
	HdrSwapped()	=	false;
	DataSwapped()	=	false;
	SetMsgOK(false);
}


CMultiXMsg::~CMultiXMsg()
{
	UnlockBuffers();
}

void	CMultiXMsg::LockBuffers()
{
	if(m_bLockAllBuffers)
		return;
	for(int	I=0;I<m_Bufs.Count();I++)
		m_Bufs.GetBuffer(I)->LockData();
	m_bLockAllBuffers	=	true;
}
void	CMultiXMsg::UnlockBuffers()
{
	if(!m_bLockAllBuffers)
		return;
	for(int	I=0;I<m_Bufs.Count();I++)
		m_Bufs.GetBuffer(I)->Unlock();
	m_bLockAllBuffers	=	false;
}




//This method appends a new data item to the current message. It is called by all other
//overloaded versions of 'AddItem'.

int32_t	CMultiXMsg::CurrentPutOffset()
{
	return	CurrentBuffer()->Length();
}



int32_t	CMultiXMsg::AddItem(int32_t	nItemCode,CMultiXMsg::TItemDataType	ItemDataType,int32_t	nItemSize,const void	*pItemData,int32_t	Dummy)
{
  if(nItemSize	<	0)	return(-1);
	if(nItemSize	>	0	&&	!pItemData)	return(-1);
	

	int32_t	NewLength	=	Int32Align(CurrentBuffer()->Length()+sizeof(nItemCode)+sizeof(nItemSize)+nItemSize);

	CurrentBuffer()->Expand(NewLength,false,true);

	int32_t	*Ptr	=	(int32_t	*)&CurrentBuffer()->LockData()[CurrentPutOffset()];

	Ptr[0]	=	nItemCode	|	(ItemDataType	<<	28);
	if(nItemSize	>	0)
	{
		Ptr[1]	=	nItemSize;
	}	else
	{
		Ptr[1]	=	0;
	}
	switch(nItemSize)
	{
		case	0	:
			break;
		case	sizeof(uchar_t)	:
			((uchar_t	*)&Ptr[2])[0]	=	((uchar_t	*)pItemData)[0];
			break;
		case	sizeof(uint16_t)	:
			((uint16_t	*)&Ptr[2])[0]	=	((uint16_t	*)pItemData)[0];
			break;
		case	sizeof(uint32_t)	:
			((uint32_t	*)&Ptr[2])[0]	=	((uint32_t	*)pItemData)[0];
			break;
			/*
#ifdef	NoUInt64
		case	sizeof(int64_t)	:
			((int64_t	*)&Ptr[2])[0]	=	((int64_t	*)pItemData)[0];
			break;
#else
		case	sizeof(uint64_t)	:
			((uint64_t	*)&Ptr[2])[0]	=	((uint64_t	*)pItemData)[0];
			break;
#endif
			*/
		default	:
			memcpy(&Ptr[2],pItemData,nItemSize);
	}

	int32_t	RetVal	=	PutOffset()	+	sizeof(Ptr[0])	+	sizeof(Ptr[1]);
	TotalSize()	+=	Int32Align(sizeof(nItemCode)+sizeof(nItemSize)+nItemSize);
	PutOffset()			=	TotalSize();
	CurrentBuffer()->ReleaseData(NewLength);
  return RetVal;
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode, CMultiXBuffer	&Buf)
{
	CMultiXBufferArray	Bufs;
	Bufs.Add(Buf);
	return	AddItem(nItemCode,Bufs);

}
int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const CMultiXBufferArray	&Bufs)
{
	if(Bufs.Count()	==	0)
		return	-1;

	int32_t	SavedLength	=	CurrentBuffer()->Length();
	int32_t	NewLength	=	Int32Align(CurrentBuffer()->Length()+sizeof(int32_t)*2);

	CurrentBuffer()->Expand(NewLength,false,true);

	TotalSize()	+=	(NewLength-SavedLength);

	int32_t	*Ptr	=	(int32_t	*)&CurrentBuffer()->LockData()[CurrentPutOffset()];

	Ptr[0]	=	nItemCode	|	(ItemDataTypeBYTE	<<	28);
	Ptr[1]	=	Bufs.DataSize();
	CurrentBuffer()->ReleaseData(NewLength);
	for(int	I=0;I	<	Bufs.Count();I++)
	{
		m_CurrentBuf++;
		m_Bufs.Add(*Bufs.GetBuffer(I));
		if(m_bLockAllBuffers)
			CurrentBuffer()->LockData();
	}

	CurrentBuffer()->Expand(Int32Align(CurrentBuffer()->Length()),true,true);
	TotalSize()	+=	Bufs.DataSize();

	int32_t	RetVal	=	PutOffset()	+	sizeof(Ptr[0])	+	sizeof(Ptr[1]);
	PutOffset()			=	TotalSize();
  return RetVal;
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode)
{
  return(AddItem(nItemCode,ItemDataTypeNone,0,NULL,0));
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	int32_t &Data)
{
  return(AddItem(nItemCode, ItemDataTypeINT32,sizeof(Data), &Data,0));
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	uint32_t &Data)
{
  return(AddItem(nItemCode, ItemDataTypeUINT32,sizeof(Data), &Data,0));
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	int64_t &Data)
{
  return(AddItem(nItemCode, ItemDataTypeINT64,sizeof(Data), &Data,0));
}

#ifndef	NoUInt64
int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	uint64_t &Data)
{
  return(AddItem(nItemCode, ItemDataTypeUINT64,sizeof(Data), &Data,0));
}
#endif

int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	int16_t &Data)
{
  return(AddItem(nItemCode,ItemDataTypeINT16, sizeof(Data), &Data,0));
}

int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const	uint16_t &Data)
{
  return(AddItem(nItemCode,ItemDataTypeUINT16, sizeof(Data), &Data,0));
}


int32_t	CMultiXMsg::AddItem(int32_t nItemCode, const char_t  *sData)
{
	if(!sData)	return(-1);
  return(AddItem(nItemCode, ItemDataTypeSTR,(int32_t)strlen((const	char	*)sData)+1, sData,0));
}

int32_t	CMultiXMsg::AddItem(int32_t ItemCode,  const	char_t *  sData, bool bTrunc)
{
	if(!sData)	return(-1);

	int32_t	nLen	=	(int32_t)strlen((const	char	*)sData);
	if(bTrunc)
	{
		std::string	S	=	(const	char	*)sData;
		while(nLen>0	&&	isspace(S[nLen-1]))	nLen--;
		S[nLen]	=	0;
		return    AddItem(ItemCode,ItemDataTypeSTR, nLen+1, S.c_str(),0);

	}
	return    AddItem(ItemCode,ItemDataTypeSTR, nLen+1, sData,0);
}

int32_t	CMultiXMsg::AddItem(int32_t ItemCode,  const	std::string	&sData)
{
	return	AddItem(ItemCode,ItemDataTypeSTR, (int32_t)sData.length()+1, sData.c_str(),0);
}


int32_t	CMultiXMsg::AddItem( int32_t nItemCode, const	uchar_t * pData, int32_t nSize)
{
	if(!pData)	return(-1);
  return( AddItem( nItemCode, ItemDataTypeBYTE, nSize, pData, 0));
}


void CMultiXMsg::SetHdrCrc()
{

	int	I;
	uint16_t	Crc	=	0;
	byte_t *	Ptr	=	(byte_t *)&m_Hdr;
	m_Hdr.Crc	=	0;
	for(I=0;I	<	sizeof(m_Hdr)-sizeof(m_Hdr.Crc);I++)
	{
		Crc	=	(uint16_t)((Crc>>8)^m_CrcTable[(((Crc)^((uint16_t)(*Ptr)))&0x0ff)]);
		Ptr++;
	}
	((byte_t *)&m_Hdr.Crc)[0]	=	(byte_t)(Crc	&	0xff);
	((byte_t *)&m_Hdr.Crc)[1]	=	(byte_t)(Crc	>>	8);

}

uint16_t CMultiXMsg::ComputeHdrCrc (CMultiXMsg::THdr	&Hdr)
{ 

	int	I;
	uint16_t	Crc	=	0;
	byte_t *	Ptr	=	(byte_t *)&Hdr;
	for(I=0;I	<	sizeof(Hdr);I++)
	{
		Crc	=	(uint16_t)((Crc>>8)^m_CrcTable[(((Crc)^((uint16_t)(*Ptr)))&0x0ff)]);
		Ptr++;
	}
	return(Crc);

}//end VdoMgrMessager::ComputeHeaderCrc () const

const	CMultiXBufferArray &CMultiXMsg::BuffersToSend()
{
	SetHdrCrc();
	memcpy(m_Bufs.GetBuffer(0)->LockData(),&m_Hdr,sizeof(m_Hdr));
	m_Bufs.GetBuffer(0)->Unlock();
	return	m_Bufs;
}

int CMultiXMsg::HdrSize()
{
	return	Int32Align(sizeof(m_Hdr));
}

int CMultiXMsg::Append(const	char_t	*pBuf,int	Length)
{
	int	CopyCount=0;
	if(HdrValidated())
	{
		if(TotalSize() <=	CurrentBuffer()->Length())
			return	0;
		CopyCount	=	MINMac(TotalSize()-CurrentBuffer()->Length(),Length);
		if(CopyCount	>	0)
		{
			int	Offset	=	CurrentBuffer()->Length();
			CurrentBuffer()->Expand(CopyCount+Offset,true,true);
			memcpy(&CurrentBuffer()->LockData()[Offset],pBuf,CopyCount);
			CurrentBuffer()->ReleaseData(CopyCount+Offset);
		}
		if(TotalSize()	==	CurrentBuffer()->Length())
		{
			SetMsgOK();
			if(HdrSwapped())
				SwapDataItems();
		}
	}	else
	{
		CopyCount	=	MINMac(HdrSize()-TotalSize(),Length);
		if(CopyCount > 0)
		{
			memcpy(&CurrentBuffer()->LockData()[TotalSize()],pBuf,CopyCount);
			TotalSize()+=CopyCount;
			CurrentBuffer()->ReleaseData(TotalSize());
		}
		if(HdrSize()	==	TotalSize())
		{
			if(IsValidHdr(*(THdr	*)CurrentBuffer()->DataPtr()))
			{
				memcpy(&Hdr(),CurrentBuffer()->DataPtr(),HdrSize());
				if(Signature()	!=	MultiXNativeSig)
				{
					SwapHdr();
				}
				m_bHdrValidated	=	true;
				if(TotalSize()	==	HdrSize())	//	a	header only msg
				{
					SetMsgOK();
				}
				if(Length	>	CopyCount)
					return	CopyCount	+	Append(pBuf+CopyCount,Length-CopyCount);
			}	else
				return	-1;
		}
	}
	return	CopyCount;
}


bool CMultiXMsg::IsValidHdr(CMultiXMsg::THdr &Hdr)
{
	if(Hdr.Signature	!=	MultiXNativeSig	&&
			Hdr.Signature	!=	MultiXForeignSig)
		return	false;

	if(ComputeHdrCrc(Hdr))
		return	false;
	return	true;
}

void CMultiXMsg::SwapDataItems()
{

	if(DataSwapped())	return;

	int32_t	Offset	=	HdrSize();
	uint32_t	*UInt32Ptr;
	char_t	*DataPtr	=	CurrentBuffer()->LockData();


	while(Offset	<	TotalSize())
	{
		UInt32Ptr	=	(uint32_t *)&DataPtr[Offset];
		CMultiXUtil::SwapInt(UInt32Ptr[0]);
		CMultiXUtil::SwapInt(UInt32Ptr[1]);
		if(UInt32Ptr[1]	>	0)
		{
			switch(UInt32Ptr[0]	>>	28)
			{
				case	ItemDataTypeINT16	:
				case	ItemDataTypeUINT16	:
				{
					CMultiXUtil::SwapInt(*((uint16_t	*)&UInt32Ptr[2]));
				}
				break;
				case	ItemDataTypeINT32	:
				case	ItemDataTypeUINT32	:
				{
					CMultiXUtil::SwapInt(UInt32Ptr[2]);
				}
				break;
				case	ItemDataTypeINT64	:
				case	ItemDataTypeUINT64	:
				{
					CMultiXUtil::SwapInt(*((int64_t	*)&UInt32Ptr[2]));
				}
				break;
			}
		}
		Offset	+=	sizeof(int32_t)*2	+	Int32Align(UInt32Ptr[1]);
	}

	DataSwapped()	=	true;
	CurrentBuffer()->Unlock();
}

void CMultiXMsg::SwapHdr()
{
	if(HdrSwapped())
		return;
//	CMultiXUtil::SwapInt(Hdr().Layer);
	CMultiXUtil::SwapInt(Hdr().MsgCode);
	CMultiXUtil::SwapInt(Hdr().Signature);
	CMultiXUtil::SwapInt(Hdr().Size);

	HdrSwapped()	=	true;
}

bool CMultiXMsg::Next()
{
	if(GetOffset()	==	0)
		GetOffset()	=	HdrSize();
	if(GetOffset()	>=	TotalSize())	
	{
		ItemCode()	=	-1;
		ItemSize()	=	-1;
		ItemOffset()	=	-1;
		ItemData()	=	NULL;
		return	false;
	}

	uint32_t	*Ptr	=	(uint32_t	*)&CurrentBuffer()->DataPtr()[GetOffset()];
	ItemCode()	=	Ptr[0];
	ItemSize()	=	Ptr[1];
	ItemData()	=	(uchar_t *)&Ptr[2];
	ItemOffset()	=	GetOffset()	+	sizeof(Ptr[0])*2;
	GetOffset()	=	ItemOffset()+Int32Align(Ptr[1]);
	return	true;
}

int32_t CMultiXMsg::GetItemCode()
{
	if(GetOffset()	==	0)	Next();
	return(ItemCode()	&	0xfffffff);

}

int32_t CMultiXMsg::GetItemSize()
{
	if(GetOffset()	==	0)	Next();
	return(ItemSize());
}
int32_t CMultiXMsg::GetItemOffset()
{
	if(GetOffset()	==	0)	Next();
	return(ItemOffset());
}

const	uchar_t * CMultiXMsg::GetItemData()
{
	if(GetOffset()	==	0)	Next();
	return	ItemData();
}

void CMultiXMsg::SetMsgOK(bool bValue)
{
	if(m_bMsgOK)
		if(bValue)
			return;
	if(bValue)
	{
		if(m_bMsgOK)
			return;
		PutOffset()	=	TotalSize();
		GetOffset()	=	0;
	}
	m_bMsgOK	=	bValue;
}

const	uchar_t * CMultiXMsg::Find(int32_t nItemCode,bool	bSave)
{
	int32_t	SaveGetOffset;
	int32_t	SaveItemCode;
	int32_t	SaveItemSize;
	uchar_t	*SaveItemData;
	const	uchar_t	*RetVal	=	NULL;

	if(bSave)
	{
		SaveGetOffset	=	GetOffset();
		SaveItemCode	=	ItemCode();
		SaveItemSize	=	ItemSize();
		SaveItemData	=	ItemData();
	}


	GetOffset()	=	0;
	while(Next())
	{
		if(GetItemCode()	==	nItemCode)
		{
			RetVal	=	GetItemData();
			break;
		}
	}
	if(bSave)
	{
		GetOffset()	=	SaveGetOffset;
		ItemCode()	=	SaveItemCode;
		ItemSize()	=	SaveItemSize;
		ItemData()	=	SaveItemData;
	}
	return	RetVal;
}

std::string & CMultiXMsg::Find(int32_t nItemCode, std::string &Data, bool bSave)
{
	char_t	*sData	=	(char_t	*)Find(nItemCode,bSave);
	if(sData)
		Data	=	(char	*)sData;
	return	Data;
}

void CMultiXMsg::RewindRead()
{
	GetOffset()	=	0;
}

bool CMultiXMsg::IsResponse()
{
	if(MsgResponseShift	==	0)
		return	(m_Hdr.Flags	&	MsgResponseBits)	!=	0;
	return	((m_Hdr.Flags	>>	MsgResponseShift)	&	MsgResponseBits)	!=	0;
}

void CMultiXMsg::SetAsResponse(bool Value)
{
	Hdr().Flags	=	(~(MsgResponseBits	<<	MsgResponseShift))	&	Hdr().Flags;
	if(Value)
		Hdr().Flags	|=	(MsgResponseBits	<<	MsgResponseShift);
}


int CMultiXMsg::Priority()
{
	if(MsgPriorityShift	==	0)
		return	(m_Hdr.Flags	&	MsgPriorityBits);
	return	((m_Hdr.Flags	>>	MsgPriorityShift)	&	MsgPriorityBits);
}

void CMultiXMsg::SetPriority(int Value)
{
	Hdr().Flags	=	(~(MsgPriorityBits	<<	MsgPriorityShift))	&	Hdr().Flags;
	if(Value)
		Hdr().Flags	|=	(MsgPriorityBits	<<	MsgPriorityShift);
}

const char_t * CMultiXMsg::OffsetToPtr(int32_t Offset)
{
	if(Offset	>=	TotalSize())
		return	NULL;
	CMultiXBuffer	*B;
	int	Idx	=	0;

	while(B	=	m_Bufs.GetBuffer(Idx))
	{
		if(B->Length()	>	Offset)
			return	B->DataPtr(Offset);
		else
			Offset	-=	B->Length();
		Idx++;
	}

	return	NULL;

/*
	if(Offset	>=	CurrentBuffer()->Length())
		return	NULL;
	if(Offset	<	0)
		return	NULL;
	return	CurrentBuffer()->DataPtr()	+	Offset;
*/
}

void CMultiXMsg::GetItemData(int32_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT32	:
		case	ItemDataTypeINT64	:
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}

void CMultiXMsg::GetItemData(uint32_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT32	:
			RetVal	=	*(uint32_t	*)GetItemData();
			break;
		case	ItemDataTypeINT64	:
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}


void CMultiXMsg::GetItemData(int16_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
		case	ItemDataTypeINT32	:
		case	ItemDataTypeUINT32	:
		case	ItemDataTypeINT64	:
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}

void CMultiXMsg::GetItemData(uint16_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
		case	ItemDataTypeUINT32	:
		case	ItemDataTypeINT64	:
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}



void CMultiXMsg::GetItemData(int64_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT32	:
			RetVal	=	*(uint32_t	*)GetItemData();
			break;
		case	ItemDataTypeINT64	:
			RetVal	=	*(int64_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}
#ifndef	NoUInt64
void CMultiXMsg::GetItemData(uint64_t &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT32	:
			RetVal	=	*(uint32_t	*)GetItemData();
			break;
		case	ItemDataTypeINT64	:
			RetVal	=	*(int64_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT64	:
			RetVal	=	*(uint64_t	*)GetItemData();
			break;
		default	:
			Throw();
	}
}
#endif


void CMultiXMsg::GetItemData(bool &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData()	?	true	:	false;
			break;
		case	ItemDataTypeINT16	:
		case	ItemDataTypeUINT16	:
		case	ItemDataTypeUINT32	:
		case	ItemDataTypeINT64	:
		case	ItemDataTypeUINT64	:
		default	:
			Throw();
	}
}




int CMultiXMsg::ItemDataType()
{
	if(GetOffset()	==	0)	Next();
	return(ItemCode()	>>	28);

}

void CMultiXMsg::GetItemData(std::string &RetVal)
{
	if(this->ItemDataType()	!=	ItemDataTypeSTR)
		Throw();
	RetVal	=	(char	*)GetItemData();
}
/*
void CMultiXMsg::GetItemData(int &RetVal)
{
	switch(this->ItemDataType())
	{
		case	ItemDataTypeINT16	:
			RetVal	=	*(int16_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT16	:
			RetVal	=	*(uint16_t	*)GetItemData();
			break;
		case	ItemDataTypeINT32	:
			RetVal	=	*(int32_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT32	:
			RetVal	=	*(uint32_t	*)GetItemData();
			break;
		case	ItemDataTypeINT64	:
			RetVal	=	*(int64_t	*)GetItemData();
			break;
		case	ItemDataTypeUINT64	:
			RetVal	=	*(uint64_t	*)GetItemData();
			break;
		default	:
			Throw();
	}

}
*/
