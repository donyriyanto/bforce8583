// MultiXUtil.cpp: implementation of the CMultiXBuffer class.
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

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXBuffer::CMultiXBuffer(const	char_t	*InitData,int	InitDataSize,CMultiXBufferPool	*Pool)	:
	m_pPool(Pool),
	m_Length(0),
	m_RefCount(1)
{

	m_Size	=	((InitDataSize >> 9)+1)<<9;	//	round it up to a multiply of 512
	m_pBuf	=	new	char_t[m_Size+1];
	m_pBuf[m_Size]	=	0;
	if(InitData	!=	NULL	&&	InitDataSize	>	0)
	{
		memcpy(m_pBuf,InitData,InitDataSize);
		m_Length	=	InitDataSize;
		m_pBuf[m_Length]	=	0;
	}
	m_Mutex.Initialize();

}

CMultiXBuffer::~CMultiXBuffer()
{
	CMultiXLocker			Locker(m_Mutex);
	if(m_RefCount)
		Throw();
	delete	m_pBuf;
}

//////////////////////////////////////////////////////////////////////
// CMultiXBufferPool Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXBufferPool::CMultiXBufferPool()
{
	m_Mutex.Initialize();

}

bool	CMultiXBuffer::operator	==	(CMultiXBuffer	&Other)
{
	byte_t	*B	=	(byte_t	*)LockData();
	byte_t	*BOther	=	(byte_t	*)Other.LockData();
	bool	RetVal	=	(this->Length()	==	Other.Length())	&&	(memcmp(B,BOther,this->Length())	==	0);
	Unlock();
	Other.Unlock();
	return	RetVal;
}

CMultiXBufferPool::~CMultiXBufferPool()
{
	EmptyPool();
}

CMultiXBuffer * CMultiXBufferPool::AllocateBuffer(const	char_t	*InitData,int	InitDataSize)
{
	CMultiXBuffer		*Buf;
	CMultiXLocker			Locker(m_Mutex);

	if(InitDataSize	<	0)
		InitDataSize	=	0;

	if(!m_Pool.empty())
	{
		Buf	=	m_Pool.front()->Clone();
		Buf->m_Length	=	0;
		Buf->Expand(InitDataSize,false,false);
		if(InitData	!=	NULL	&&	InitDataSize	>	0)
		{
			memcpy(Buf->m_pBuf,InitData,InitDataSize);
			Buf->m_Length	=	InitDataSize;
			Buf->m_pBuf[Buf->m_Length]	=	0;

		}
		m_Pool.pop_front();
	}	else
	{
		Buf	=	new	CMultiXBuffer(InitData,InitDataSize,this);
	}
	return	Buf;
}

void CMultiXBufferPool::ReturnToPool(CMultiXBuffer &Buf)
{
	CMultiXLocker			Locker(m_Mutex);
	m_Pool.push_back(&Buf);
}

void CMultiXBuffer::ReturnBuffer()
{
	CMultiXLocker	Locker(m_Mutex);
	m_RefCount--;
	if(m_RefCount	==	0)
		m_pPool->ReturnToPool(*this);
	else if(m_RefCount	<	0)
		Throw();
}

//////////////////////////////////////////////////////////////////////
// CMultiXEvent Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXEvent::CMultiXEvent(EventCodes	EventCode,CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	m_pOrigin(Origin),
	m_pTarget(Target),
	m_EventCode(EventCode)
{

}

CMultiXEvent::~CMultiXEvent()
{

}

//////////////////////////////////////////////////////////////////////
// CMultiXRequest Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXRequest::CMultiXRequest(RequestCodes	RequestCode,CMultiXLayer	*Origin,CMultiXLayer	*Target)	:
	CMultiXEvent(EventIsRequest,Origin,Target),
	m_RequestCode(RequestCode)
{

}

CMultiXRequest::~CMultiXRequest()
{

}

CMultiXMsgStack::CMultiXMsgStack()
{
}

CMultiXMsgStack::~CMultiXMsgStack()
{
	while(!this->Stack().empty())
	{
		this->Stack().front()->ReturnBuffer();
		this->Stack().pop_front();
	}
}


