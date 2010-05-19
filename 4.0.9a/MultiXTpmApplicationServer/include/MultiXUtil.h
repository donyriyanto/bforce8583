/*!	\file	*/
// MultiXUtil.h: interface for the CMultiXBuffer class.
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


#if !defined(AFX_MULTIXUTIL_H__61BEF7CB_7367_4F5F_A40A_ECA19E45DD05__INCLUDED_)
#define AFX_MULTIXUTIL_H__61BEF7CB_7367_4F5F_A40A_ECA19E45DD05__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include	<string.h>
#include	"MultiXThreads.h"

class	CMultiXLayer;
class	CMultiXBufferPool;


/*!
	This class is used when the application calls the Throw() macro. When Throw() is called,
	it creates an instance of this class and calls the C++ throw statement with this class
	as a parameter.
*/
class	MULTIXWIN32DLL_API	CMultiXException
{
public:
	std::string	m_File;	//!<	File name where the exception occured.
	int			m_Line;	//!<	Line number where the exception occured.
	std::string	m_Text;	//!<	Error text used 
	CMultiXException(const	char	*FileName,int	Line,std::string	ErrorText="")	:
		m_File(FileName),
		m_Line(Line),
		m_Text(ErrorText)
	{
	}
	~CMultiXException(){}
};


typedef	struct{byte_t	C[32];}	TMultiXGenericID;


/*!
This class provides the means for managing buffer space and buffer pool. It encapsulate
the logic for allocating memory, accessing the underlying memory space and adding and removing
from a pool. ::CMultiXBuffer objects are allocated one using CMultiXApp::AllocateBuffer() and
never freed until the process terminates. When the application does not need the buffer anymore,
it returns it to a pool by calling ReturnBuffer(), the application has no access to the destractor of
the class. A buffer that was returned to the pool, will be used on the next allocate request. ::CMultiXBuffer
is thread safe object, this means that an application can access the same object from multiple threads.
If the application needs to keep multiple pointers to the same object, the application must Clone()
the object and when it not needed any more it must be "returned".

*/
class MULTIXWIN32DLL_API	CMultiXBuffer  
{
	friend	class	CMultiXBufferPool;
public:
	/*!
	\brief	Updates the length of the actual data in the buffer.

	This function will update the length of the data, but without locking the object. This means
	that application must assume that no other thread is using the same buffer at the same time.
	This call used to enhance performance.

	\param	Value	The new length of the buffer, it must not exceede the value used in CMultiXApp::AllocateBuffer()
	or the Expand().

	*/
	void UpdateLengthNoLock(int32_t Value);
	char_t * GetDataNoLock();//!<	\return A pointer to the underline buffer, used for modification of the data, not thread safe.
	CMultiXBuffer * Clone();	//!<	\return	apointer to the same object and incrementing the reference count.
	void Unlock();	//!<	Unlocks the object that was locked with LockData(), frees other threads to manipulate the object.
	/*!
	\brief	Create a new CMultiXBuffer object.

	This function creates a new object and if specified, copies the data from the current object. After the
	duplicate, both objects are not related anymore.
	\param bDuplicate	if true, copies the content of the current object to the newly created object
	\return	A pointer to the newly created object.
	*/
	CMultiXBuffer * CreateNew(bool	bDuplicate=false);
	/*!
	\brief	Create a new CMultiXBuffer object and copies the specified range of data to the new object.

	This function creates a new object and if specified, copies the data from the current object. After the
	duplicate, both objects are not related anymore.
	\param StartPos	the index of the first byte to be copied to the new object. if this value is greater the the length
	of the current buffer, an empty object will be returned (not NULL).
	\param	Len	the number of bytes to copy to the new object. if there are not enough bytes to copy,
	a smaller amount will be copied.
	\return	A pointer to the newly created object.
	*/

	CMultiXBuffer * Mid(int	StartPos,int	Len=0);

	int	Find(const	char_t	*ToFind,int Len=0);
	int	Find(int	StartPos,const	char_t	*ToFind,int Len=0);

