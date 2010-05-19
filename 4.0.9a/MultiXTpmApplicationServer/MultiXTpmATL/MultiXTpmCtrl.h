// MultiXTpmCtrl.h : Declaration of the CMultiXTpmCtrl
#pragma once
#include "resource.h"       // main symbols
#include <atlctl.h>

#if defined(_WIN32_WCE) && !defined(_CE_DCOM) && !defined(_CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA)
#error "Single-threaded COM objects are not properly supported on Windows CE platform, such as the Windows Mobile platforms that do not include full DCOM support. Define _CE_ALLOW_SINGLE_THREADED_OBJECTS_IN_MTA to force ATL to support creating single-thread COM object's and allow use of it's single-threaded COM object implementations. The threading model in your rgs file was set to 'Free' as that is the only threading model supported in non DCOM Windows CE platforms."
#endif


// IMultiXTpmCtrl
[
	object,
	uuid(683A2477-5398-4241-99DE-A89D2EC1A194),
	dual,
	helpstring("IMultiXTpmCtrl Interface"),
	pointer_default(unique)
]
__interface IMultiXTpmCtrl : public IDispatch
{
	[id(1), helpstring("method Shutdown")] HRESULT Shutdown([in] LONG GracePeriod);
	[id(2), helpstring("method Start")] HRESULT Start([out,retval] VARIANT_BOOL* pRetVal);
	[propget, id(3), helpstring("property WorkingDirectory")] HRESULT WorkingDirectory([out, retval] BSTR* pVal);
	[propput, id(3), helpstring("property WorkingDirectory")] HRESULT WorkingDirectory([in] BSTR newVal);
	[propget, id(4), helpstring("property WebServicePort")] HRESULT WebServicePort([out, retval] USHORT* pVal);
	[propput, id(4), helpstring("property WebServicePort")] HRESULT WebServicePort([in] USHORT newVal);
	[propget, id(5), helpstring("property ConfigFileName")] HRESULT ConfigFileName([out, retval] BSTR* pVal);
	[propput, id(5), helpstring("property ConfigFileName")] HRESULT ConfigFileName([in] BSTR newVal);
	[id(6), helpstring("method StartWithID")] HRESULT StartWithID([in] LONG ProcID, [out,retval] VARIANT_BOOL* pRetVal);
};


// CMultiXTpmCtrl
[
	coclass,
	control,
	default(IMultiXTpmCtrl),
	threading(apartment),
	vi_progid("MultiXTpmATL.MultiXTpmCtrl"),
	progid("MultiXTpmATL.MultiXTpmCtrl.1"),
	version(1.0),
	uuid("50A93A17-D58A-4AF5-BA4A-0565074E92F4"),
	helpstring("MultiXTpmCtrl Class"),
	support_error_info(IMultiXTpmCtrl),
	registration_script("control.rgs")
]
class ATL_NO_VTABLE CMultiXTpmCtrl :
	public IMultiXTpmCtrl,
	public IPersistStreamInitImpl<CMultiXTpmCtrl>,
	public IOleControlImpl<CMultiXTpmCtrl>,
	public IOleObjectImpl<CMultiXTpmCtrl>,
	public IOleInPlaceActiveObjectImpl<CMultiXTpmCtrl>,
	public IViewObjectExImpl<CMultiXTpmCtrl>,
	public IOleInPlaceObjectWindowlessImpl<CMultiXTpmCtrl>,
	public IPersistStorageImpl<CMultiXTpmCtrl>,
	public ISpecifyPropertyPagesImpl<CMultiXTpmCtrl>,
	public IQuickActivateImpl<CMultiXTpmCtrl>,
#ifndef _WIN32_WCE
	public IDataObjectImpl<CMultiXTpmCtrl>,
#endif
	public IProvideClassInfo2Impl<&__uuidof(CMultiXTpmCtrl), NULL>,
#ifdef _WIN32_WCE // IObjectSafety is required on Windows CE for the control to be loaded correctly
	public IObjectSafetyImpl<CMultiXTpmCtrl, INTERFACESAFE_FOR_UNTRUSTED_CALLER>,