void CMultiXBuffer::Expand(int NewSize,bool	bUpdateLength,bool	bWithLock)
{
	if(bWithLock)
		m_Mutex.Lock();

	int	NewLength	=	NewSize;
	if(Size()	<	NewSize)
	{
		NewSize	=	((NewSize >> 9)+1)<<9;	//	round it up to a multiply of 512
		char_t	*NewBuf	=	new	char_t[NewSize+2];

		if(m_Length)
			memcpy(NewBuf,m_pBuf,m_Length);
		if(m_pBuf)
			delete m_pBuf;
		m_Size	=	NewSize;
		m_pBuf	=	NewBuf;
		NewBuf[NewSize]	=	0;
	}

	if(bUpdateLength)
	{
		if(NewLength	>	m_Length)
		{
			m_Length	=	NewLength;
			m_pBuf[m_Length]	=	0;
		}
	}
	if(bWithLock)
		m_Mutex.Unlock();
}
const	char_t	*CMultiXBuffer::AppendBuffer(CMultiXBuffer	&Buffer)
{
	char_t	*B	=	Buffer.LockData();
	const	char_t	*RetVal	=	AppendData(B,Buffer.Length());
	Buffer.Unlock();
	return	RetVal;
}

CMultiXBuffer	&CMultiXBuffer::operator <<=	(int ShiftCount)
{
	if(ShiftCount	<	0)
		Throw();
	char_t	*B	=	this->LockData();
	if(ShiftCount	>=	Length())
	{
		this->ReleaseData(0);
	}	else
	{
		memcpy(B,B+ShiftCount,Length()	-	ShiftCount);
		ReleaseData(Length()	-	ShiftCount);
		B[Length()]	=	0;
	}
	return	*this;
}
CMultiXBuffer	&CMultiXBuffer::operator >>=	(int ShiftCount)
{
	if(ShiftCount	<	0)
		Throw();
	if(ShiftCount	>	0)
	{
		if(Length()	>	0)
		{
			char_t	*B	=	this->LockData(Length()+ShiftCount);
			memmove(B+ShiftCount,B,Length());
			B[Length()	+	ShiftCount]	=	0;
			ReleaseData(Length()	+	ShiftCount);
		}	else
		{
			Expand(ShiftCount,true,true);
		}
	}
	return	*this;
}

const	char_t	*CMultiXBuffer::AppendData(const	char_t	*Data,int	DataSize)
{
	if(Data	!=	NULL	||	DataSize	>	0)
	{
		char_t	*B	=	LockData(Length()+DataSize+1);
		memcpy(B+Length(),Data,DataSize);
		B[Length()+DataSize]	=	0;
		ReleaseData(Length()+DataSize);
	}
	return	DataPtr();
}
const	char_t	*CMultiXBuffer::AppendString(const	char_t	*Data,bool	bAddNull)
{
	if(Data	!=	NULL)
	{
		int	Len	=	(int)strlen(Data);
		if(bAddNull)
			Len++;
		if(Len	>	0)
			return	AppendData(Data,Len);
	}
	return	DataPtr();
}

const	char_t	*CMultiXBuffer::AppendString(const	std::string	&Data,bool	bAddNull)
{
	return	AppendString(Data.c_str(),bAddNull);
}

char_t * CMultiXBuffer::LockData(int MinSize)
{
	m_Mutex.Lock();
	Expand(MinSize,false,true);
	return	m_pBuf;
}

void	CMultiXBuffer::Empty()
{
	CMultiXLocker	Locker(m_Mutex);
	m_Length	=	0;
	m_pBuf[m_Length]	=	0;
}

void CMultiXBuffer::ReleaseData(int NewLength)
{
	CMultiXLocker	Locker(m_Mutex);
	if(NewLength	<	0)
		Throw();
	if(NewLength	>	Size())
		Throw();
	m_Length	=	NewLength;
	m_pBuf[m_Length]	=	0;
	m_Mutex.Unlock();
}

CMultiXBuffer * CMultiXBuffer::CreateNew(bool	bDuplicate)
{
	if(bDuplicate)
		return	m_pPool->AllocateBuffer(DataPtr(),Length());
	else
		return	m_pPool->AllocateBuffer(NULL,0);
}

CMultiXBuffer * CMultiXBuffer::Mid(int	StartPos,int	Len)
{
	if(StartPos	>=	Length())
		return	m_pPool->AllocateBuffer(NULL,0);
	if(Len	<	0)
		Len	=	0;
	if(StartPos	<	0)
		StartPos	=	0;
	if(Len	==	0	||	Len	>	(Length() -	StartPos))
		Len	=	(Length() -	StartPos);
	return	m_pPool->AllocateBuffer(DataPtr()+StartPos,Len);
}

