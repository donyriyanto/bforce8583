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
#include "StdAfx.h"
#include	<string>
#include	<queue>
#include	<deque>
#include	<list>
#include	<map>
//#include	<vector>
#ifdef	UseSTLWrapper
namespace	MultiXStlWrapper
{
#ifdef	WrapStdString
	MULTIXWIN32DLL_API	string	operator+(const	string &Left,const	string &Right)
	{
		string	RetVal	=	Left;
		RetVal	+=	Right;
		return	RetVal;
	}
	MULTIXWIN32DLL_API	string	operator+(const	char	*Left,const	string &Right)
	{
		string	RetVal	=	Left;
		RetVal	+=	Right;
		return	RetVal;
	}
	MULTIXWIN32DLL_API	string	operator+(const	string &Left,const	char	*Right)
	{
		string	RetVal	=	Left;
		RetVal	+=	Right;
		return	RetVal;
	}

	string::string()
	{
		m_pStr	=	new	std::string();
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
	}
	string::string(const	char	*x)
	{
		m_pStr	=	new	std::string(x);
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
	}
	string::string(const	string	&Other)
	{
		m_pStr	=	new	std::string(Other.c_str());
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
	}

	string::~string()
	{
		if(m_pStr)
		{
			const	char	*S	=	c_str();
			delete	(std::string *)m_pStr;
			m_pStr	=	NULL;
		}

	}
	
	bool	string::operator	==	(const	string	&Other)	const
	{
		return	*(std::string *)m_pStr	==	*(std::string *)Other.m_pStr;
	}

	bool	string::operator	==	(const	char	*Other)	const
	{
		return	*(std::string *)m_pStr	==	Other;
	}

	bool	string::operator	<	(const	string	&Other)	const
	{
		return	*(std::string *)m_pStr	<	*(std::string *)Other.m_pStr;
	}
	bool	string::operator	>	(const	string	&Other)	const
	{
		return	*(std::string *)m_pStr	>	*(std::string *)Other.m_pStr;
	}
	bool	string::operator	!=	(const	string	&Other)	const
	{
		return	*(std::string *)m_pStr	!=	*(std::string *)Other.m_pStr;
	}
	bool	string::empty()	const
	{
		return	((std::string *)m_pStr)->empty();
	};
	size_t	string::length()	const
	{
		return	((std::string *)m_pStr)->length();
	};
	size_t	string::size()	const
	{
	return	m_pStr	?	((std::string *)m_pStr)->size()	:	0;
	};
	const	char	*string::c_str()	const
	{
		return	((std::string *)m_pStr)->c_str();
	}
	void	string::clear()
	{
		((std::string *)m_pStr)->clear();
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
	}

	char	&string::operator[](size_t _Pos)
	{
		return	(*((std::string *)m_pStr))[_Pos];
	}

	size_t	string::find(char	*S)
	{
		return	((std::string *)m_pStr)->find(S);
	}
	size_t	string::find(char	C)
	{
		return	((std::string *)m_pStr)->find(C);
	}

	string	string::substr(size_t	offset,size_t len)
	{
		return	string(((std::string *)m_pStr)->substr(offset,len).c_str());
	}

	string	&string::operator+=(const	string &Other)
	{
		*((std::string *)m_pStr)	+=	*((std::string *)Other.m_pStr);
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
		return	*this;
	}
	string	&string::operator=(const	string &Other)
	{
		*((std::string *)m_pStr)	=	Other.c_str();
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
		return	*this;
	}
	string	&string::operator	=	(const	char	*x)
	{
		*((std::string *)m_pStr)	=	x;
		m_pCStrPtr	=	((std::string	*)m_pStr)->c_str();
		return	*this;
	}
#endif
#define	DefineQueue(ElementType)\
	void	Queue::Construct(ElementType *)	{\
		m_pQueue	=	new	std::queue<ElementType >;\
	}\
	ElementType &Queue::Front(ElementType	*){\
		return	((std::queue<ElementType >	*)m_pQueue)->front();\
	}\
	void	Queue::Destruct(ElementType*){\
		delete	(std::queue<ElementType >	*)m_pQueue;\
		m_pQueue	=	NULL;\
	}\
	bool	Queue::Empty(ElementType*) const{\
		return	((std::queue<ElementType >	*)m_pQueue)->empty();\
	}\
	void	Queue::Push(ElementType Elem ){\
		((std::queue<ElementType>	*)m_pQueue)->push(Elem);\
	}\
	void	Queue::Pop(ElementType*){\
		((std::queue<ElementType >	*)m_pQueue)->pop();\
	}
	DefineQueue(CMultiXEvent*)
	DefineQueue(CMultiXAppMsg*)
	DefineQueue(CMultiXMsgStack*)
	DefineQueue(CMultiXL2LinkMsgQueueEntry*)

