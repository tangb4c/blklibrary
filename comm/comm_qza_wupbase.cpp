#include "comm_qza_wupbase.h"
using namespace comm;
const char* CWupQzaBase::SetHeadWithRef(const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int retcode)
{
   if(unlikely(!buf))
	  THROW_CODE_EX(EBadParams, "buf is NULL");
   if(unlikely(bufsize < (int)m_protocol.GetProtocolStructLen()))
	  THROW_CODE_EX(EBufSize, "bufsize %d not enough for qzhead", bufsize);

   if(ref_pkg)
   {
	  int _len = m_protocol.GetHeadLen(buf);
	  if(bufsize < _len)
		 THROW_CODE_EX(EBufSize, "bufsize %d not enough for ref-qzhead %d", bufsize, _len);
	  memcpy(buf, ref_pkg, _len);
	  m_protocol.SetPacketLen(buf, _len);
   }
   else
   {
	  buf->Clear();
	  m_protocol.SetPacketLen(buf, m_protocol.GetProtocolStructLen());
   }
   buf->SetRspCode(retcode);
   return (const char *)buf;
}