int	CMultiXBuffer::Find(const	char_t	*ToFind,int Len)
{
	return	Find(0,ToFind,Len);
}

int	CMultiXBuffer::Find(int	StartPos,const	char_t	*ToFind,int Len)
{
	int	CurrentPos	=	StartPos;
	if(ToFind	==	NULL)
		return	-1;
	if(Len	<=	0)
		Len	=	(int)strlen((const	char	*)ToFind);
	while(CurrentPos	+	Len	<=	Length())
	{
		if(*DataPtr(CurrentPos)	==	*ToFind)
		{
			if(memcmp(ToFind,DataPtr(CurrentPos),Len)	==	0)
				return	CurrentPos;
		}
		CurrentPos++;
	}
	return	-1;
}

int32_t CMultiXMsgStack::TotalBufferSize()
{
	if(this->Stack().empty())
		return	0;
	int32_t	RetVal	=	0;
	for(iterator	I=Stack().begin();I!=Stack().end();I++)
		RetVal	+=	(*I)->Length();

	return	RetVal;

}

void CMultiXBuffer::Unlock()
{
	m_Mutex.Unlock();
}

CMultiXBuffer * CMultiXBuffer::Clone()
{
	CMultiXLocker	Locker(m_Mutex);
	m_RefCount++;
	return	this;
}

//////////////////////////////////////////////////////////////////////
// CMultiXBufferArray Class
//////////////////////////////////////////////////////////////////////

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMultiXBufferArray::CMultiXBufferArray()
{

}

CMultiXBufferArray::~CMultiXBufferArray()
{
	for(size_t	I=0;I<m_Buffers.size();I++)
		m_Buffers[I]->ReturnBuffer();
}

void CMultiXBufferArray::Add(CMultiXBuffer &Buf)
{
	m_Buffers.push_back(Buf.Clone());
}

CMultiXBuffer * CMultiXBufferArray::GetBuffer(int Index) const
{
	if(Index	>=	Count())
		Throw();
	return	m_Buffers[Index];
}

int32_t CMultiXBufferArray::DataSize()	const
{
	int32_t	Size	=	0;
	for(int	I=0;I<Count();I++)
		Size	+=	m_Buffers[I]->Length();
	return	Size;
}

char_t * CMultiXBuffer::GetDataNoLock()
{
	return	m_pBuf;	
}

void CMultiXBuffer::UpdateLengthNoLock(int32_t NewLength)
{
	if(NewLength	<	0)
		Throw();
	if(NewLength	>	Size())
		Throw();
	m_Length	=	NewLength;
	m_pBuf[m_Length]	=	0;
}

void CMultiXBufferPool::EmptyPool()
{
	CMultiXLocker			Locker(m_Mutex);
	while(!m_Pool.empty())
	{
		delete	m_Pool.front();
		m_Pool.pop_front();
	}

}

template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>::CMultiXVector(ObjectOwner	*pOwner,bool	bSync)
{
	m_pOwner	=	pOwner;
	m_bSync=bSync;
	m_Mutex.Initialize();
}
template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>::~CMultiXVector()
{
}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Next()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->Next(*this);
			}	catch(...)
			{
				return	NULL;
			}
		}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Prev()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->Prev(*this);
			}	catch(...)
			{
				return	NULL;
			}
		}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::CObjectID::GetObject()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->GetObject(*this);
			}	catch(...)
			{
				return	NULL;
			}
		}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::CObjectID::RemoveObject()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->RemoveObject(*this);
			}	catch(...)
			{
				return	NULL;
			}

		}

template	<class	ObjectType,class	ObjectOwner>
bool CMultiXVector<ObjectType,ObjectOwner>::CObjectID::IsValid()
		{
			try
			{
				if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
					return	false;
				if(Container()	==	NULL)
					return	false;
				if(Index()	<	0)
					return	false;
				try
				{
					return	Container()->IsValid(*this);
				}
				catch	(...)
				{
					return	false;
				}
			}
			catch	(...)
			{
				return	false;
			}
		}

template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>::CObjectID::CObjectID()	
		{
			if(DataSize()	!=	sizeof(TMultiXGenericID))
				Throw();

			memset(&m_Data,0,sizeof(m_Data));
			Init();
		}
template	<class	ObjectType,class	ObjectOwner>
void CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Init()
		{
			Container()	=	NULL;
			Index()	=	-1;
			IDIndicator()	=	__MULTIX_OBJECT_ID_INDICATOR;			
		}