#endif
	public CComControl<CMultiXTpmCtrl>
{

private:
	CTpmApp	*m_pTpmApp;
	USHORT	m_WSPort;
	std::string	m_ConfigFileName;

public:
	CContainedWindow m_ctlStatic;

#pragma warning(push)
#pragma warning(disable: 4355) // 'this' : used in base member initializer list


	CMultiXTpmCtrl()
		: m_ctlStatic(_T("Static"), this, 1)
	{
		m_pTpmApp	=	NULL;
		m_bWindowOnly = FALSE;
	}

#pragma warning(pop)

DECLARE_OLEMISC_STATUS(OLEMISC_RECOMPOSEONRESIZE |
	OLEMISC_CANTLINKINSIDE |
	OLEMISC_INSIDEOUT |
	OLEMISC_ACTIVATEWHENVISIBLE |
	OLEMISC_SETCLIENTSITEFIRST
)


BEGIN_PROP_MAP(CMultiXTpmCtrl)
	PROP_DATA_ENTRY("_cx", m_sizeExtent.cx, VT_UI4)
	PROP_DATA_ENTRY("_cy", m_sizeExtent.cy, VT_UI4)
	// Example entries
	// PROP_ENTRY("Property Description", dispid, clsid)
	// PROP_PAGE(CLSID_StockColorPage)
END_PROP_MAP()


BEGIN_MSG_MAP(CMultiXTpmCtrl)
	MESSAGE_HANDLER(WM_CREATE, OnCreate)
	MESSAGE_HANDLER(WM_SETFOCUS, OnSetFocus)
	CHAIN_MSG_MAP(CComControl<CMultiXTpmCtrl>)
ALT_MSG_MAP(1)
	// Replace this with message map entries for superclassed Static
END_MSG_MAP()
// Handler prototypes:
//  LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
//  LRESULT CommandHandler(WORD wNotifyCode, WORD wID, HWND hWndCtl, BOOL& bHandled);
//  LRESULT NotifyHandler(int idCtrl, LPNMHDR pnmh, BOOL& bHandled);

	LRESULT OnSetFocus(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = CComControl<CMultiXTpmCtrl>::OnSetFocus(uMsg, wParam, lParam, bHandled);
		if (m_bInPlaceActive)
		{
			if(!IsChild(::GetFocus()))
				m_ctlStatic.SetFocus();
		}
		return lRes;
	}

	LRESULT OnCreate(UINT /*uMsg*/, WPARAM /*wParam*/, LPARAM /*lParam*/, BOOL& /*bHandled*/)
	{
		RECT rc;
		GetWindowRect(&rc);
		rc.right -= rc.left;
		rc.bottom -= rc.top;
		rc.top = rc.left = 0;
		m_ctlStatic.Create(m_hWnd, rc);
		return 0;
	}

	STDMETHOD(SetObjectRects)(LPCRECT prcPos,LPCRECT prcClip)
	{
		IOleInPlaceObjectWindowlessImpl<CMultiXTpmCtrl>::SetObjectRects(prcPos, prcClip);
		int cx, cy;
		cx = prcPos->right - prcPos->left;
		cy = prcPos->bottom - prcPos->top;
		::SetWindowPos(m_ctlStatic.m_hWnd, NULL, 0,
			0, cx, cy, SWP_NOZORDER | SWP_NOACTIVATE);
		return S_OK;
	}

// IViewObjectEx
	DECLARE_VIEW_STATUS(VIEWSTATUS_SOLIDBKGND | VIEWSTATUS_OPAQUE)

// IMultiXTpmCtrl

	DECLARE_PROTECT_FINAL_CONSTRUCT()


	HRESULT FinalConstruct()
	{
		if(m_pTpmApp)
		{
			delete	m_pTpmApp;
		}
		m_pTpmApp	=	new	CTpmApp(1);

		return S_OK;
	}

	void FinalRelease()
	{
		if(m_pTpmApp)
		{
			delete	m_pTpmApp;
			m_pTpmApp	=	NULL;
		}
	}



	STDMETHOD(Shutdown)(LONG GracePeriod);
	STDMETHOD(Start)(VARIANT_BOOL* pRetVal);
	STDMETHOD(get_WorkingDirectory)(BSTR* pVal);
	STDMETHOD(put_WorkingDirectory)(BSTR newVal);
	STDMETHOD(get_WebServicePort)(USHORT* pVal);
	STDMETHOD(put_WebServicePort)(USHORT newVal);
	STDMETHOD(get_ConfigFileName)(BSTR* pVal);
	STDMETHOD(put_ConfigFileName)(BSTR newVal);
	STDMETHOD(StartWithID)(LONG ProcID, VARIANT_BOOL* pRetVal);

	};