	template	<class	ElementType>
	typename	std::deque<ElementType>::iterator	*IteratorPtr(typename	std::deque<ElementType>::iterator		It)
	{
		typename	std::deque<ElementType>::iterator		*Ptr	=	new	typename	std::deque<ElementType>::iterator	;
		*Ptr	=	It;
		return	Ptr;
	}


#define	DefineDeQueue(ElementType,Suffix)\
	void	DeQueue::Construct(ElementType *)	{\
		m_pQueue	=	new	std::deque<ElementType >;\
	}\
	void	DeQueue::Destruct(ElementType*)	{\
		delete	(std::deque<ElementType >	*)m_pQueue;\
		m_pQueue	=	NULL;\
	}\
	bool	DeQueue::Empty(ElementType *) const{\
		return	((std::deque<ElementType >	*)m_pQueue)->empty();\
	}\
	size_t	DeQueue::Size(ElementType *) const{\
	return	m_pQueue	?	((std::deque<ElementType >	*)m_pQueue)->size()	:	0;\
	}\
	ElementType &DeQueue::Front(ElementType *){\
		return	((std::deque<ElementType >	*)m_pQueue)->front();\
	}\
	void	DeQueue::PopFront(ElementType* ){\
		((std::deque<ElementType >	*)m_pQueue)->pop_front();\
	}\
	void	DeQueue::PushBack(ElementType E){\
		((std::deque<ElementType >	*)m_pQueue)->push_back(E);\
	}\
	void	DeQueue::PushFront(ElementType E){\
		((std::deque<ElementType >	*)m_pQueue)->push_front(E);\
	}\
	void	IteratorDestructor##Suffix(void	*Ptr)\
	{\
		delete	(std::deque<ElementType>::iterator	*)Ptr;\
	}\
	BaseIterator	DeQueue::Begin(ElementType*E){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::deque<ElementType>	*)m_pQueue)->begin()),IteratorDestructor##Suffix);\
	}\
	BaseIterator	DeQueue::End(ElementType*){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::deque<ElementType>	*)m_pQueue)->end()),IteratorDestructor##Suffix);\
	}\
	BaseIterator	DeQueue::IteratorCopy(ElementType*,void	*It){\
	return	BaseIterator(1,IteratorPtr<ElementType>(*((std::deque<ElementType>::iterator	*)It)),IteratorDestructor##Suffix);\
	}\
	bool	DeQueue::IteratorEQ(ElementType*,void	*First,void	*Second){\
	return	*((std::deque<ElementType>::iterator	*)First)	==	*((std::deque<ElementType>::iterator	*)Second);\
	}\
	BaseIterator	DeQueue::IteratorPostInc(ElementType*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<ElementType>((*(std::deque<ElementType>::iterator	*)Ptr)++),IteratorDestructor##Suffix);\
	}\
	BaseIterator	DeQueue::IteratorErase(ElementType*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::deque<ElementType >	*)m_pQueue)->erase(*((std::deque<ElementType>::iterator	*)Ptr))),IteratorDestructor##Suffix);\
	}\
	void	DeQueue::IteratorGetValue(ElementType	*Elem,void	*Ptr){\
	*Elem	=	*(*((std::deque<ElementType>::iterator	*)Ptr));\
	}\
	void	DeQueue::IteratorGetPtr(ElementType*Elem,void	*Ptr){\
	*Elem =	*((std::deque<ElementType>::iterator	*)Ptr)->operator->();\
	}\
	void	DestructIterator(ElementType	*,void	*Ptr){\
	delete	(std::deque<ElementType>::iterator	*)Ptr;\
	}
	DefineDeQueue(CMultiXMsgStack*,1);
	DefineDeQueue(CMultiXBuffer*,2);
	DefineDeQueue(CMultiXL2LinkMsgQueueEntry*,3);

	/*
	template	<class	ElementType>
	typename	std::vector<ElementType>::iterator	*IteratorPtr(typename	std::vector<ElementType>::iterator		It)
	{
		typename	std::vector<ElementType>::iterator		*Ptr	=	new	typename	std::vector<ElementType>::iterator	;
		*Ptr	=	It;
		return	Ptr;
	}


#define	DefineVector(ElementType,Suffix)\
	void	Vector::Construct(ElementType *)	{\
		m_pVector	=	new	std::vector<ElementType >;\
	}\
	void	Vector::Destruct(ElementType *)	{\
		delete	(std::vector<ElementType >	*)m_pVector;\
		m_pVector	=	NULL;\
	}\
	bool	Vector::Empty(ElementType *) const{\
		return	((std::vector<ElementType >	*)m_pVector)->empty();\
	}\
	size_t	Vector::Size(ElementType *) const{\
	return	m_pVector	?	((std::vector<ElementType >	*)m_pVector)->size()	:	0;\
	}\
	void	Vector::PushBack(ElementType E){\
		((std::vector<ElementType >	*)m_pVector)->push_back(E);\
	}\
	void	IteratorDestructor##Suffix(void	*Ptr)\
	{\
		delete	(std::vector<ElementType>::iterator	*)Ptr;\
	}\
	BaseIterator	Vector::Begin(ElementType *){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::vector<ElementType>	*)m_pVector)->begin()),IteratorDestructor##Suffix);\
	}\
	BaseIterator	Vector::End(ElementType *){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::vector<ElementType>	*)m_pVector)->end()),IteratorDestructor##Suffix);\
	}\
	BaseIterator	Vector::IteratorErase(ElementType*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::vector<ElementType >	*)m_pVector)->erase(*((std::vector<ElementType>::iterator	*)Ptr))),IteratorDestructor##Suffix);\
	}\
	void	Vector::IteratorGetValue(ElementType	*Elem,void	*Ptr){\
	*Elem	=	*(*((std::vector<ElementType>::iterator	*)Ptr));\
	}\
	BaseIterator	Vector::IteratorCopy(ElementType*,void	*It){\
	return	BaseIterator(1,IteratorPtr<ElementType>(*((std::vector<ElementType>::iterator	*)It)),IteratorDestructor##Suffix);\
	}\
	void	Vector::DestructIterator(ElementType	*,void	*Ptr){\
	delete	(std::vector<ElementType>::iterator	*)Ptr;\
	}\
	ElementType &Vector::GetAT(ElementType*,size_t	Pos )	const{\
			return	(*((std::vector<ElementType >	*)m_pVector))[Pos];\
	}

	DefineVector(CMultiXBuffer*,4						);
	DefineVector(CMultiXAlertableObject*,5	);
	DefineVector(CMultiXTransportDevice*,6	);
	DefineVector(CMultiXL3Link*,7						);
	DefineVector(CMultiXL2Link*,8						);
	DefineVector(CMultiXLink*,9							);
	DefineVector(std::string,10							);
*/
	template	<class	ElementType>
	typename	std::list<ElementType>::iterator	*IteratorPtr(typename	std::list<ElementType>::iterator		It)
	{
		typename	std::list<ElementType>::iterator		*Ptr	=	new	typename	std::list<ElementType>::iterator	;
		*Ptr	=	It;
		return	Ptr;
	}


