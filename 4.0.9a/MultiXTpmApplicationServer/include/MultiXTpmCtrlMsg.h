// TpmCtrlMsg.h: interface for the CTpmCtrlMsg class.
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


#if !defined(AFX_TPMCTRLMSG_H__70307252_E1F8_4266_B2DD_32CE2002E549__INCLUDED_)
#define AFX_TPMCTRLMSG_H__70307252_E1F8_4266_B2DD_32CE2002E549__INCLUDED_
#ifdef	ShowInclude
#pragma message( "Include " __FILE__ )
#endif

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class MULTIXWIN32DLL_API	CMultiXTpmCtrlMsg : public CMultiXMsg  
{
public:
	enum	MsgCodes
	{
		ConfigDataMsgCode	=	-200,
		SetListeningAddressMsgCode,
		GetListeningAddressMsgCode,
		LeaveSessionMsgCode,
		KillSessionMsgCode,
		SessionMemberLeftMsgCode,
		SessionKilledMsgCode,
		ProcessShutdownMsgCode,
		ProcessRestartMsgCode,
		ProcessSuspendMsgCode,
		ProcessResumeMsgCode
	};
	
	enum
	{
		DebugLevelItemCode	=	10,
		StartLinkItemCode,
		LinkIDItemCode,				
		LinkDescriptionItemCode,
		LinkTypeItemCode,			
		OpenModeItemCode,			
		IsRawItemCode,				
		LocalAddressItemCode,	
		LocalPortItemCode,		
		RemoteAddressItemCode,
		RemotePortItemCode,		
		ApplicationItemCode_Obsolete,	
		EndLinkItemCode,
		ProcessIDItemCode,
		ProcessClassItemCode,
		DatabaseServerItemCode_Obsolete,
		DatabaseNameItemCode_Obsolete,
		DatabaseUserItemCode_Obsolete,
		DatabasePasswordItemCode_Obsolete,
		IsEncryptedItemCode_Obsolete,
		MaxConnectRetriesItemCode_Obsolete,
		ConnectRetriesDelayItemCode_Obsolete,
		GracePeriodItemCode,
		DefaultSendTimeoutItemCode,
		ProtocolInitMsgTimeoutItemCode_Obsolete,
		ProtocolDataMsgTimeoutItemCode_Obsolete,
		MaxSendRetriesItemCode_Obsolete,
		StartParamItemCode,
		ParamNameItemCode,
		ParamValueItemCode,
		EndProcessParamItemCode,
		EndLinkParamItemCode,
		AdditionalConfigTextItemCode,
		LastItemCode
	};
	

	CMultiXTpmCtrlMsg(int32_t	MsgCode,CMultiXApp	&Owner);
	virtual ~CMultiXTpmCtrlMsg();
	static	int	LayerIndicator(){return	8;}
private:
	CMultiXBuffer & NewBuf(CMultiXApp &Owner);
	CMultiXBuffer	*m_pNewBuf;

};

#endif // !defined(AFX_TPMCTRLMSG_H__70307252_E1F8_4266_B2DD_32CE2002E549__INCLUDED_)
