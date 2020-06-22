#if defined (USE_TINY)
	#include "nfcm1833tiny.h"
#elif defined (USE_TINZ)
	#include "nfcm1833tinz.h"
#endif

protocol_status_t select_tag_type(tag_type_t tag_type)
{
#if defined (USE_TINY)
	return select_tag_type_uart(tag_type);
#elif defined (USE_TINZ)
	return select_tag_type_spi(tag_type);
#endif
}

send_recv_status_t sendrecv(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result)
{
#if defined (USE_TINY)
	return sendrecv_uart(command,command_len,data_result);
#elif defined (USE_TINZ)
	return sendrecv_spi(command,command_len,data_result);
#endif
}

send_recv_status_t sendrecv_with_iso14443a(uint8_t * command, uint8_t command_len, cr95hf_recv_format_t * data_result)
{
#if defined (USE_TINZ)
	return sendrecv_spi_with_iso14443A(command,command_len,data_result);
#endif
}

ping_status ping_module(void)
{
#if defined (USE_TINY)
	return ping_module_uart();
#elif defined (USE_TINZ)
	return ping_module_spi();
#endif
}

send_recv_status_t getDeviceID(uint8_t * deviceID)
{
#if defined (USE_TINY)
	return getDeviceID_uart(deviceID);
#elif defined (USE_TINZ)
	return getDeviceID_spi(deviceID);
#endif
}

send_recv_status_t getDeviceID_with_iso14443a(uint8_t * deviceID)
{
#if defined (USE_TINZ)
	return getDeviceID_type4A_spi(deviceID);
#endif
}

void WakeUp_CR95HF (void)
{
#if defined (USE_TINZ)
	WakeUp_TinZ();
#endif
}