#define	DefineList(ElementType,Suffix)\
	void	List::Construct(ElementType *)	{\
		m_pList	=	new	std::list<ElementType >;\
	}\
	void	List::Destruct(ElementType *)	{\
		delete	(std::list<ElementType >	*)m_pList;\
		m_pList	=	NULL;\
	}\
	bool	List::Empty(ElementType *) const{\
		return	((std::list<ElementType >	*)m_pList)->empty();\
	}\
	size_t	List::Size(ElementType *) const{\
	return	m_pList	?	((std::list<ElementType >	*)m_pList)->size()	:	0;\
	}\
	void	List::PushBack(ElementType E){\
		((std::list<ElementType >	*)m_pList)->push_back(E);\
	}\
	void	IteratorDestructor##Suffix(void	*Ptr)\
	{\
		delete	(std::list<ElementType>::iterator	*)Ptr;\
	}\
	BaseIterator	List::Begin(ElementType *){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::list<ElementType>	*)m_pList)->begin()),IteratorDestructor##Suffix);\
	}\
	BaseIterator	List::End(ElementType *){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::list<ElementType>	*)m_pList)->end()),IteratorDestructor##Suffix);\
	}\
	bool	List::IteratorEQ(ElementType*,void	*First,void	*Second){\
	return	*((std::list<ElementType>::iterator	*)First)	==	*((std::list<ElementType>::iterator	*)Second);\
	}\
	BaseIterator	List::IteratorCopy(ElementType*,void	*It){\
	return	BaseIterator(1,IteratorPtr<ElementType>(*((std::list<ElementType>::iterator	*)It)),IteratorDestructor##Suffix);\
	}\
	BaseIterator	List::IteratorPostInc(ElementType*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<ElementType>((*(std::list<ElementType>::iterator	*)Ptr)++),IteratorDestructor##Suffix);\
	}\
	BaseIterator	List::IteratorErase(ElementType*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<ElementType>(((std::list<ElementType >	*)m_pList)->erase(*((std::list<ElementType>::iterator	*)Ptr))),IteratorDestructor##Suffix);\
	}\
	void	List::IteratorGetValue(ElementType*Elem,void	*Ptr){\
	*Elem	=	*(*((std::list<ElementType>::iterator	*)Ptr));\
	}\
	void												List::DestructIterator(ElementType	*,void	*Ptr){\
	delete	(std::list<ElementType>::iterator	*)Ptr;\
	}
	DefineList(CMultiXL3Link*,11);

	template	<class	KeyType,class	ElementType>
	typename	std::map<KeyType,ElementType>::iterator	*IteratorPtr(typename	std::map<KeyType,ElementType>::iterator		It)
	{
		typename	std::map<KeyType,ElementType>::iterator		*Ptr	=	new	typename	std::map<KeyType,ElementType>::iterator	;
		*Ptr	=	It;
		return	Ptr;
	}


#define	DefineMap(KeyType,ElementType,Suffix)\
void	IteratorDestructor##Suffix(void	*Ptr)\
{\
	delete	(std::map<KeyType,ElementType>::iterator	*)Ptr;\
}\
void	Map::Construct(ElementType*,KeyType*)	{\
		m_pMap	=	new	std::map<KeyType,ElementType >;\
	}\
void	Map::Destruct(ElementType*,KeyType*){\
		delete	(std::map<KeyType,ElementType>	*)m_pMap;\
		m_pMap	=	NULL;\
	}\
bool	Map::Empty(ElementType*,KeyType* ) const{\
		return	((std::map<KeyType,ElementType >	*)m_pMap)->empty();\
	}\
ElementType &Map::GetAT(ElementType*,KeyType Val)	const{\
			return	(*((std::map<KeyType,ElementType>	*)m_pMap))[Val];\
	}\
	size_t	Map::Erase(ElementType*,const	KeyType	&Val){\
		return	((std::map<KeyType,ElementType >	*)m_pMap)->erase(Val);\
	}\
void	Map::Clear(ElementType*,KeyType*){\
		return	((std::map<KeyType,ElementType >	*)m_pMap)->clear();\
	}\
BaseIterator	Map::Find(ElementType*,KeyType Val){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::map<KeyType,ElementType>	*)m_pMap)->find(Val)),IteratorDestructor##Suffix);\
	}\
