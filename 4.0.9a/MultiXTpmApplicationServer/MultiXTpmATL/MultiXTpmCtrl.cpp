// MultiXTpmCtrl.cpp : Implementation of CMultiXTpmCtrl
#include "stdafx.h"
#include "MultiXTpmCtrl.h"
#include	<direct.h>
#include	<comdef.h>


// CMultiXTpmCtrl

STDMETHODIMP CMultiXTpmCtrl::Shutdown(LONG GracePeriod)
{
	if(m_pTpmApp	&&	m_pTpmApp->IsRunning())
	{
		CMultiXTpmAppEvent	*Event	=	new	CMultiXTpmAppEvent(CMultiXTpmAppEvent::ShutdownRequest,GracePeriod);
		m_pTpmApp->QueueEvent(Event);
		m_pTpmApp->WaitEnd();
		delete	m_pTpmApp;
		m_pTpmApp	=	NULL;
	}
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::Start(VARIANT_BOOL* pRetVal)
{
	// TODO: Add your implementation code here
	return	StartWithID(1,pRetVal);
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::get_WorkingDirectory(BSTR* pVal)
{
	// TODO: Add your implementation code here
	char	xx[300];
	_getcwd(xx,290);
	*pVal	=	bstr_t(xx);
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::put_WorkingDirectory(BSTR newVal)
{
	// TODO: Add your implementation code here
	SetCurrentDirectory(bstr_t(newVal));
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::get_WebServicePort(USHORT* pVal)
{
	// TODO: Add your implementation code here
	*pVal	=	m_WSPort;	
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::put_WebServicePort(USHORT newVal)
{
	// TODO: Add your implementation code here
	m_WSPort	=	newVal;
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::get_ConfigFileName(BSTR* pVal)
{
	// TODO: Add your implementation code here
	*pVal	=	bstr_t(m_ConfigFileName.c_str());
	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::put_ConfigFileName(BSTR newVal)
{
	// TODO: Add your implementation code here
	m_ConfigFileName	=	bstr_t(newVal);

	return S_OK;
}

STDMETHODIMP CMultiXTpmCtrl::StartWithID(LONG ProcID, VARIANT_BOOL* pRetVal)
{
	// TODO: Add your implementation code here
	if(!m_pTpmApp)
		m_pTpmApp	=	new	CTpmApp(ProcID);
	if(!m_pTpmApp->IsRunning())
	{
		char	X[1000];
		sprintf(X,"%d",m_WSPort);
		m_pTpmApp->WSPort()	=	X;
		sprintf(X,"MultiXTpmConfig=%s",m_ConfigFileName.c_str());
		putenv(X);
		m_pTpmApp->ResetProcID(ProcID);
		m_pTpmApp->Start();
	}
	*pRetVal	=	TRUE;
	return S_OK;

}
