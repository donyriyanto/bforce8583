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
// COMISO8583Msg.h : Declaration of the CCOMISO8583Msg

#pragma once
#include "resource.h"       // main symbols
#include	<ISO8583Shared.h>



#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif
// ICOMISO8583Msg
[
	object,
	uuid("28E2A70A-5DB0-407C-919C-8A032DBDB6AF"),
	dual,	helpstring("ICOMISO8583Msg Interface"),
	pointer_default(unique)
]
__interface ICOMISO8583Msg : IDispatch
{
	[propget, id(1), helpstring("property MTI")] HRESULT MTI([out, retval] LONG* pVal);
	[propget, id(2), helpstring("property ElementValue")] HRESULT ElementValue([in] LONG ElementID, [out, retval] VARIANT* pVal);
	[propput, id(2), helpstring("property ElementValue")] HRESULT ElementValue([in] LONG ElementID, [in] VARIANT newVal);
	[id(3), helpstring("method FromISO")] HRESULT FromISO([in] VARIANT ISO8583Buffer, [out,retval] CISO8583Msg::TValidationError* Error);
	[id(4), helpstring("method ToISO")] HRESULT ToISO([out] VARIANT* ISO8583Buffer, [out,retval] CISO8583Msg::TValidationError* Error);
	[id(5), helpstring("method SetMTI")] HRESULT SetMTI(LONG NewMTI, [out,retval] CISO8583Msg::TValidationError* Error);
	[propget, id(6), helpstring("property LastProcessedElementID")] HRESULT LastProcessedElementID([out, retval] LONG* pVal);

	[propget, id(7), helpstring("property MTIVersion")]		HRESULT	MTIVersion	([out, retval] CISO8583Msg::TMTIVersion	* pVal);
	[propput, id(7), helpstring("property MTIVersion")]		HRESULT	MTIVersion	([in]	CISO8583Msg::TMTIVersion	newVal);
	[propget, id(8), helpstring("property MTIClass")]			HRESULT	MTIClass		([out, retval] CISO8583Msg::TMTIClass		* pVal);
	[propput, id(8), helpstring("property MTIClass")]			HRESULT	MTIClass		([in]	CISO8583Msg::TMTIClass	newVal);
	[propget, id(9), helpstring("property MTIFunction")]	HRESULT	MTIFunction	([out, retval] CISO8583Msg::TMTIFunction	* pVal);
	[propput, id(9), helpstring("property MTIFunction")]	HRESULT	MTIFunction	([in]	CISO8583Msg::TMTIFunction	newVal);
	[propget, id(10),helpstring("property MTIOrigin")]		HRESULT	MTIOrigin		([out, retval] CISO8583Msg::TMTIOrigin		* pVal);
	[propput, id(10),helpstring("property MTIOrigin")]		HRESULT	MTIOrigin		([in]	CISO8583Msg::TMTIOrigin	newVal);
	[propget, id(11), helpstring("property ElementsIDs")] HRESULT ElementsIDs([out, retval] VARIANT* pVal);
	[propget, id(12), helpstring("property XML")] HRESULT XML([out, retval] BSTR* pVal);
	[propput, id(12), helpstring("property XML")] HRESULT XML([in] BSTR newVal);
};



// CCOMISO8583Msg

[
	coclass,
	default(ICOMISO8583Msg),
	threading(apartment),
	vi_progid("ISO8583ATL.COMISO8583Msg"),
	progid("ISO8583ATL.COMISO8583Msg.1"),
	version(1.0),
	uuid("27A803F2-116D-4CFE-AC95-B1B593D78283"),
	helpstring("COMISO8583Msg Class")
]
class ATL_NO_VTABLE CCOMISO8583Msg :
	public ICOMISO8583Msg
{
public:
	CCOMISO8583Msg()
	{
	}



	DECLARE_PROTECT_FINAL_CONSTRUCT()

	HRESULT FinalConstruct()
	{
		return S_OK;
	}

	void FinalRelease()
	{
	}

	CISO8583Msg	m_Msg;
public:

	STDMETHOD(get_MTI)(LONG* pVal);
	STDMETHOD(get_ElementValue)(LONG ElementID, VARIANT* pVal);
	STDMETHOD(put_ElementValue)(LONG ElementID, VARIANT newVal);
	STDMETHOD(FromISO)(VARIANT ISO8583Buffer, CISO8583Msg::TValidationError* Error);
	STDMETHOD(ToISO)(VARIANT* ISO8583Buffer, CISO8583Msg::TValidationError* Error);
	STDMETHOD(SetMTI)(LONG NewMTI, CISO8583Msg::TValidationError* Error);
	STDMETHOD(get_LastProcessedElementID)(LONG* pVal);
	STDMETHOD(get_MTIVersion	)(CISO8583Msg::TMTIVersion	* pVal);
	STDMETHOD(put_MTIVersion	)(CISO8583Msg::TMTIVersion	newVal);
	STDMETHOD(get_MTIClass		)(CISO8583Msg::TMTIClass		* pVal);
	STDMETHOD(put_MTIClass		)(CISO8583Msg::TMTIClass	newVal);
	STDMETHOD(get_MTIFunction	)(CISO8583Msg::TMTIFunction	* pVal);
	STDMETHOD(put_MTIFunction	)(CISO8583Msg::TMTIFunction	newVal);
	STDMETHOD(get_MTIOrigin		)(CISO8583Msg::TMTIOrigin		* pVal);
	STDMETHOD(put_MTIOrigin		)(CISO8583Msg::TMTIOrigin	newVal);




	STDMETHOD(get_ElementsIDs)(VARIANT* pVal);
	STDMETHOD(get_XML)(BSTR* pVal);
	STDMETHOD(put_XML)(BSTR newVal);
};


class CISO8583ATLApp : public CMultiXApp  
{
public:
	CISO8583ATLApp()	:
		CMultiXApp(0,NULL,"","",1)
	{
	}

	/*
	void OnStartup();
	void OnTpmConfiguredLink(CMultiXTpmConfiguredLink &Link);
	CMultiXSession * CreateNewSession(const	TMultiXSessionID &SessionID);
	CMultiXProcess * AllocateNewProcess(TMultiXProcID ProcID);
	virtual ~CISO8583AcquirerGatewayFEApp();
	void OnPrepareForShutdown(int32_t WaitTime);
	void OnProcessRestart();
	void OnProcessSuspend();
	void OnProcessResume();
	void OnShutdown();
	bool OnMultiXException(CMultiXException &e);
	void OnTpmConfigCompleted();
	void OnApplicationEvent(CMultiXEvent *Event);
	void OnConnectProcessFailed(TMultiXProcID ProcessID);
	CMultiXLogger	*Logger(){return	MultiXLogger();}
	const	std::string	&MyGatewayID();
	std::string	m_TransactionOriginatorIdentificationCode;
	int	m_NextSTAN;
	std::string GetNextSTAN(void);
	CISO8583AcquirerGatewayFELink * FindReadyLink(std::string	RemoteGatewayID);
	*/
};
