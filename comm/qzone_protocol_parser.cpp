#include "qzone_protocol_parser.h"
#include "comm_macro.h"
using namespace comm;
int CQzoneParser::CheckPacket(const void *qzone_packet, int len)throw()
{
    if(unlikely(!qzone_packet))
    {
        ERRMSG("argment is null.");
        return RTN_ERROR;
    }
    if(unlikely(len < (int)GetProtocolStructLen()))
    {
        ERRMSG("packet-len field invalid. buf-len:%d less than:%d", len, GetProtocolStructLen());
        return RTN_ERROR;
    }
    char* _buf = (char*)qzone_packet;
    if(unlikely(*_buf != QzoneProtocolSOH || _buf[len - 1] != QzoneProtocolEOT))
    {
        ERRMSG("beginning or ending flag unexpected. begin:0x%x, end:0x%x, expected-beginning:0x%x, expected-ending:0x%x", *_buf, _buf[len - 1], QzoneProtocolSOH, QzoneProtocolEOT);
        return RTN_ERROR;
    }
    if(unlikely(GetLen(qzone_packet) != (unsigned)len))
    {
        ERRMSG("packet length [%d] unmatch with buf length[%d].", GetLen(qzone_packet), len);
        return RTN_ERROR;
    }
    return RTN_SUCCESS;
}
int CQzoneParser::CheckPacket(const void *qzone_packet, int len, unsigned flow)throw()
{
    if(unlikely(CheckPacket(qzone_packet, len)))return RTN_ERROR;
    if(unlikely(flow && GetSerialNo(qzone_packet) != flow))
    {
        ERRMSG("serno unmatch. expect %u, actual:%u", flow, GetSerialNo(qzone_packet));
        return RTN_WARN;
    }
    return RTN_SUCCESS;
}

int CQzoneParser::CheckPacketAtSpp(const void *qzone_packet, int len)throw()
{
    if(unlikely(!qzone_packet))
    {
        ERRMSG("argment is null.");
        return RTN_ERROR;
    }
    char* buf = (char*)qzone_packet;
    if(unlikely(*buf != QzoneProtocolSOH))
    {
        ERRMSG("beginning flag unexpected. begin:0x%x, expected-beginning:0x%x, buf-len:%d", *buf, QzoneProtocolSOH, len);
        return RTN_ERROR;
    }
    if(unlikely(len < (int)sizeof(QzoneProtocol) ))
    {
        TRACE("buf size %d less than qzone protocol size %d.continue recieve...", len, sizeof(QzoneProtocol));
        return 0;
    }
    int _packet_len = (int)GetLen(qzone_packet);
    if(unlikely(_packet_len < (int)sizeof(QzoneProtocol)))
    {
        ERRMSG("packet-len field invalid. packet-len:%d buf-len:%d", _packet_len, len);
        return RTN_ERROR;
    }
    if(unlikely(_packet_len > len))
    {
        TRACE("buf size %d less than packet size %d.continue recieve...", len, GetLen(qzone_packet));
        return 0;
    }
    if(unlikely(buf[_packet_len - 1] != QzoneProtocolEOT))
    {
        ERRMSG("end flag unexpected. end:0x%x, expected-end:0x%x, buf-len:%d", buf[_packet_len - 1], QzoneProtocolEOT, len);
        return RTN_ERROR;
    }
    return _packet_len;
}