	/*!
	\brief	Unlock the object an updates its length to NewLength

	This function can be called only if it was lock by LockData(), otherwise
	an exception is raised.
	\param	NewLength	The new length of the actual data.
	*/
	void ReleaseData(int NewLength);
	/*!
	\brief	Lock the underlying buffer for exclusive update.

	When there is a need to update the underlying buffer, this function is called. If there
	is a need to ensure the minimum size of the underlying buffer, MinSize is provider. Once 
	the pointer is returned, no other thread can access the object or its clones until Releasedata()
	or Unlock() is called.
	\param	MinSize	the minimum size required for the underlying buffer.
	\return	a pointer to the underlying buffer.
	*/
	char_t * LockData(int	MinSize=0);
	/*!
	\brief	expand the size of the underlying buffer.

	Use this function to expand the size of the underlying buffer when there is a need to add data
	to the underlying buffer. Besides reallocating the underlying buffer, by specifying bUpdateLength,
	the application tells MultiX to set the length of the data to the size of the buffer, usually, the size
	of the buffer is larger than the actual data that it stores. If there is a need to ensure
	that no other thread accesses the object at the time of the expand , set bWithLock to true.
	\param	NewSize	Reallocate the buffer to the new size.
	\param	bUpdateLength	Set the length of data to the buffer size.
	\param	bWithLock	Lock the object to ensure exclusive access while updating the object size.
	*/
	void Expand(int NewSize,bool	bUpdateLength,bool	bWithLock);
	/*!
	\brief	Return the buffer to the pool.

	Once returned, the application should not use the pointer and it should treat it as a deleted object.
	*/
	void ReturnBuffer();

	inline	int	Size(){return	m_Size;}	//!<	\return	The allocated size of the underlying buffer.
	inline	int	Length()	const	{return	m_Length;}	//!<	\return	The length of data stored in the underlying buffer.
	/*!
	\brief	Get read access to the data in the underlying buffer.
	\param	index	The offset into the underlying buffer.
	\return	a read only pointer to the underlying buffer.
	*/
	inline	const	char_t	*DataPtr(int	index=0)	const	{return	m_pBuf+index;}

	/*!
	\brief	Append the provided data to the underlying buffer.

	Use this function to append data to the underlying buffer. Before adding the data, MultiX expands the buffer and then
	it copies the data to the end of the buffer. During the entire process, the buffer is locked, and
	only the calling thread has write access to the buffer.
	\param	Data a pointer to the data to be added to the buffer.
	\param	Size the size of the data to be added.
	\return	a read only pointer to the new underlying buffer.
	*/
	const	char_t	*AppendData(const	char_t	*Data,int	Size);

	/*!
	\brief	Append the provided string to the underlying buffer.

	Use this function to append string to the underlying buffer. Before adding the data, MultiX expands the buffer and then
	it copies the data to the end of the buffer. During the entire process, the buffer is locked, and
	only the calling thread has write access to the buffer.
	
	\param	StringData a pointer to the null terminated C string. If it is empty, a null byte is appended (if bAddNull is true). If it is NULL, no data is appended.
	\param	bAddNull indicates whether to add the null byte or not
	\return	a read only pointer to the new underlying buffer.
	*/
	const	char_t	*AppendString(const	char_t	*StringData,bool	bAddNull);
	const	char_t	*AppendString(const	std::string	&StringData,bool	bAddNull);

/*!
	\brief	Append the provided CMultiXBuffer to the underlying buffer.

	Use this function to append a CMultiXBuffer Data to the underlying buffer. Before adding the data, MultiX expands the buffer and then
	it copies the data to the end of the buffer. During the entire process, the buffer is locked, and
	only the calling thread has write access to the buffer.
	\param	Buffer a reference to an existing CMultiXBuffer object.
	\return	a read only pointer to the new underlying buffer.
	*/
	const	char_t	*AppendBuffer(CMultiXBuffer	&Buffer);

/*!
	\brief	Shift the content of the underlying buffer to the left.

	Use this function to shift the underlying buffer towards the begining of the buffer.
	the length of the buffer is adjusted accordingly. If ShiftCount is greater then the total length of the underlying buffer,
	the buffer becomes empty. During the entire process, the buffer is locked, and
	only the calling thread has write access to the buffer.
	\param	ShiftCount The count of bytes to discard from the begining. The total length of the buffer is subtructed by this number.
	\return	a read only pointer to the new underlying buffer.
	*/

	CMultiXBuffer	&	operator	<<=	(int	ShiftCount);

/*!
	\brief	Shift the content of the underlying buffer to the right.

	Use this function to shift the underlying buffer towards the end of the buffer.
	the length of the buffer is adjusted accordingly. If ShiftCount is greater then the total length of the underlying buffer,
	the buffer becomes empty. During the entire process, the buffer is locked, and
	only the calling thread has write access to the buffer.
	\param	ShiftCount The count of bytes to add to the buffer. The total length of the buffer is incremented by this number.
	\return	a read only pointer to the new underlying buffer.
	*/