BaseIterator	Map::Begin(ElementType*,KeyType*){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::map<KeyType,ElementType>	*)m_pMap)->begin()),IteratorDestructor##Suffix);\
	}\
BaseIterator	Map::End(ElementType*,KeyType*){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::map<KeyType,ElementType>	*)m_pMap)->end()),IteratorDestructor##Suffix);\
	}\
void	Map::DestructIterator(pair<KeyType,ElementType>	*,void	*It){\
	delete	(std::map<KeyType,ElementType>::iterator	*)It;\
	}\
void	Map::Insert(ElementType*,KeyType*,void	*It1,void	*It2){\
	((std::map<KeyType,ElementType >	*)m_pMap)->insert(*((std::map<KeyType,ElementType>::iterator	*)It1),*((std::map<KeyType,ElementType>::iterator	*)It2));\
	}\
BaseIterator	Map::IteratorCopy(pair<KeyType,ElementType>	*,void	*It){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(*((std::map<KeyType,ElementType>::iterator	*)It)),IteratorDestructor##Suffix);\
	}\
bool	Map::IteratorEQ(pair<KeyType,ElementType>	*,void	*First,void	*Second){\
	return	*((std::map<KeyType,ElementType>::iterator	*)First)	==	*((std::map<KeyType,ElementType>::iterator	*)Second);\
	}\
