//comm pack with rspcode -----------
inline const char* CWupQzaBase::CommPack(const int&, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode)
{
   return SetHeadWithRef(ref_pkg, buf, bufsize, rspcode);
}
inline const char* CWupQzaBase::CommPack(const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode)
{
   return SetHeadWithRef(ref_pkg, buf, bufsize, rspcode);
}
template<typename T>
inline const char* CWupQzaBase::CommPack(T& obj, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode)
{
   SetHeadWithRef(ref_pkg, buf, bufsize, rspcode);
   return CommPack(obj, buf, bufsize);
}
//end comm pack with rspcode -----------
template<typename T>
inline const char* CWupQzaBase::CommPack(T& obj, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize)
{
   return CommPack(obj, ref_pkg, buf, bufsize, 0);
}
template<typename T>
inline const char* CWupQzaBase::CommPack(T& content, QZAHEAD *buf, int bufsize)
{
   int _len = PackJce(content, m_protocol.GetBody(buf), bufsize - m_protocol.GetHeadLen(buf));
   m_protocol.SetPacketLenWithBodyLen(buf, _len);
   return (const char *)buf;
}
//unpack ---------------
template<typename T>
void CWupQzaBase::CommUnpack(T& out_result, QZAHEAD *pkg)
{
   if(pkg == NULL)
	  THROW_CODE_EX(EBadParams, "pkg is null, bad params");
   int _body_len = m_protocol.GetBodyLen(pkg);
   if(_body_len <= 0)
	  THROW_CODE_EX(ENoData, "body len %d, empty data.", _body_len);
   UnpackJce(out_result, m_protocol.GetBody(pkg), _body_len);
}
template<>
inline void CWupQzaBase::CommUnpack(NullObj&, QZAHEAD *)
{
}
