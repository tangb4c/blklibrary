/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_qza_wupbase.h
 * 
 * Description:
 * 
 * --------------------
 * 2011-6-21, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_QZA_WUPBASE_H__062111__
#define __COMM_QZA_WUPBASE_H__062111__
#include "comm_base.h"
#include "comm_jcepacker.h"
#include "qza_protocol_parser.h"
namespace comm
{
class CWupQzaBase : public CCommBase
{
public:
   /// <summary>
   /// pack obj to buf with QZAHEAD
   /// Initialization of QZAHEAD(buf) should be happened before this function call.
   /// </summary>
   /// <param name="T"></param>
   /// <param name="obj"></param>
   /// <param name="buf"></param>
   /// <param name="bufsize"></param>
   /// <returns></returns>
   template<typename T>
   const char* CommPack(T& obj, QZAHEAD *buf, int bufsize);
   /// <summary>
   /// pack obj and rspcode into buf with reference pkg(ref_pkg)
   /// </summary>
   /// <param name="T"></param>
   /// <param name="obj"></param>
   /// <param name="ref_pkg">refer package</param>
   /// <param name="buf"></param>
   /// <param name="bufsize"></param>
   /// <param name="rspcode">rspcode will be writted in qzahead._rsp_code</param>
   /// <returns></returns>
   template<typename T>
   const char* CommPack(T& obj, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize);
   template<typename T>
   const char* CommPack(T& obj, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode);
   const char* CommPack(const int&, const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode);
   const char* CommPack(const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode);
   /// <summary>
   /// unpack pkg to out_result
   /// </summary>
   /// <param name="T"></param>
   /// <param name="out_result"></param>
   /// <param name="pkg"></param>
   /// <returns></returns>
   template<typename T>
   void CommUnpack(T& out_result, QZAHEAD *pkg);
   void CommUnpack(int, QZAHEAD *) { }
protected:
   const char* SetHeadWithRef(const QZAHEAD *ref_pkg, QZAHEAD *buf, int bufsize, int rspcode);
protected:
   CQzaParser m_protocol;
};
#include "hpp/comm_qza_wupbase.hpp"
}
#endif /* __COMM_QZA_WUPBASE_H__062111__ */
