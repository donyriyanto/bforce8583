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

#ifndef	STLWrapperIncluded
#define	STLWrapperIncluded
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif


class	CMultiXEvent;
class	CMultiXMsgStack;
class	CMultiXBuffer;
class	CMultiXAlertableObject;
class	CMultiXTransportDevice;
class	CMultiXL3Link;
class	CMultiXL2Link;
class	CMultiXLink;
class	CMultiXProcess;
class	CMultiXL3Process;
class	CMultiXAppMsg;
class	CMultiXSessionID;
class	CMultiXSession;
class	CMultiXTimer;
class	CMultiXL2LinkMsgQueueEntry;

typedef	int32_t	TMultiXProcID;
#include	<string>

#ifdef	UseSTLWrapper
#include	<vector>
/*
template class MULTIXWIN32DLL_API std::vector<CMultiXBuffer *>;
template class MULTIXWIN32DLL_API std::vector<CMultiXL2Link *>;
template class MULTIXWIN32DLL_API std::vector<CMultiXTransportDevice *>;
template class MULTIXWIN32DLL_API std::vector<CMultiXL3Link *>;
template class MULTIXWIN32DLL_API std::vector<CMultiXLink *>;
template class MULTIXWIN32DLL_API std::vector<CMultiXAlertableObject *>;
template class MULTIXWIN32DLL_API std::vector<std::string>;
*/
namespace	MultiXStlWrapper
{

#ifdef	WrapStdString
	class	MULTIXWIN32DLL_API	string
	{
	public:
		string();
		string(const	char	*x);
		string(const	string	&);
		virtual	~string();
		bool	operator	==	(const	string	&Other)	const;
		bool	operator	==	(const	char	*Other)	const;
		bool	operator	<	(const	string	&Other)	const;
		bool	operator	>	(const	string	&Other)	const;
		bool	operator	!=	(const	string	&Other)	const;
		bool	empty()	const;
		size_t	length() const;
		const	char	*c_str()	const;
		/*
		operator	const	char	*()	const
		{
			return	c_str();
		}
		*/
		void	clear();
		char &operator[](size_t _Pos);
		size_t	find(char *);
		size_t	find(char);
		string	substr(size_t=0,size_t=(size_t)(-1));
		size_t	size() const;
		string	&operator+=(const	string &Other);
//		string	&operator=(string Other);
		string	&operator=(const	string &Other);
		string	&	operator	=	(const	char	*x);
		static const size_t npos = (size_t)-1;
	private:
		void	*m_pStr;
		const	char	*m_pCStrPtr;
	};

	extern	MULTIXWIN32DLL_API	string	operator+(const	string &Left,const	string &Right);
	extern	MULTIXWIN32DLL_API	string	operator+(const	char	*Left,const	string &Right);
	extern	MULTIXWIN32DLL_API	string	operator+(const	string &Left,const	char	*Right);
	/*
#else
	/*
	class	MULTIXWIN32DLL_API	string	:	public	std::string
	{
	public:
		string()	:	std::string(){}
		string(const	char	*x):	std::string(x){}
		string(const	string	&x):	std::string(x){}
	};
	*/
#endif

	class		MULTIXWIN32DLL_API	Queue
	{
	public:
		Queue()	:	m_pQueue(NULL)
		{
		};
		virtual	~Queue(){};
#define	DeclareQueue(ElementType)\
			void	Construct(ElementType *);\
			void	Destruct(ElementType *);\
			void	Push(ElementType Elem);\
			bool	Empty(ElementType *) const;\
			ElementType &Front(ElementType *);\
			void	Pop(ElementType *);

	DeclareQueue(CMultiXEvent*)
	DeclareQueue(CMultiXAppMsg*)
	DeclareQueue(CMultiXMsgStack*)
	DeclareQueue(CMultiXL2LinkMsgQueueEntry*)



	private:
			void	*m_pQueue;
	};

	template	<class	ObjectType>
	class	MULTIXWIN32DLL_API		queue
	{
	public:
		queue()
		{
			m_Queue.Construct((ObjectType	*)0);
		}
		virtual	~queue()
		{
			m_Queue.Destruct((ObjectType	*)0);
		}
		void	push(ObjectType	Elem)
		{
			m_Queue.Push(Elem);
		}
		bool	empty()	const
		{
			return m_Queue.Empty((ObjectType	*)0);
		}
		const ObjectType& front()
		{
			return	m_Queue.Front((ObjectType	*)0);
		}
		void	pop()
		{
			m_Queue.Pop((ObjectType	*)0);
		}
	private:
		Queue	m_Queue;
	};


