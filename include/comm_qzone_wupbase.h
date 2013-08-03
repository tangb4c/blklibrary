/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 *
 * comm_qzone_wupbase.h
 *
 * Description:
 *
 * --------------------
 * 2012/12/20, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_QZONE_WUPBASE_H__122012__
#define __COMM_QZONE_WUPBASE_H__122012__

#include "comm_base.h"
#include "comm_jcepacker.h"
#include "qzone_protocol_parser.h"
namespace comm
{
class CWupQzoneBase : public CCommBase
{
public:
   /// <summary>
   /// pack obj to buf with QzoneProtocol
   /// </summary>
   /// <param name="T"></param>
   /// <param name="obj"></param>
   /// <param name="buf"></param>
   /// <param name="bufsize"></param>
   /// <returns></returns>
   template<typename T>
   const char* CommPack(T& obj, QzoneProtocol *buf, int bufsize);
   /// <summary>
   /// pack obj to buf with ref_pkg
   /// </summary>
   /// <param name="T"></param>
   /// <param name="obj"></param>
   /// <param name="ref_pkg"></param>
   /// <param name="buf"></param>
   /// <param name="bufsize"></param>
   /// <param name="retcode"></param>
   /// <returns></returns>
   template<typename T>
   const char* CommPack(T& obj, const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode);
   const char* CommPack(const int&, const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode);
   const char* CommPack(const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode);
   /// <summary>
   /// unpack pkg to out_result
   /// </summary>
   /// <param name="T"></param>
   /// <param name="out_result"></param>
   /// <param name="pkg"></param>
   /// <returns></returns>
   template<typename T>
   void CommUnpack(T& out_result, QzoneProtocol *pkg);
   void CommUnpack(int, QzoneProtocol *) { }
protected:
   const char* SetHeadWithRef(const QzoneProtocol *ref_pkg, QzoneProtocol *buf, int bufsize, int retcode);
protected:
   CQzoneParser m_protocol;
};
#include "hpp/comm_qzone_wupbase.hpp"
}

#endif /* __COMM_QZONE_WUPBASE_H__122012__ */
