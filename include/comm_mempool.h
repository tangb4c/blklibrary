/******************************************************************************
 * Copyright:      Shenzhen Tecent Co.Ltd.
 *
 * comm_mempool.h
 *
 * Description:
 *
 * --------------------
 * 2013/7/23, blaketang create
 * --------------------
 ******************************************************************************/
#ifndef __COMM_MEMPOOL_H__072313__
#define __COMM_MEMPOOL_H__072313__

namespace comm
{
template<typename T>
class CMempool
{
public:
   static void ReleasePool();
   static void* operator new(size_t size);
   static void operator delete(void* p);
private:
   struct _Node
   {
      _Node* next;
   };
   static _Node *__free;
};





template<typename T>
typename CMempool<T>::_Node* CMempool<T>::__free = NULL;

template<typename T>
void* CMempool<T>::operator new(size_t size)
{
   if(__free)
   {
      _Node* _r = __free;
      __free = __free->next;
      return _r;
   }
   return new char[size];
}
template<typename T>
void CMempool<T>::operator delete(void* p)
{
   _Node* _obj = reinterpret_cast<_Node*>(p);
   _obj->next = __free;
   __free = _obj;
}
template<typename T>
void CMempool<T>::ReleasePool()
{
   void* _obj = NULL;
   while(__free)
   {
      _obj = __free;
      __free = __free->next;
      delete[] _obj;
   }
}
}

#endif /* __COMM_MEMPOOL_H__072313__ */