	class	MULTIXWIN32DLL_API	 BaseIterator
	{
	public:
		BaseIterator()
		{
			m_pSTLIt	=	NULL;
			m_pRefCount	=	NULL;
			m_pIteratorDelete	=	NULL;
		}

		BaseIterator(int	x,void	*pSTLIt,void	(*pIteratorDelete)(void	*))
		{
			m_pSTLIt	=	pSTLIt;
			m_pRefCount	=	new	int[1];
			m_pRefCount[0]	=	1;
			m_pIteratorDelete	=	pIteratorDelete;
		}


		virtual	~BaseIterator()
		{
			if(m_pRefCount)
			{
				m_pRefCount[0]--;
				if(m_pRefCount[0]	==	0)
				{
					delete	[]	m_pRefCount;
					m_pRefCount	=	NULL;
				}
			}
			if(m_pRefCount	==	NULL	&&	m_pSTLIt)
			{
				m_pIteratorDelete(m_pSTLIt);
				m_pSTLIt	=	NULL;
			}
		}

		int	RefCount()	const
		{
			if(m_pRefCount	==	NULL)
				return	0;
			return	*m_pRefCount;
		}
		int	AddRef()
		{
			if(m_pRefCount)
			{
				m_pRefCount[0]++;
				return	m_pRefCount[0];
			}
			return	0;
		}
		int	Release()
		{
			if(m_pRefCount)
			{
				m_pRefCount[0]--;
				if(m_pRefCount[0]	>	0)
					return	m_pRefCount[0];
				delete	[]	m_pRefCount;
				m_pRefCount	=	NULL;
			}
			if(m_pRefCount	==	NULL	&&	m_pSTLIt)
			{
				m_pIteratorDelete(m_pSTLIt);
				m_pSTLIt	=	NULL;
			}
			return	0;
		}

		void	Attach(const	BaseIterator	&Other)
		{
			m_pRefCount	=	Other.m_pRefCount;
			m_pSTLIt	=	Other.m_pSTLIt;
			m_pIteratorDelete	=	Other.m_pIteratorDelete;
			AddRef();
		}

		void	*STLIt()	const
		{
			return	m_pSTLIt;
		}
		/*
	protected:
		BaseIterator(const	BaseIterator	&Other)
		{
			m_pSTLIt	=	Other.m_pSTLIt;
			m_pRefCount	=	Other.m_pRefCount;
			m_pRefCount[0]++;
		}
		*/

	protected:
		void	*m_pSTLIt;
		int		*m_pRefCount;
		void	(*m_pIteratorDelete)(void	*);
	};

	template	<class	ObjectType,class	Container>
	class	MULTIXWIN32DLL_API		ContainerIterator	:	public	BaseIterator
	{
	public:
		Container	*m_pCont;
		ObjectType	m_Value;

		ContainerIterator(Container	*pCont,const	BaseIterator	&Other)	:
			BaseIterator()
		{
			m_pCont	=	pCont;
			Attach(m_pCont->m_Functions.IteratorCopy((ObjectType	*)0,Other.STLIt()));
		}
		ContainerIterator(const	ContainerIterator	&Other)	:
			BaseIterator()
		{
			m_pCont	=	Other.m_pCont;
			Attach(m_pCont->m_Functions.IteratorCopy((ObjectType	*)0,Other.STLIt()));
		}
/*
		ContainerIterator(const	ContainerIterator	*Other)	:
			ContainerIterator(Other->m_pCont,*Other)
		{
		}
*/


		ContainerIterator operator=(const	ContainerIterator	&Other)
		{
			Release();
			/*
			if(Release()	==	0)
			{
				if(STLIt()	&&	m_pCont)
					m_pCont->m_Functions.DestructIterator((ObjectType *)0,STLIt());
			}
			*/
			Attach(m_pCont->m_Functions.IteratorCopy((ObjectType	*)0,Other.STLIt()));
			return	*this;
		}