	CMultiXBuffer	&	operator	>>=	(int	ShiftCount);
	/*!
	\brief	Clears the content of the buffer and sets the length to zero.

	*/
	void	Empty();

	/*!
	\brief	Compare the content of 2 buffers

	\return	 true if the content and length of the of the data is the same
	*/
	bool	operator	==	(CMultiXBuffer	&Other);

private:
	CMultiXBuffer(const	char_t	*InitData,int	InitDataSize,CMultiXBufferPool	*Pool);
	virtual ~CMultiXBuffer();
private:
	int		m_Size;
	char_t	*m_pBuf;
	CMultiXBufferPool	*m_pPool;
	int		m_Length;
	CMultiXMutex	m_Mutex;
	int		m_RefCount;

};


class MULTIXWIN32DLL_API	CMultiXBufferPool  
{
	friend	class	CMultiX;
	friend	class	CMultiXBuffer;
public:
	void EmptyPool();
	CMultiXBuffer * AllocateBuffer(const	char_t	*InitData,int	InitDataSize);
	virtual ~CMultiXBufferPool();
private:
	typedef	EXPORTABLE_STL::deque<CMultiXBuffer	*>	TIoPool;
	TIoPool	m_Pool;
	CMultiXMutex	m_Mutex;
	void ReturnToPool(CMultiXBuffer &Buf);
protected:
	CMultiXBufferPool();
};


class	MULTIXWIN32DLL_API	CMultiXMsgStack
{
public:
	typedef	EXPORTABLE_STL::deque<CMultiXBuffer	*>::iterator	iterator;

	int32_t TotalBufferSize();
	CMultiXMsgStack();
	virtual	~CMultiXMsgStack();
	void	*&Owner(){return	m_pOwner;}
	inline	EXPORTABLE_STL::deque<CMultiXBuffer	*>	&Stack(){return	m_Queue;}


private:
	EXPORTABLE_STL::deque<CMultiXBuffer	*>	m_Queue;
	void	*m_pOwner;

};

/*!
::CMultiXEvent is the base class for all events in MultiX. If the application wishes
to generate events, it should derive a class from CMultiXAppEvent which is derived from this class.
*/
class MULTIXWIN32DLL_API	CMultiXEvent  
{
public:
enum	EventCodes
{
	EventIsRequest		=	-1000,
	EventIsTimer,
	L1ConnectCompleted	=	-900,
	L1AcceptCompleted,
	L1ReadCompleted,
	L1WriteCompleted,
	L1CloseCompleted,

	L2ConnectCompleted,
	L2AcceptCompleted,
	L2SendCompleted,
	L2NewBlockReceived,
	L2CloseCompleted,


	L3OpenFailed,
	L3OpenCompleted,
	L3ConnectFailed,
	L3ListenFailed,
	L3LinkConnected,
	L3LinkAccepted,
	L3LinkDisconnected,
	L3SendDataFailed,
	L3SendDataCompleted,
	L3DataReceived,
	L3LinkClosed,
	
	
	L3NewMsg,
	L3ProcessConnected,
	L3ProcessDisconnected,
//	L3SendMsgFailed,
//	L3SendMsgCompleted,


	MultiXAppStarted,
	MultiXAppShutdownReq,
	EventCodeLast
}	m_EventCode;

	friend	class	CMultiXAppEvent;
	friend	class	CMultiXL1Event;
	friend	class	CMultiXL2Event;
	friend	class	CMultiXL3Event;
	friend	class	CMultiXTransportEvent;
	friend	class	CMultiXRequest;
	friend	class	CMultiXTimerEvent;
public:
	virtual ~CMultiXEvent();
	/*!
	\brief	The code used by the application for the event.
	\return	The event code.
	*/
	inline	EventCodes	EventCode(){return	m_EventCode;}
	inline	CMultiXLayer	*&Target(){return	m_pTarget;}
	inline	CMultiXLayer	*&Origin(){return	m_pOrigin;}
private:
	CMultiXEvent(EventCodes	EventCode,CMultiXLayer	*Origin,CMultiXLayer	*Target);
	CMultiXLayer	*m_pOrigin;
	CMultiXLayer	*m_pTarget;



};

