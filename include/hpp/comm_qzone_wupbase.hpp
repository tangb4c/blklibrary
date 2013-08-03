//comm pack with retcode -----------
inline const char* CWupQzoneBase::CommPack(const int&, const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode)
{
   return SetHeadWithRef(ref_pkg, buf, bufsize, retcode);
}
inline const char* CWupQzoneBase::CommPack(const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode)
{
   return SetHeadWithRef(ref_pkg, buf, bufsize, retcode);
}
template<typename T>
inline const char* CWupQzoneBase::CommPack(T& obj, const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode)
{
   SetHeadWithRef(ref_pkg, buf, bufsize, retcode);
   return CommPack(obj, buf, bufsize);
}
template<typename T>
inline const char* CWupQzoneBase::CommPack(T& content, QzoneProtocol *buf, int bufsize)
{
   int _len = PackJce(content, m_protocol.GetBody(buf), bufsize - m_protocol.GetHeadLen(buf));
   m_protocol.SetPacketLenWithBodyLen(buf, _len);
   return (const char *)buf;
}
//end comm pack with retcode -----------
//unpack ---------------
template<typename T>
void CWupQzoneBase::CommUnpack(T& out_result, QzoneProtocol *pkg)
{
   if(pkg == NULL)
	  THROW_CODE_EX(EBadParams, "pkg is null, bad params");
   int _body_len = m_protocol.GetBodyLen(pkg);
   if(_body_len <= 0)
	  THROW_CODE_EX(ENoData, "body len %d, empty data.", _body_len);
   UnpackJce(out_result, m_protocol.GetBody(pkg), _body_len);
}
template<>
inline void CWupQzoneBase::CommUnpack(NullObj&, QzoneProtocol *)
{
}