template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>::CObjectID::~CObjectID()
		{
			if(IDIndicator()	==	__MULTIX_OBJECT_ID_INDICATOR)
				IDIndicator()	=	0;
		}
template	<class	ObjectType,class	ObjectOwner>
ObjectOwner	*&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Owner()
{
	return	m_Data.S.m_pOwner;
}
template	<class	ObjectType,class	ObjectOwner>
int32_t	&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Index()
{
	return	m_Data.S.m_Index;
}
template	<class	ObjectType,class	ObjectOwner>
int32_t	&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Signature()
{
	return	m_Data.S.m_Signature;
}
template	<class	ObjectType,class	ObjectOwner>
int16_t	&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::IDIndicator()
{
	return	m_Data.S.m_IDIndicator;
}
template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>	*&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::Container()
{
	return	m_Data.S.m_pContainer;
}
template	<class	ObjectType,class	ObjectOwner>
TMultiXGenericID	&CMultiXVector<ObjectType,ObjectOwner>::CObjectID::GenericID()
{
	return	m_Data.I;
}
template	<class	ObjectType,class	ObjectOwner>
int	CMultiXVector<ObjectType,ObjectOwner>::CObjectID::SignificantDataSize()
{
	return	sizeof(((TIDData *)0)->S);
}
template	<class	ObjectType,class	ObjectOwner>
const	byte_t	*CMultiXVector<ObjectType,ObjectOwner>::CObjectID::SignificantData()
{
	return	(byte_t	*)&m_Data.S;
}

template	<class	ObjectType,class	ObjectOwner>
void	CMultiXVector<ObjectType,ObjectOwner>::CObjectID::operator=(const	uchar_t *Ptr)
{
	m_Data	=	*(TIDData	*)Ptr;
}
template	<class	ObjectType,class	ObjectOwner>
bool	CMultiXVector<ObjectType,ObjectOwner>::CObjectID::operator	==	(CObjectID	&Second)
{
	return	memcmp(&m_Data,&Second.m_Data,sizeof(m_Data)) == 0;
}
template	<class	ObjectType,class	ObjectOwner>
CMultiXVector<ObjectType,ObjectOwner>::CObjectID::operator	uchar_t	*()
{
	return	(uchar_t	*)&m_Data;
}
template	<class	ObjectType,class	ObjectOwner>
size_t	CMultiXVector<ObjectType,ObjectOwner>::CObjectID::DataSize()
{
	return	sizeof(m_Data);
}


template	<class	ObjectType,class	ObjectOwner>
bool	CMultiXVector<ObjectType,ObjectOwner>::IsValid(CObjectID &ObjectID)
	{
		try
		{
			if(ObjectID.IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	false;
			if(ObjectID.Container()	!=	this)
				return	false;
			if(ObjectID.Index()	<	0)
				return	false;
			if(ObjectID.Index()	>=	(int)m_Objects.size())
				return	false;
			if(m_Objects[ObjectID.Index()]	==	NULL)
				return	false;
			if(m_Objects[ObjectID.Index()]->ID().Index()	!=	ObjectID.Index())
				return	false;
			if(m_Objects[ObjectID.Index()]->ID().Signature()	!=	ObjectID.Signature())
				return	false;
		}	catch(...)
		{
			return	false;
		}
		return	true;
	}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::GetObject(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		return	m_Objects[ObjectID.Index()];
	}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::First()
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		for(int	I	=	0;I	<	(int)m_Objects.size();I++)
		{
			if(m_Objects[I]	!=	NULL)
			{
				try
				{
					if(IsValid(m_Objects[I]->ID()))
						return	m_Objects[I];
				}	catch	(...)
				{
				}
			}
		}
		return	NULL;
	}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::Next(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		for(int	I	=	ObjectID.Index()+1;I	<	(int)m_Objects.size();I++)
		{
			if(m_Objects[I]	!=	NULL)
			{
				try
				{
					if(IsValid(m_Objects[I]->ID()))
						return	m_Objects[I];
				}	catch	(...)
				{
				}
			}
		}
		return	NULL;
	}

template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::Prev(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		for(int	I	=	ObjectID.Index()-1;I	>=	0;I--)
		{
			if(m_Objects[I]	!=	NULL)
			{
				try
				{
					if(IsValid(m_Objects[I]->ID()))
						return	m_Objects[I];
				}	catch	(...)
				{
				}
			}
		}
		return	NULL;
	}