class	MULTIXWIN32DLL_API	CMultiXRequest	:	public	CMultiXEvent
{
	friend	class	CMultiXL1OpenReq;
	friend	class	CMultiXL1ConnectReq;
	friend	class	CMultiXL1ListenReq;
	friend	class	CMultiXL1ReadReq;
	friend	class	CMultiXL1WriteReq;
	friend	class	CMultiXL1CloseReq;
	friend	class	CMultiXL1RemoveReq;
	friend	class	CMultiXL2OpenReq;
	friend	class	CMultiXL2ConnectReq;
	friend	class	CMultiXL2ListenReq;
	friend	class	CMultiXL2WriteReq;
	friend	class	CMultiXL2CloseReq;
	friend	class	CMultiXL2RemoveReq;
	friend	class	CMultiXL3OpenReq;
	friend	class	CMultiXL3SendMsgReq;
	friend	class	CMultiXL3SendDataReq;
	friend	class	CMultiXL3CloseReq;
	friend	class	CMultiXL3RemoveReq;
	friend	class	CMultiXL3CloseProcessLinksReq;

public:
enum	RequestCodes
{
	L1OpenLink,
	L1ConnectLink,
	L1ListenLink,
	L1ReadLink,
	L1WriteLink,
	L1CloseLink,
	L1RemoveLink,
	


	L2OpenLink,
	L2ConnectLink,
	L2ListenLink,
	L2WriteLink,
	L2CloseLink,
	L2RemoveLink,




	L3OpenLink,
	L3SendMsg,
	L3SendData,
	L3CloseLink,
	L3RemoveLink,
	L3CloseProcessLinks,

	LastRequestCode
};
public:
	inline	RequestCodes	RequestCode(){return	m_RequestCode;}
	virtual ~CMultiXRequest();
private:
	CMultiXRequest(RequestCodes	RequestCode,CMultiXLayer	*Origin,CMultiXLayer	*Target);
	RequestCodes		m_RequestCode;
};




#define	__MULTIX_OBJECT_ID_INDICATOR	0x4903
template	<class	ObjectType,class	ObjectOwner>
class	MULTIXWIN32DLL_API	CMultiXVector
{
public:
	CMultiXVector(ObjectOwner	*pOwner,bool	bSync);
	virtual	~CMultiXVector();
public:
	class	MULTIXWIN32DLL_API	CObjectID
	{
public:
		ObjectType * Next();
		ObjectType * Prev();
		ObjectType * GetObject();
		ObjectType	*RemoveObject();
		bool IsValid();
		CObjectID();
		void	Init();
		virtual	~CObjectID();
		ObjectOwner	*&Owner();
		int32_t	&Index();
		int32_t	&Signature();
		int16_t	&IDIndicator();
		CMultiXVector<ObjectType,ObjectOwner>	*&Container();
		TMultiXGenericID	&GenericID();
		static	int	SignificantDataSize();
		const	byte_t	*SignificantData();
		void	operator=(const	uchar_t *Ptr);
		bool	operator	==	(CObjectID	&Second);
		operator	uchar_t	*();
		size_t	DataSize();
private:
		union	TIDData
		{
			TMultiXGenericID	I;
			struct
			{
				ObjectOwner	*m_pOwner;
				CMultiXVector<ObjectType,ObjectOwner>	*m_pContainer;
				int32_t	m_Index;
				int32_t	m_Signature;
				int16_t	m_IDIndicator;
			}	S;
		}	m_Data;
	};

	bool IsValid(CObjectID &ObjectID);
	ObjectType * GetObject(CObjectID &ObjectID);
	ObjectType * First();
	ObjectType * Next(CObjectID &ObjectID);
	ObjectType * Prev(CObjectID &ObjectID);
	ObjectType	*RemoveObject(CObjectID &ObjectID);
	CObjectID InsertObject(ObjectType	*Object);
	CMultiXSyncObject	&GetSyncObj();
	bool	IsEmpty();
	std::vector<ObjectType	*>	&Objects();
	size_t	Size();
	void	Lock();
	void	Unlock();
	void	DeleteAll();
private:
	std::vector<ObjectType	*>	m_Objects;
	int32_t	m_ObjectSignatures;
	ObjectOwner	*m_pOwner;
	bool	m_bSync;
	CMultiXMutex	m_Mutex;
};


