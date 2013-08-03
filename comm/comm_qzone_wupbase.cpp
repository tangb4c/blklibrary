#include "comm_qzone_wupbase.h"
using namespace comm;
const char* CWupQzoneBase::SetHeadWithRef(const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode)
{
	if(!buf)
		THROW_CODE_EX(EBadParams, "buf is NULL");
	if(bufsize < (int)m_protocol.GetProtocolStructLen())
		THROW_CODE_EX(EBufSize, "bufsize %d not enough for qzhead", bufsize);

	if(ref_pkg)
	{
		int _len = m_protocol.GetHeadLen(buf);
		if(unlikely(bufsize < _len))
			THROW_CODE_EX(EBufSize, "bufsize %d not enough for ref-qzhead %d", bufsize, _len);
		memcpy(buf, ref_pkg, _len);
		m_protocol.SetPacketLen(buf, _len);
	}
	else
	{
		m_protocol.SetDefault(buf);
		m_protocol.SetPacketLen(buf, m_protocol.GetProtocolStructLen());
	}
	m_protocol.SetRspFlag(buf, retcode, retcode);
	return (const char *)buf;
}