template	<class	ObjectType,class	ObjectOwner>
ObjectType * CMultiXVector<ObjectType,ObjectOwner>::RemoveObject(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		ObjectType	*Object;
		int32_t	Index	=	ObjectID.Index();
		if(!IsValid(ObjectID))
			return	NULL;
		Object	=	m_Objects[Index];
		Object->ID().Container()	=	NULL;
		Object->ID().Index()	=	-1;
		m_Objects[Index]	=	NULL;
		return	Object;
	}
template	<class	ObjectType,class	ObjectOwner>
typename	CMultiXVector<ObjectType,ObjectOwner>::CObjectID CMultiXVector<ObjectType,ObjectOwner>::InsertObject(ObjectType	*Object)
	{
		size_t	I;
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(IsValid(Object->ID()))
			Throw();
		for(I=0;I<m_Objects.size();I++)
		{
			if(m_Objects[I]	==	NULL)
			{
				break;
			}
		}
		Object->ID().Owner()	=	m_pOwner;
		Object->ID().Index()	=	(int32_t)I;
		Object->ID().Signature()	=	m_ObjectSignatures++;
		Object->ID().Container()	=	this;
		if(I	==	m_Objects.size())
			m_Objects.push_back(Object);
		else
			m_Objects[I]	=	Object;
		return	Object->ID();
	}
template	<class	ObjectType,class	ObjectOwner>
CMultiXSyncObject	&CMultiXVector<ObjectType,ObjectOwner>::GetSyncObj()
{
	return	m_Mutex;
}
template	<class	ObjectType,class	ObjectOwner>
bool	CMultiXVector<ObjectType,ObjectOwner>::IsEmpty()
{
	return	m_Objects.empty();
}
template	<class	ObjectType,class	ObjectOwner>
std::vector<ObjectType	*>	&CMultiXVector<ObjectType,ObjectOwner>::Objects()
{
	return	m_Objects;
}
template	<class	ObjectType,class	ObjectOwner>
size_t	CMultiXVector<ObjectType,ObjectOwner>::Size()
{
	return	m_Objects.size();
}
template	<class	ObjectType,class	ObjectOwner>
void	CMultiXVector<ObjectType,ObjectOwner>::Lock()
	{
		m_Mutex.Lock();
	}
template	<class	ObjectType,class	ObjectOwner>
void	CMultiXVector<ObjectType,ObjectOwner>::Unlock()
	{
		m_Mutex.Unlock();
	}
template	<class	ObjectType,class	ObjectOwner>
void	CMultiXVector<ObjectType,ObjectOwner>::DeleteAll()
	{
		CMultiXLocker	Locker(m_Mutex);
		while(!m_Objects.empty())
		{
			ObjectType	*O	=	*m_Objects.begin();
			if(O)
			{
				O->ID().Init();
				delete	O;
			}
			m_Objects.erase(m_Objects.begin());
		}
	}