BaseIterator	Map::IteratorPostInc(pair<KeyType,ElementType>	*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>((*(std::map<KeyType,ElementType>::iterator	*)Ptr)++),IteratorDestructor##Suffix);\
	}\
BaseIterator	Map::IteratorPostDec(pair<KeyType,ElementType>	*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>((*(std::map<KeyType,ElementType>::iterator	*)Ptr)--),IteratorDestructor##Suffix);\
	}\
BaseIterator	Map::IteratorErase(ElementType*,KeyType*,void	*Ptr){\
	typedef	std::map<KeyType,ElementType>::iterator Iterator;\
	Iterator	It1	=	(*((Iterator	*)Ptr));\
	Iterator	It2	=	It1++;\
	/*IteratorPtr<KeyType,ElementType>(((std::map<KeyType,ElementType >	*)m_pMap)->erase(*((Iterator	*)Ptr)));*/\
	((std::map<KeyType,ElementType >	*)m_pMap)->erase(It2);\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(It1),IteratorDestructor##Suffix);\
	}\
void	Map::IteratorGetPtr(pair<KeyType,ElementType>	*Value,void	*Ptr){\
	std::map<KeyType,ElementType>::value_type	*V;\
	V	=	((std::map<KeyType,ElementType>::iterator	*)Ptr)->operator->();\
	Value->first	=	V->first;\
	Value->second	=	V->second;\
	}\
void	Map::IteratorGetValue(pair<KeyType,ElementType>	*Value,void	*Ptr){\
	std::map<KeyType,ElementType>::value_type	*V;\
	V	=	((std::map<KeyType,ElementType>::iterator	*)Ptr)->operator->();\
	Value->first	=	V->first;\
	Value->second	=	V->second;\
	}\
size_t	Map::Size(ElementType*,KeyType*) const{\
	return	m_pMap	?	((std::map<KeyType,ElementType >	*)m_pMap)->size()	:	0;\
	}

	DefineMap(TMultiXProcID,	CMultiXL3Process*,20);
	DefineMap(TMultiXProcID,	CMultiXProcess*,21);
	DefineMap(int32_t,	CMultiXAppMsg*,22);
	DefineMap(uint32_t,	void*,23);
	DefineMap(int32_t,	void*,231);
	DefineMap(int64_t,	void*,232);
	DefineMap(std::string,	void*,24);
	DefineMap(TMultiXProcID,	TMultiXProcID,25);
	DefineMap(std::string,	std::string,26);
	DefineMap(CMultiXSessionID,	CMultiXSession*,27);
	DefineMap(int32_t,	std::string,28);
	DefineMap(std::string,	int32_t,281);



	template	<class	KeyType,class	ElementType>
	typename	std::multimap<KeyType,ElementType>::iterator	*IteratorPtr(typename	std::multimap<KeyType,ElementType>::iterator		It,int x)
	{
		typename	std::multimap<KeyType,ElementType>::iterator		*Ptr	=	new	typename	std::multimap<KeyType,ElementType>::iterator	;
		*Ptr	=	It;
		return	Ptr;
	}


#define	DefineMultimap(KeyType,ElementType,Suffix)\
void	IteratorDestructor##Suffix(void	*Ptr)\
{\
	delete	(std::multimap<KeyType,ElementType>::iterator	*)Ptr;\
}\
void	Multimap::Construct(ElementType*,KeyType*)	{\
		m_pMultimap	=	new	std::multimap<KeyType,ElementType >;\
	}\
void	Multimap::Destruct(ElementType*,KeyType*){\
		delete	(std::multimap<KeyType,ElementType>	*)m_pMultimap;\
		m_pMultimap	=	NULL;\
	}\
BaseIterator	Multimap::Begin(ElementType*,KeyType*){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::multimap<KeyType,ElementType>	*)m_pMultimap)->begin(),1),IteratorDestructor##Suffix);\
	}\
BaseIterator	Multimap::End(ElementType*,KeyType*){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::multimap<KeyType,ElementType>	*)m_pMultimap)->end(),1),IteratorDestructor##Suffix);\
	}\
