/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 * 
 * comm_jcepacker.h
 * 
 * Description:
 * 
 * --------------------
 * 2012-7-17, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_JCEPACKER_H__071712__
#define __COMM_JCEPACKER_H__071712__
#include "comm_include.h"
#include "jce/Jce.h"
namespace comm
{
/// <summary>
/// pack content to buf
/// </summary>
/// <param name="T"></param>
/// <param name="content"></param>
/// <param name="buf"></param>
/// <param name="max_size"></param>
/// <returns>used buf size</returns>
template<typename T>
size_t PackJce(const T& content, char *buf, int max_size);
template<typename T>
size_t PackJce(const T& content, string& result);
/// <summary>
/// unpack buf with type of T
/// </summary>
/// <param name="T"></param>
/// <param name="content"></param>
/// <param name="buf"></param>
/// <param name="max_size"></param>
/// <returns>left buf length</returns>
template<typename T>
size_t UnpackJce(T& content, const char *buf, int max_size);
};
#include "hpp/comm_jcepacker.hpp"
#endif /* __COMM_JCEPACKER_H__071712__ */