		virtual	~ContainerIterator()
		{
			/*
			if(RefCount()	<=	1)
			{
				if(STLIt()	&&	m_pCont)
					m_pCont->m_Functions.DestructIterator((ObjectType *)0,STLIt());
			}
			*/
		}
		bool	operator	!=	(const	ContainerIterator	&Other)	const
		{
			return	!m_pCont->m_Functions.IteratorEQ((ObjectType	*)0,STLIt(),Other.STLIt());
		}
		bool	operator	==	(const	ContainerIterator	&Other)	const
		{
			return	m_pCont->m_Functions.IteratorEQ((ObjectType	*)0,STLIt(),Other.STLIt());
		}
		ContainerIterator operator++(int)	//	post increment
		{
			BaseIterator	B	=	m_pCont->m_Functions.IteratorPostInc((ObjectType	*)0,STLIt());
			ContainerIterator	C	=ContainerIterator(m_pCont,B);
			return	C;
		}
		ContainerIterator operator--(int)	//	post deccrement
		{
			BaseIterator	B	=	m_pCont->m_Functions.IteratorPostDec((ObjectType	*)0,STLIt());
			ContainerIterator	C	=ContainerIterator(m_pCont,B);
			return	C;
		}
		ObjectType	*operator->()
		{
			m_pCont->m_Functions.IteratorGetPtr(&m_Value,STLIt());
			return	&m_Value;
		}
		ObjectType	&operator*()
		{
			m_pCont->m_Functions.IteratorGetValue(&m_Value,STLIt());
			return	m_Value;
		}
	};




	class	MULTIXWIN32DLL_API		DeQueue
	{
	public:
		DeQueue()	:	m_pQueue(NULL)
		{
		};
		virtual	~DeQueue(){};
#define	DeclareDeQueue(ElementType)\
			void	Construct(ElementType *);\
			void	Destruct(ElementType *);\
			bool	Empty(ElementType *) const;\
			size_t	Size(ElementType *) const;\
			ElementType &Front(ElementType *);\
			void	PopFront(ElementType *);\
			void	PushBack(ElementType);\
			void	PushFront(ElementType);\
			BaseIterator	Begin(ElementType *);\
			BaseIterator	End(ElementType *);\
			bool	IteratorEQ(ElementType	*,void	*First,void	*Second);\
			BaseIterator	IteratorPostInc(ElementType	*,void	*);\
			BaseIterator	IteratorCopy(ElementType	*,void	*);\
			BaseIterator	IteratorErase(ElementType	*,void	*);\
			void	IteratorGetValue(ElementType	*,void	*);\
			void	IteratorGetPtr(ElementType	*,void	*);\
			void	DestructIterator(ElementType *,void	*);

	DeclareDeQueue(CMultiXBuffer*)
	DeclareDeQueue(CMultiXMsgStack*)
	DeclareDeQueue(CMultiXL2LinkMsgQueueEntry*)

	private:
			void	*m_pQueue;
	};



	template	<class	ObjectType>
	class	MULTIXWIN32DLL_API		deque
	{
	public:
		DeQueue	m_Functions;

		class	iterator;
		friend	class	iterator;
		class	iterator	:	public	ContainerIterator<ObjectType, deque<ObjectType> >
		{
		public:
			iterator(deque<ObjectType>	*pCont, const	BaseIterator	&Other)	:
				ContainerIterator<ObjectType, deque<ObjectType> >(pCont,Other)
				{
				}

		};
		deque()
		{
			m_Functions.Construct((ObjectType	*)0);
		}
		virtual	~deque()
		{
			m_Functions.Destruct((ObjectType	*)0);
		}

