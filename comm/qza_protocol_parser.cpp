#include "qza_protocol_parser.h"
#include "comm_macro.h"
using namespace comm;
int CQzaParser::CheckPacket(const void *qza_packet, int len)throw()
{
    if(unlikely(!qza_packet))
    {
        ERRMSG("qza_packet is null.");
        return RTN_ERROR;
    }
    if(unlikely(len < (int)GetProtocolStructLen()))
    {
        ERRMSG("packet-len field invalid. buf-len:%d less than:%d", len, GetProtocolStructLen());
        return RTN_ERROR;
    }
    if(unlikely(GetLen(qza_packet) != (unsigned)len))
    {
        ERRMSG("packet length(in head):%d not equal to actual recved-len:%d.", GetLen(qza_packet), len);
        return RTN_ERROR;
    }
    return RTN_SUCCESS;
}
int CQzaParser::CheckPacket(const void *qza_packet, int len, unsigned flow)throw()
{
    if(unlikely(CheckPacket(qza_packet, len)))return RTN_ERROR;

    if(unlikely(flow && GetSerialNo(qza_packet) != flow))
    {
        ERRMSG("serno unmatch. expect %u, actual:%u", flow, GetSerialNo(qza_packet));
        return RTN_WARN;
    }
    return RTN_SUCCESS;
}

int CQzaParser::CheckPacketAtSpp(const void *qza_packet, int len)throw()
{
    if(unlikely(!qza_packet))
    {
        ERRMSG("argment is null.");
        return RTN_ERROR;
    }
    if(unlikely(len < (int)sizeof(QZAHEAD) ))
    {
        TRACE("buf size %d less than qzone protocol size %d.continue recieve...", len, sizeof(QzoneProtocol));
        return 0;
    }
    int _packet_len = GetLen(qza_packet);
    if(unlikely(_packet_len < (int)sizeof(QZAHEAD)))
    {
        ERRMSG("packet-len field invalid. packet-len:%d buf-len:%d", _packet_len, len);
        return RTN_ERROR;
    }
    if(unlikely(_packet_len > len))
    {
        TRACE("buf size %d less than packet size %d.continue recieve...", len, _packet_len);
        return 0;
    }
    return _packet_len;
}
