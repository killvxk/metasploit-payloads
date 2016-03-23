/*!
 * @file powershell_bindings.cpp
 * @brief Wrapper functions for bridging native meterp calls to powershell
 */
extern "C" {
#include "../../common/common.h"
#include "powershell_bindings.h"
}

Remote* gRemote = NULL;

VOID MeterpreterInvoke(unsigned int isLocal, unsigned char* input, unsigned int inputLength, unsigned char** output, unsigned int* outputLength)
{
	dprintf("[PSH BINDING] Input %p of %d bytes received", input, inputLength);

	Packet packet = { 0 };
	packet.header = *(PacketHeader*)input;
	packet.header.length = ntohl(packet.header.length);
	packet.payload = (PUCHAR)(input + sizeof(PacketHeader));
	packet.payloadLength = (ULONG)inputLength - sizeof(TlvHeader);
	packet.local = isLocal == 1;

	dprintf("[PSH BINDING] Packet header length:  %u", packet.header.length);
	dprintf("[PSH BINDING] Packet header type:    %u", packet.header.type);
	dprintf("[PSH BINDING] Packet payload length: %u", packet.payloadLength);
	dprintf("[PSH BINDING] Packet local flag:     %u", isLocal);

	command_handle(gRemote, &packet);

	if (packet.partner != NULL)
	{
		dprintf("[PSH BINDING] Response packet generated");
		*output = (unsigned char*)LocalAlloc(LHND, packet.partner->payloadLength);
		*outputLength = packet.partner->payloadLength;
		memcpy(*output, packet.partner->payload, packet.partner->payloadLength);
		packet_destroy(packet.partner);
	}
	else
	{
		dprintf("[PSH BINDING] Response packet not generated");
		*output = NULL;
		*outputLength = 0;
	}
}