		bool	empty()	const
		{
			return	m_Functions.Empty((ObjectType	*)0);
		}
		const ObjectType& front()
		{
			return	m_Functions.Front((ObjectType	*)0);
		}
		void	pop_front()
		{
			m_Functions.PopFront((ObjectType	*)0);
		}
		iterator	begin()
		{
			BaseIterator	B	=	m_Functions.Begin((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	end()
		{
			BaseIterator	B	=	m_Functions.End((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	erase(iterator	It)
		{
			BaseIterator	B	=	m_Functions.IteratorErase((ObjectType	*)0,It.STLIt());
			iterator	I	=	iterator(this,B);
			return	I;
		}

		void	push_back(ObjectType Elem)
		{
			m_Functions.PushBack(Elem);
		}
		void	push_front(ObjectType Elem)
		{
			m_Functions.PushFront(Elem);
		}
		size_t	size()	const
		{
			return	m_Functions.Size((ObjectType	*)0);
		}
	};



/*
	class	MULTIXWIN32DLL_API		Vector
	{
	public:
		Vector()	:	m_pVector(NULL)
		{
		};
		virtual	~Vector(){};
#define	DeclareVector(ElementType)\
			void	Construct(ElementType *);\
			void	Destruct(ElementType *);\
			ElementType &GetAT(ElementType	*,size_t	Pos )	const;\
			void	PushBack(ElementType );\
			bool	Empty(ElementType *) const;\
			BaseIterator	Begin(ElementType *);\
			BaseIterator	End(ElementType *);\
			void	IteratorGetValue(ElementType*,void	*);\
			BaseIterator	IteratorCopy(ElementType	*,void	*);\
			BaseIterator	IteratorErase(ElementType*,void	*);\
			void					DestructIterator(ElementType	*,void	*);\
			size_t	Size(ElementType *) const;
	
	DeclareVector(CMultiXBuffer*)
	DeclareVector(CMultiXAlertableObject*)
	DeclareVector(CMultiXTransportDevice*)
	DeclareVector(CMultiXL3Link*)
	DeclareVector(CMultiXL2Link*)
	DeclareVector(CMultiXLink*)
	DeclareVector(std::string)
	private:
			void	*m_pVector;
	};

	template	<class	ObjectType>
	class	MULTIXWIN32DLL_API		vector
	{
	public:
		Vector	m_Functions;

		class	iterator;
		friend	class	iterator;
		class	iterator	:	public	ContainerIterator<ObjectType,vector<ObjectType> >
		{
		public:
			iterator(vector<ObjectType>	*pCont, const	BaseIterator	&Other)	:
				ContainerIterator<ObjectType, vector<ObjectType> >(pCont,Other)
				{
				}
		};
	public:
		vector()
		{
			m_Functions.Construct((ObjectType	*)0);
		}
		virtual	~vector()
		{
			m_Functions.Destruct((ObjectType	*)0);
		}
		size_t	size()	const
		{
			return	m_Functions.Size((ObjectType	*)0);
		}
		ObjectType	& operator[](size_t _Pos)	const
		{
			return	m_Functions.GetAT((ObjectType	*)0,_Pos);
		}
		void	push_back(ObjectType Elem)
		{
			m_Functions.PushBack(Elem);
		}
		bool	empty()	const
		{
			return	m_Functions.Empty((ObjectType	*)0);
		}
		iterator	begin()
		{
			BaseIterator	B	=	m_Functions.Begin((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	end()
		{
			BaseIterator	B	=	m_Functions.End((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	erase(iterator	It)
		{
			BaseIterator	B	=	m_Functions.IteratorErase((ObjectType	*)0,It.STLIt());
			iterator	I	=	iterator(this,B);
			return	I;
		}

	};
*/
	class	MULTIXWIN32DLL_API		List
	{
	public:
		List()	:	m_pList(NULL)
		{
		};
		virtual	~List(){};
#define	DeclareList(ElementType)\
			void	Construct(ElementType *);\
			void	Destruct(ElementType *);\
			void	PushBack(ElementType );\
			bool	Empty(ElementType *) const;\
			BaseIterator	Begin(ElementType *);\
			BaseIterator	End(ElementType *);\
			BaseIterator	IteratorErase(ElementType*,void	*);\
			void	IteratorGetValue(ElementType*,void	*);\
			void	DestructIterator(ElementType	*,void	*);\
			bool	IteratorEQ(ElementType	*,void	*First,void	*Second);\
			BaseIterator	IteratorCopy(ElementType	*,void	*);\
			BaseIterator	IteratorPostInc(ElementType*,void	*);\
			size_t	Size(ElementType *) const;
	DeclareList(CMultiXL3Link*)

	private:
			void	*m_pList;
	};

	template	<class	ObjectType>
	class	MULTIXWIN32DLL_API		list
	{
	public:
		List	m_Functions;

		class	iterator;
		friend	class	iterator;
		class	iterator	:	public	ContainerIterator<ObjectType,list<ObjectType> >
		{
		public:
			iterator(list<ObjectType>	*pCont, const	BaseIterator	&Other)	:
				ContainerIterator<ObjectType, list<ObjectType> >(pCont,Other)
				{
				}
		};
	public:
		list()
		{
			m_Functions.Construct((ObjectType	*)0);
		}
		virtual	~list()
		{
			m_Functions.Destruct((ObjectType	*)0);
		}
		size_t	size()	const
		{
			return	m_Functions.Size((ObjectType	*)0);
		}
		void	push_back(ObjectType Elem)
		{
			m_Functions.PushBack(Elem);
		}
		bool	empty()	const
		{
			return	m_Functions.Empty((ObjectType	*)0);
		}
		iterator	begin()
		{
			BaseIterator	B	=	m_Functions.Begin((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	end()
		{
			BaseIterator	B	=	m_Functions.End((ObjectType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	erase(iterator	It)
		{
			BaseIterator	B	=	m_Functions.IteratorErase((ObjectType	*)0,It.STLIt());
			iterator	I	=	iterator(this,B);
			return	I;
		}
	};





	template	<class	T1,class	T2>
	class	MULTIXWIN32DLL_API		pair
	{
	public:
		pair(const	T1	&f,const T2	&s)
		{
			first	=	f;
			second	=	s;
		}
		pair(){};
		T1	first;
		T2	second;
	};

	class	MULTIXWIN32DLL_API		Map
	{
	public:
		Map()	:	m_pMap(NULL)
		{
		};
		virtual	~Map(){};
#define	DeclareMap(KeyType,ElementType)\
		void	Construct(ElementType*,KeyType*);\
		void	Destruct(ElementType*,KeyType*);\
			bool	Empty(ElementType*,KeyType* ) const;\
			ElementType &GetAT(ElementType*,KeyType)	const;\
			size_t	Erase(ElementType*,const	KeyType	&);\
			void	Clear(ElementType*,KeyType*);\
			BaseIterator	Find(ElementType*,KeyType);\
			BaseIterator	Begin(ElementType*,KeyType*);\
			BaseIterator	End(ElementType*,KeyType*);\
			void	DestructIterator(pair<KeyType,ElementType>	*,void	*);\
			void	Insert(ElementType*,KeyType*,void	*,void	*);\
			bool	IteratorEQ(pair<KeyType,ElementType>	*,void	*First,void	*Second);\
			BaseIterator	IteratorPostInc(pair<KeyType,ElementType>	*,void	*);\
			BaseIterator	IteratorPostDec(pair<KeyType,ElementType>	*,void	*);\
			BaseIterator	IteratorCopy(pair<KeyType,ElementType>	*,void	*);\
			BaseIterator	IteratorErase(ElementType*,KeyType*,void	*);\
			void	IteratorGetPtr(pair<KeyType,ElementType>	*,void	*);\
			void	IteratorGetValue(pair<KeyType,ElementType>	*,void	*);\
			size_t	Size(ElementType*,KeyType*) const;

	DeclareMap(TMultiXProcID,	CMultiXL3Process*)
	DeclareMap(TMultiXProcID,	CMultiXProcess*)
	DeclareMap(int32_t,	CMultiXAppMsg*)
	DeclareMap(int32_t,	std::string)
	DeclareMap(uint32_t,	void*)
	DeclareMap(int32_t,	void*)
	DeclareMap(int64_t,	void*)
	DeclareMap(std::string,	void*)
	DeclareMap(TMultiXProcID,	TMultiXProcID)
	DeclareMap(std::string,	std::string)
	DeclareMap(CMultiXSessionID,	CMultiXSession*)
	DeclareMap(std::string,	int32_t)

	private:
			void	*m_pMap;
	};


	template	<class	KeyType,class	ObjectType>
	class	MULTIXWIN32DLL_API		map
	{
	public:
		Map	m_Functions;

		class	iterator;
		friend	class	iterator;
		class	iterator	:	public	ContainerIterator<pair<KeyType,ObjectType>, map<KeyType,ObjectType> >
		{
		public:
			iterator(map<KeyType,ObjectType>	*pCont, const	BaseIterator	&Other)	:
				ContainerIterator<pair<KeyType,ObjectType>, map<KeyType,ObjectType> >(pCont,Other)
				{
				}
			iterator(const	iterator	&Other)	:
				ContainerIterator<pair<KeyType,ObjectType>, map<KeyType,ObjectType> >(Other)
				{
				}

		};
		map()
		{
			m_Functions.Construct((ObjectType	*)0,(KeyType	*)0);
		}
		virtual	~map()
		{
			m_Functions.Destruct((ObjectType	*)0,(KeyType	*)0);
		}
		size_t	size()	const
		{
			return	m_Functions.Size((ObjectType	*)0,(KeyType	*)0);
		}
		ObjectType	& operator[](KeyType Pos)	const
		{
			return	m_Functions.GetAT((ObjectType	*)0,Pos);
		}
		iterator	find(const KeyType &KeyVal)
		{
			BaseIterator	B	=	m_Functions.Find((ObjectType	*)0,KeyVal);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	begin()
		{
			BaseIterator	B	=	m_Functions.Begin((ObjectType	*)0,(KeyType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	end()
		{
			BaseIterator	B	=	m_Functions.End((ObjectType	*)0,(KeyType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		void	clear()
		{
			return	m_Functions.Clear((ObjectType	*)0,(KeyType	*)0);
		}


		bool	empty()	const
		{
			return	m_Functions.Empty((ObjectType	*)0,(KeyType	*)0);
		}
		iterator	erase(iterator	It)
		{
			BaseIterator	B	=	m_Functions.IteratorErase((ObjectType	*)0,(KeyType	*)0,It.STLIt());
			iterator	I	=	iterator(this,B);
			return	I;
		}
		size_t	erase(const	KeyType	&Val)
		{
			return	m_Functions.Erase((ObjectType	*)0,Val);
		}
		void insert(iterator It1,iterator It2)
		{
			return	m_Functions.Insert((ObjectType	*)0,(KeyType	*)0,It1.STLIt(),It2.STLIt());
		}
	};

	class	MULTIXWIN32DLL_API		Multimap
	{
	public:
		Multimap()	:	m_pMultimap(NULL)
		{
		};
		virtual	~Multimap(){};
#define	DeclareMultimap(KeyType,ElementType)\
		void	Construct(ElementType*,KeyType*);\
		void	Destruct(ElementType*,KeyType*);\
			BaseIterator	Begin(ElementType*,KeyType*);\
			BaseIterator	IteratorErase(ElementType*,KeyType*,void	*);\
			BaseIterator	Insert(const	pair<KeyType,ElementType> &Val);\
			BaseIterator	End(ElementType*,KeyType*);\
			void	DestructIterator(pair<KeyType,ElementType>	*,void	*);\
			bool	IteratorEQ(pair<KeyType,ElementType>	*,void	*First,void	*Second);\
			BaseIterator	IteratorCopy(pair<KeyType,ElementType>	*,void	*);\
			BaseIterator	IteratorPostInc(pair<KeyType,ElementType>	*,void	*);\
			void	IteratorGetPtr(pair<KeyType,ElementType>	*,void	*);\
			size_t	Size(ElementType*,KeyType*) const;
	
	DeclareMultimap(MultiXMilliClock,CMultiXTimer*)
	private:
			void	*m_pMultimap;
	};


	template	<class	KeyType,class	ObjectType>
	class	MULTIXWIN32DLL_API		multimap
	{
	public:
		typedef	pair<KeyType,ObjectType>	value_type;

		Multimap	m_Functions;

		class	iterator;
		friend	class	iterator;
		class	iterator	:	public	ContainerIterator<pair<KeyType,ObjectType>, multimap<KeyType,ObjectType> >
		{
		public:
			iterator(multimap<KeyType,ObjectType>	*pCont, const	BaseIterator	&Other)	:
				ContainerIterator<pair<KeyType,ObjectType>, multimap<KeyType,ObjectType> >(pCont,Other)
				{
				}

		};
		multimap()
		{
			m_Functions.Construct((ObjectType	*)0,(KeyType	*)0);
		}
		virtual	~multimap()
		{
			m_Functions.Destruct((ObjectType	*)0,(KeyType	*)0);
		}
		size_t	size()	const
		{
			return	m_Functions.Size((ObjectType	*)0,(KeyType	*)0);
		}
		iterator	begin()
		{
			BaseIterator	B	=	m_Functions.Begin((ObjectType	*)0,(KeyType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	end()
		{
			BaseIterator	B	=	m_Functions.End((ObjectType	*)0,(KeyType	*)0);
			iterator	I	=	iterator(this,B);
			return	I;
		}
		iterator	erase(iterator	It)
		{
			BaseIterator	B	=	m_Functions.IteratorErase((ObjectType	*)0,(KeyType	*)0,It.STLIt());
			iterator	I	=	iterator(this,B);
			return	I;

		}
		iterator	insert(const	value_type &Val)
		{
			BaseIterator	B	=	m_Functions.Insert(Val);
			iterator	I	=	iterator(this,B);
			return	I;
		}

	};

};
#endif

#endif