template	<class	KeyType,class	ObjectType,class	ObjectOwner>
class	MULTIXWIN32DLL_API	CMultiXMap
{
private:
	typedef	typename	EXPORTABLE_STL::map<KeyType,ObjectType *>::iterator		TIterator;

public:
	CMultiXMap(ObjectOwner	*pOwner,bool	bSync);
	virtual	~CMultiXMap();
public:
	class	MULTIXWIN32DLL_API	CObjectID
	{
public:
		ObjectType * GetObject();
		ObjectType * Next();
		ObjectType * Prev();
		ObjectType	*RemoveObject();
		bool IsValid();
		CObjectID();
		void	Init();
		virtual	~CObjectID();
		ObjectOwner	*&Owner();
		KeyType	&Key();
		int32_t	&Signature();
		int16_t	&IDIndicator();
		CMultiXMap<KeyType,ObjectType,ObjectOwner>	*&Container();
		TMultiXGenericID	&GenericID();
		static	int	SignificantDataSize();
		const	byte_t	*SignificantData();
		bool	operator	==	(CObjectID	&Second);
		void	operator=(const	uchar_t *Ptr);
		operator	uchar_t	*();
		size_t	DataSize();
private:
		union	TIDData
		{
			TMultiXGenericID	I;
			struct
			{
				ObjectOwner	*m_pOwner;
				CMultiXMap<KeyType,ObjectType,ObjectOwner>	*m_pContainer;
				KeyType	m_Key;
				int32_t	m_Signature;
				int16_t	m_IDIndicator;
			}	S;
		}	m_Data;
	};
	ObjectType * GetFirst();
	ObjectType * GetObject(CObjectID &ObjectID);
	ObjectType * Next(CObjectID &ObjectID);
	ObjectType * Prev(CObjectID &ObjectID);
	ObjectType * Find(KeyType &Key);
	ObjectType	*RemoveObject(KeyType	Key);
	ObjectType	*RemoveObject(CObjectID &ObjectID);
	bool InsertObject(ObjectType	*Object,KeyType	Key);
	void	DeleteAll();
	CMultiXSyncObject	&GetSyncObj();
	EXPORTABLE_STL::map<KeyType,ObjectType *>	&Objects();
	bool	IsEmpty();
	size_t	Size();
	void	Lock();
	void	Unlock();
private:
	bool IsValid(CObjectID &ObjectID);
	EXPORTABLE_STL::map<KeyType,ObjectType *>	m_Objects;
private:
	int32_t	m_ObjectSignatures;
	ObjectOwner	*m_pOwner;
	bool	m_bSync;
protected:
	CMultiXMutex	m_Mutex;
};

template	<class	ObjectType,class	ObjectOwner>
class	MULTIXWIN32DLL_API	CMultiXInt32Map	:	public	CMultiXMap<int32_t,ObjectType,ObjectOwner>
{
public:
	CMultiXInt32Map(ObjectOwner	*pOwner,bool	bSync);
	virtual	~CMultiXInt32Map();
	bool InsertObject(ObjectType	*Object);
private:
	int32_t	m_NextKey;
};


class MULTIXWIN32DLL_API	CMultiXBufferArray  
{
public:
	int32_t DataSize()	const;
	CMultiXBuffer * GetBuffer(int Index)	const	;
	void Add(CMultiXBuffer &Buf);
	CMultiXBufferArray();
	virtual ~CMultiXBufferArray();
	inline	int	Count()	const	{return	(int)m_Buffers.size();}
private:
	std::vector<CMultiXBuffer *>	m_Buffers;
};



class	MULTIXWIN32DLL_API	CMultiXUtil
{
public:
	template<class	_T>
	static	MULTIXWIN32DLL_API	void	SwapInt(_T	&I);
};

class	MULTIXWIN32DLL_API	CMultiXDebugBuffer
{
public:
	CMultiXDebugBuffer(int	InitSize	=	0x100000)
	{
		m_pData	=	new	char[InitSize+1];
		m_Size	=	InitSize;
	}
	char	*Data(){return	m_pData;}
	int	Size(){return m_Size;}
	~CMultiXDebugBuffer()
	{
		if(m_pData)
			delete	[]	m_pData;
	}
	char	*m_pData;
	int	m_Size;
};

#endif // !defined(AFX_MULTIXUTIL_H__61BEF7CB_7367_4F5F_A40A_ECA19E45DD05__INCLUDED_)