void	Multimap::DestructIterator(pair<KeyType,ElementType>	*,void	*It){\
	delete	(std::multimap<KeyType,ElementType>::iterator	*)It;\
	}\
bool	Multimap::IteratorEQ(pair<KeyType,ElementType>	*,void	*First,void	*Second){\
	return	*((std::multimap<KeyType,ElementType>::iterator	*)First)	==	*((std::multimap<KeyType,ElementType>::iterator	*)Second);\
	}\
BaseIterator	Multimap::IteratorCopy(pair<KeyType,ElementType>	*,void	*It){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(*((std::multimap<KeyType,ElementType>::iterator	*)It),1),IteratorDestructor##Suffix);\
	}\
BaseIterator	Multimap::IteratorPostInc(pair<KeyType,ElementType>	*,void	*Ptr){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>((*(std::multimap<KeyType,ElementType>::iterator	*)Ptr)++,1),IteratorDestructor##Suffix);\
	}\
BaseIterator	Multimap::IteratorErase(ElementType*,KeyType*,void	*Ptr){\
	typedef	std::multimap<KeyType,ElementType>::iterator Iterator;\
	Iterator	It1	=	(*((Iterator	*)Ptr));\
	Iterator	It2	=	It1++;\
	/*IteratorPtr<KeyType,ElementType>(((std::map<KeyType,ElementType >	*)m_pMap)->erase(*((Iterator	*)Ptr)));*/\
	((std::multimap<KeyType,ElementType >	*)m_pMultimap)->erase(It2);\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(It1,1),IteratorDestructor##Suffix);\
	/*return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::multimap<KeyType,ElementType >	*)m_pMultimap)->erase(*((std::multimap<KeyType,ElementType>::iterator	*)Ptr)),1),IteratorDestructor##Suffix);*/\
	}\
BaseIterator	Multimap::Insert(const	pair<KeyType,ElementType>	&Value){\
	return	BaseIterator(1,IteratorPtr<KeyType,ElementType>(((std::multimap<KeyType,ElementType >	*)m_pMultimap)->insert(std::multimap<KeyType,ElementType>::value_type(Value.first,Value.second)),1),IteratorDestructor##Suffix);\
	}\
void	Multimap::IteratorGetPtr(pair<KeyType,ElementType>	*Value,void	*Ptr){\
	std::multimap<KeyType,ElementType>::value_type	*V;\
	V	=	((std::multimap<KeyType,ElementType>::iterator	*)Ptr)->operator->();\
	Value->first	=	V->first;\
	Value->second	=	V->second;\
	}\
size_t	Multimap::Size(ElementType*,KeyType*) const{\
	return	m_pMultimap	?	((std::multimap<KeyType,ElementType >	*)m_pMultimap)->size()	:	0;\
	}

	DefineMultimap(MultiXMilliClock,CMultiXTimer*,30);


};
template	class EXPORTABLE_STL::map<int32_t,std::string>;
template	class	EXPORTABLE_STL::pair<int32_t,std::string>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<int32_t,std::string>, EXPORTABLE_STL::map<int32_t,std::string> >;

template	class EXPORTABLE_STL::map<uint32_t,void	*>;
template	class	EXPORTABLE_STL::pair<uint32_t,void	*>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<uint32_t,void	*>, EXPORTABLE_STL::map<uint32_t,void	*> >;

template	class EXPORTABLE_STL::map<int32_t,void	*>;
template	class	EXPORTABLE_STL::pair<int32_t,void	*>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<int32_t,void	*>, EXPORTABLE_STL::map<int32_t,void	*> >;

template	class EXPORTABLE_STL::map<int64_t,void	*>;
template	class	EXPORTABLE_STL::pair<int64_t,void	*>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<int64_t,void	*>, EXPORTABLE_STL::map<int64_t,void	*> >;

template	class EXPORTABLE_STL::map<std::string,void	*>;
template	class	EXPORTABLE_STL::pair<std::string,void	*>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,void	*>, EXPORTABLE_STL::map<std::string,void	*> >;

template	class EXPORTABLE_STL::map<std::string,int32_t>;
template	class	EXPORTABLE_STL::pair<std::string,int32_t>;
template	class	EXPORTABLE_STL::ContainerIterator<EXPORTABLE_STL::pair<std::string,int32_t>, EXPORTABLE_STL::map<std::string,int32_t> >;
#endif

