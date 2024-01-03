/*
 ***************************************************************************************************
 * This file is part of WIRELESS CONNECTIVITY SDK for STM32:
 *
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE THAT WÜRTH ELEKTRONIK
 * EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY KIND RELATED TO, BUT NOT LIMITED
 * TO THE NON-INFRINGEMENT OF THIRD PARTIES’ INTELLECTUAL PROPERTY RIGHTS OR THE
 * MERCHANTABILITY OR FITNESS FOR YOUR INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT
 * WARRANT OR REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY PATENT
 * RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY RIGHT RELATING TO ANY
 * COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT IS USED. INFORMATION PUBLISHED BY
 * WÜRTH ELEKTRONIK EISOS REGARDING THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE
 * FROM WÜRTH ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR ENDORSEMENT
 * THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS DRIVER PACKAGE.
 *
 * COPYRIGHT (c) 2023 Würth Elektronik eiSos GmbH & Co. KG
 *
 ***************************************************************************************************
 */
#include <stdio.h>
#include <AdrasteaI/Examples/ATProprietaryExamples.h>
#include <AdrasteaI/ATCommands/ATPacketDomain.h>
#include <AdrasteaI/ATCommands/ATProprietary.h>
#include <AdrasteaI/AdrasteaI.h>
#include <AdrasteaI/ATCommands/ATEvent.h>
#include <AdrasteaI/Examples/AdrasteaI_Examples.h>

void AdrasteaI_ATPacketDomain_EventCallback(char *eventText);

static AdrasteaI_ATPacketDomain_Network_Registration_Status_t status = {
		.state = 0 };

/**
 * @brief This example connects to the cellular network and checks the domain of the connection
 *
 */
void ATPacketDomainExample()
{
	printf("*** Start of Adrastea-I ATPacketDomain example ***\r\n");

	if (!AdrasteaI_Init(&AdrasteaI_uart, &AdrasteaI_pins, &AdrasteaI_ATPacketDomain_EventCallback))
	{
		printf("Initialization error\r\n");
		return;
	}

	bool ret = AdrasteaI_ATPacketDomain_SetNetworkRegistrationResultCode(AdrasteaI_ATPacketDomain_Network_Registration_Result_Code_Enable_with_Location_Info);
	AdrasteaI_ExamplesPrint("Set Network Registration Result Code", ret);
	while (status.state != AdrasteaI_ATPacketDomain_Network_Registration_State_Registered_Roaming)
	{
		WE_Delay(10);
	}

	AdrasteaI_ATPacketDomain_Network_Registration_Status_t statusRead;
	ret = AdrasteaI_ATPacketDomain_ReadNetworkRegistrationStatus(&statusRead);
	AdrasteaI_ExamplesPrint("Read Network Registration Status", ret);
	if (ret)
	{
		printf("Result Code: %d, State: %d, TAC: %s, ECI: %s, AcT: %d\r\n", statusRead.resultCode, statusRead.state, statusRead.TAC, statusRead.ECI, statusRead.AcT);
	}

	AdrasteaI_ATPacketDomain_PDP_Context_t context = {
			.cid = 2,
			.pdpType = AdrasteaI_ATPacketDomain_PDP_Type_IPv4,
			.apnName = "web.vodafone.de" };
	ret = AdrasteaI_ATPacketDomain_DefinePDPContext(context);
	AdrasteaI_ExamplesPrint("Define PDP Context", ret);

	ret = AdrasteaI_ATPacketDomain_ReadPDPContexts();
	AdrasteaI_ExamplesPrint("Read PDP Contexts", ret);

	ret = AdrasteaI_ATPacketDomain_ReadPDPContextsState();
	AdrasteaI_ExamplesPrint("Read PDP Contexts State", ret);
}

void AdrasteaI_ATPacketDomain_EventCallback(char *eventText)
{
	AdrasteaI_ATEvent_t event;
	if (false == AdrasteaI_ATEvent_ParseEventType(&eventText, &event))
	{
		return;
	}

	switch (event)
	{
	case AdrasteaI_ATEvent_PacketDomain_Network_Registration_Status:
	{
		AdrasteaI_ATPacketDomain_ParseNetworkRegistrationStatusEvent(eventText, &status);
		break;
	}
	case AdrasteaI_ATEvent_PacketDomain_PDP_Context:
	{
		AdrasteaI_ATPacketDomain_PDP_Context_t pdpContext;
		if (!AdrasteaI_ATPacketDomain_ParsePDPContextEvent(eventText, &pdpContext))
		{
			return;
		}
		printf("CID: %d, PDP Type: %d, APN Name: %s\r\n", pdpContext.cid, pdpContext.pdpType, pdpContext.apnName);
		break;
	}
	case AdrasteaI_ATEvent_PacketDomain_PDP_Context_State:
	{
		AdrasteaI_ATPacketDomain_PDP_Context_CID_State_t pdpContextState;
		if (!AdrasteaI_ATPacketDomain_ParsePDPContextStateEvent(eventText, &pdpContextState))
		{
			return;
		}
		printf("CID: %d, State: %d\r\n", pdpContextState.cid, pdpContextState.state);
		break;
	}
	default:
		break;
	}
}
