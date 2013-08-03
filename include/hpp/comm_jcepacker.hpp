namespace comm
{
inline size_t PackJce(const NullObj&, char *, int)
{
   return 0;
}
template<typename T>
size_t PackJce(const T& c, char *buf, int max_size)
{
   if(buf == NULL) THROW_CODE_EX(EBadParams, "buf is null, bad params");
   T& content = const_cast<T&>(c);
   try
   {
	  taf::JceOutputStream<taf::BufferWriterBuff> os;
	  os.setBuffer(buf, max_size);
	  content.writeTo(os);
	  return os.getLength();
   }
   catch (taf::JceNotEnoughBuff& e)
   {
	  THROW_CODE_EX(EWupParse, "%s JceNotEnoughBuff. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
   catch (taf::JceDecodeInvalidValue& e)
   {
	  THROW_CODE_EX(EWupParse, "%s JceDecodeInvalidValue. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
   catch (exception& e)
   {
	  THROW_CODE_EX(EWupParse, "%s Other Error Happend. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
}

template<typename T>
size_t PackJce(const T& c, string& result)
{
   const int _ALLOCSIZES[] = { 8 * 1024, 32 * 1024, 1024 * 1024, 10 * 1024 * 1024 };
   T& content = const_cast<T&>(c);
   int _bufsize = 0;
   for (const int* it = _ALLOCSIZES; it != utility::EndOf(_ALLOCSIZES);)
   {
	  _bufsize = *it;
	  result.resize(_bufsize);
	  try
	  {
		 taf::JceOutputStream<taf::BufferWriterBuff> os;
		 os.setBuffer((char *)result.data(), _bufsize);
		 content.writeTo(os);
		 result.resize(os.getLength());
		 return os.getLength();
	  }
	  catch (taf::JceNotEnoughBuff& e)
	  {
		 if(++it == utility::EndOf(_ALLOCSIZES))
			THROW_CODE_EX(EWupParse, "%s JceNotEnoughBuff. bufsize:%d msg:%s", content.className().c_str(), _bufsize, e.what());
	  }
	  catch (exception& e)
	  {
		 THROW_CODE_EX(EWupParse, "%s Other Error Happend. bufsize:%d msg:%s", content.className().c_str(), _bufsize, e.what());
	  }
   }
}
inline size_t UnpackJce(NullObj&, const char *, int)
{
   return 0;
}
template<typename T>
size_t UnpackJce(T& content, const char *buf, int max_size)
{
   if(buf == NULL) THROW_CODE_EX(EBadParams, "buf is null, bad params");;
   try
   {
	  taf::JceInputStream<taf::MapBufferReader> _is;
	  _is.setBuffer(buf, max_size);
	  content.readFrom(_is);
	  return _is.left();
   }
   catch (taf::JceDecodeMismatch& e)
   {
	  THROW_CODE_EX(EWupParse, "%s JceDecodeMismatch. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
   catch (taf::JceDecodeInvalidValue& e)
   {
	  THROW_CODE_EX(EWupParse, "%s JceDecodeInvalidValue. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
   catch (taf::JceDecodeRequireNotExist& e)
   {
	  THROW_CODE_EX(EWupParse, "%s JceDecodeRequireNotExist. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
   catch (exception& e)
   {
	  THROW_CODE_EX(EWupParse, "%s Other Error Happend. bufsize:%d msg:%s", content.className().c_str(), max_size, e.what());
   }
}
}
