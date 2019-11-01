/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 *
 * comm_errorcode.h
 *
 * Description:
 *
 * --------------------
 * 2011-6-24, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_ERRORCODE_H__062411__
#define __COMM_ERRORCODE_H__062411__

namespace comm
{
//ERROR code以-17000开头的，即第14位（从0位起）为0,则表示此错误可重试解决
//以-16000开头，则第14位为1, 则表示此错误无法重试解决
enum EError
{
   EL5Route = -40,   //l5返回失败
   ENetwork = -30,   //网络问题、超时

   /*可重试解决的错误*/
   ERCache = -17000, //cache读取时出错
   EWCache = -17001, //cache写入时出错
   EDiffFlow = -17002,  //序列号
   //EL5Route = -17003,   //l5返回失败
   //ENetwork = -17004,   //网络问题、超时

   /*业务*/
   EQQRelation = -17500, //关系链
   EOldVisitor = -17501, //旧访客
   EProfile = -17502,  //个人档
   EOidb = -17503,  //oidb
   EComCount = -17504,   //统一计数
   ECommentSystem = -17505, //评论系统
   ELike = -17506,      //like系统
   EDas = -17507,       //DAS统一存储

   /*重试不可解决的错误*/
   EBadParams = -16000,    //参数错误
   EWupParse = -16001,     //wup/jce解析出错
   ESystemFailed = -16002, //系统出错
   EInitFailed = -16003,   //初始化时失败
   EBufSize = -16004,      // bufsize不足
   EQzoneIni = -16005,
   EUndefineCmd = -16006,
   EBadData = -16007,      //  数据有脏数据，不能解析等
   ENoData = -16008,       //  数据不存在
   EProtocolChk = -16009,  //  协议头检查失败
   ERspcode = -16010,      //后端依赖的服务返回了错误
   EBill = -16011,         //流水错误
   EDeny = -16012,         //由于策略原因，拒绝请求.
   EBadUin = -16013,       //uin不合法
   EExceedLimit = -16014,  //参数超过限制。比如批量数、有效值范围
   ENoLogin = -16015,      //没有登录
   ENoPermission = -16016,  //没有权限操作
   EFrequency = -16017,     //频率限制
   EMalicious = -16018,     //被安全打击的恶意请求
   EDirtyWords = -16020,   //脏话、敏感词
   ENotMember = -16021,    //(访问者)非成员
   ETargetNoMember = -16022, //目标非成员（被访问者非成员）
   ERepeatData = -16023,      //重复内容
};

const int ERETRY_FLAG = 0x1 << 14;
// is able to recover by retrying
//errorcode defined at enum EError
inline bool IsRetryRetcode(int errorcode)
{
   return !(errorcode & (ERETRY_FLAG));
}
}

#endif /* __COMM_ERRORCODE_H__062411__ */