template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>::CMultiXMap(ObjectOwner	*pOwner,bool	bSync)
{
	m_pOwner	=	pOwner;
	m_bSync=bSync;
	m_Mutex.Initialize();
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>::~CMultiXMap()
{
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::GetObject()
{
	if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
		return	NULL;
	if(Container()	==	NULL)
		return	NULL;
	try
	{
		return	Container()->GetObject(*this);
	}	catch(...)
	{
		return	NULL;
	}
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Next()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->Next(*this);
			}	catch(...)
			{
				return	NULL;
			}
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Prev()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->Prev(*this);
			}	catch(...)
			{
				return	NULL;
			}
		}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::RemoveObject()
		{
			if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
				return	NULL;
			if(Container()	==	NULL)
				return	NULL;
			try
			{
				return	Container()->RemoveObject(*this);
			}	catch(...)
			{
				return	NULL;
			}

		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
bool	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::IsValid()
		{
			try
			{
				if(IDIndicator()	!=	__MULTIX_OBJECT_ID_INDICATOR)
					return	false;
				if(Container()	==	NULL)
					return	false;
				try
				{
					return	(Container()->GetObject(*this)	!=	NULL);
				}
				catch	(...)
				{
					return	false;
				}
			}	catch	(...)
			{
				return	false;
			}
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::CObjectID()	
		{
			if(DataSize()	!=	sizeof(TMultiXGenericID))
				Throw();
			memset(&m_Data,0,sizeof(m_Data));
			Init();
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
void	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Init()
		{
			Container()	=	NULL;
			IDIndicator()	=	__MULTIX_OBJECT_ID_INDICATOR;
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::~CObjectID()
		{
			if(IDIndicator()	==	__MULTIX_OBJECT_ID_INDICATOR)
				IDIndicator()	=	0;
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectOwner	*&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Owner()
{
	return	m_Data.S.m_pOwner;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
KeyType	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Key()
{
	return	m_Data.S.m_Key;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
int32_t	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Signature()
{
	return	m_Data.S.m_Signature;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
int16_t	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::IDIndicator()
{
	return	m_Data.S.m_IDIndicator;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>	*&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::Container()
{
	return	m_Data.S.m_pContainer;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
TMultiXGenericID	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::GenericID()
{
	return	m_Data.I;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
int	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::SignificantDataSize()
{
	return	sizeof(((TIDData *)0)->S);
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
const	byte_t	*CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::SignificantData()
{
	return	(byte_t	*)&m_Data.S;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
bool	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::operator	==	(CObjectID	&Second)
		{
			return	memcmp(&m_Data,&Second.m_Data,sizeof(m_Data)) == 0;
		}
		template	<class	KeyType,class	ObjectType,class	ObjectOwner>
void	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::operator=(const	uchar_t *Ptr)
		{
			m_Data	=	*(TIDData	*)Ptr;
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::operator	uchar_t	*()
		{
			return	(uchar_t	*)&m_Data;
		}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
size_t	CMultiXMap<KeyType,ObjectType,ObjectOwner>::CObjectID::DataSize()
		{
			return	sizeof(m_Data);
		}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::GetFirst()
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!m_Objects.empty())
			return	m_Objects.begin()->second;
		return	NULL;
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::GetObject(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		return	m_Objects[ObjectID.Key()];
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::Next(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		
		TIterator	I	=	m_Objects.find(ObjectID.Key());
		if(I	==	m_Objects.end())
			return	NULL;
		I++;
		if(I	==	m_Objects.end())
			return	NULL;
		return	I->second;
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::Prev(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(!IsValid(ObjectID))
			return	NULL;
		TIterator	I	=	m_Objects.find(ObjectID.Key());
		if(I	==	m_Objects.end())
			return	NULL;
		I--;
		if(I	==	m_Objects.begin())
			return	NULL;
		return	I->second;
	}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::Find(KeyType &Key)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		TIterator	I	=	m_Objects.find(Key);
		if(I	==	m_Objects.end())
			return	NULL;
		return	I->second;
	}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::RemoveObject(KeyType	Key)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		ObjectType	*O	=	Find(Key);
		if(O)
			RemoveObject(O->ID());
		return	O;
	}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
ObjectType *CMultiXMap<KeyType,ObjectType,ObjectOwner>::RemoveObject(CObjectID &ObjectID)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		ObjectType	*Object;
		KeyType	Key	=	ObjectID.Key();

		if(!IsValid(ObjectID))
			return	NULL;
		Object	=	m_Objects[Key];
		Object->ID().Container()	=	NULL;
		m_Objects.erase(Key);
		return	Object;
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
bool CMultiXMap<KeyType,ObjectType,ObjectOwner>::InsertObject(ObjectType	*Object,KeyType	Key)
	{
		CMultiXLocker	Locker(m_Mutex,m_bSync);
		if(m_Objects[Key]	!=	NULL)
			return	false;
		m_Objects[Key]	=	Object;
		Object->ID().Owner()	=	m_pOwner;
		Object->ID().Key()	=	Key;
		Object->ID().Signature()	=	m_ObjectSignatures++;
		Object->ID().Container()	=	this;

		return	true;
	}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
void	CMultiXMap<KeyType,ObjectType,ObjectOwner>::DeleteAll()
	{
		CMultiXLocker	Locker(m_Mutex);
		while(!m_Objects.empty())
		{
			ObjectType	*O	=	m_Objects.begin()->second;
			O->ID().Init();
			delete	O;
			m_Objects.erase(m_Objects.begin());
		}
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
CMultiXSyncObject	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::GetSyncObj()
{
	return	m_Mutex;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
EXPORTABLE_STL::map<KeyType,ObjectType *>	&CMultiXMap<KeyType,ObjectType,ObjectOwner>::Objects()
{
	return	m_Objects;
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
bool	CMultiXMap<KeyType,ObjectType,ObjectOwner>::IsEmpty()
{
	return	m_Objects.empty();
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
size_t	CMultiXMap<KeyType,ObjectType,ObjectOwner>::Size()
{
	return	m_Objects.size();
}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
void	CMultiXMap<KeyType,ObjectType,ObjectOwner>::Lock()
	{
		m_Mutex.Lock();
	}
template	<class	KeyType,class	ObjectType,class	ObjectOwner>
void	CMultiXMap<KeyType,ObjectType,ObjectOwner>::Unlock()
	{
		m_Mutex.Unlock();
	}

template	<class	KeyType,class	ObjectType,class	ObjectOwner>
bool CMultiXMap<KeyType,ObjectType,ObjectOwner>::IsValid(CObjectID &ObjectID)
	{
		try
		{
			if(ObjectID.Container()	!=	this)
				return	false;
			TIterator	I	=	m_Objects.find(ObjectID.Key());
			if(I	==	m_Objects.end())
				return	false;
			if(I->second->ID().Signature()	!=	ObjectID.Signature())
				return	false;
		}	catch(...)
		{
			return	false;
		}
		return	true;
	}



template	<class	ObjectType,class	ObjectOwner>
CMultiXInt32Map<ObjectType,ObjectOwner>::CMultiXInt32Map(ObjectOwner	*pOwner,bool	bSync)	:
CMultiXMap<int32_t,ObjectType,ObjectOwner>(pOwner,bSync)
{
	m_NextKey	=	1;
}
template	<class	ObjectType,class	ObjectOwner>
CMultiXInt32Map<ObjectType,ObjectOwner>::~CMultiXInt32Map()
{
}
template	<class	ObjectType,class	ObjectOwner>
bool CMultiXInt32Map<ObjectType,ObjectOwner>::InsertObject(ObjectType	*Object)
{
#ifdef	TandemOs
	CMultiXLocker	Locker(CMultiXMap<int32_t,ObjectType,ObjectOwner>::m_Mutex,true);
#else
	CMultiXLocker	Locker(CMultiXMap<int32_t,ObjectType,ObjectOwner>::m_Mutex,true);
//	CMultiXLocker	Locker(m_Mutex,true);
#endif
	m_NextKey++;
	return	CMultiXMap<int32_t,ObjectType,ObjectOwner>::InsertObject(Object,m_NextKey);
}

template	class CMultiXVector<CMultiXLink,CMultiXApp>;
template	class CMultiXVector<CMultiXL3Link,CMultiXL3>;
template	class CMultiXVector<CMultiXTransportDevice,CMultiXTransportInterface>;
template	class CMultiXVector<class CMultiXL2Link,class CMultiXL2>;
template	class CMultiXVector<class CMultiXAlertableObject,int>;
template	class CMultiXMap<TMultiXProcID,class CMultiXProcess,class CMultiXApp>;
template	class CMultiXMap<int32_t,class CMultiXAppMsg,class CMultiXProcess>;
template	class CMultiXMap<TMultiXProcID,class CMultiXL3Process,class CMultiXL3>;
template	class CMultiXInt32Map<class CMultiXAppMsg,class CMultiXProcess>;

template<class	_T>	MULTIXWIN32DLL_API
void	CMultiXUtil::SwapInt(_T	&I)
{
	_T	O;
	char	*First		=	(char *)&I;
	char	*Second		=	(char *)&O;
	if (sizeof(_T)==2)
	{
		Second[0]	=	First[1];
		Second[1]	=	First[0];
	}	else
	if (sizeof(_T)==4)
	{
		Second[0]	=	First[3];
		Second[1]	=	First[2];
		Second[2]	=	First[1];
		Second[3]	=	First[0];
	}	else
	if	(sizeof(_T)==8)
	{
		Second[0]	=	First[7];
		Second[1]	=	First[6];
		Second[2]	=	First[5];
		Second[3]	=	First[4];
		Second[4]	=	First[3];
		Second[5]	=	First[2];
		Second[6]	=	First[1];
		Second[7]	=	First[0];
	}	else
		return;
	I	=	O;
}
template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(int16_t	&);
template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(int32_t	&);
template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(int64_t	&);

template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(uint16_t	&);
template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(uint32_t	&);
template	MULTIXWIN32DLL_API	void	CMultiXUtil::SwapInt(uint64_t	&);
