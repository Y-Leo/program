#include "LZHashTable.h"
#include <string.h>

const USH HASH_BITS = 15;
const USH HASH_SIZE = (1 << HASH_BITS);
const USH HASH_MASK = HASH_SIZE - 1;

LZHashTable::LZHashTable(USH size)
    :_prev(new USH[size * 2])
     ,_head(_prev + size)
{
    memset(_prev, 0, size * 2 * sizeof(USH));
}

LZHashTable::~LZHashTable()
{
    delete[] _prev;
    _prev = nullptr;
}

void LZHashTable::Insert(USH& matchHead, UCH ch, USH pos, USH& hashAddr)
{
    HashFunc(hashAddr,ch);

    //找当前三个字符在查找缓冲区中的最近一个，即匹配链的头
    matchHead = _head[hashAddr];
    //pos 可能会超过 32k ,&MASK 的目的就是为了不越界
    _prev[pos & HASH_MASK] = _head[hashAddr];
    _head[hashAddr] = pos;
}

//abcdegh字符串
//hashAddr1:abc
//hashAddr2:bcd

//hashAddr:前一次计算出的哈希地址 abc
//本次需要计算bcd哈希地址
//ch:本次匹配三个字符中的最后一个
//本次哈希地址是在上一次哈希地址的基础上算出来的
void LZHashTable::HashFunc(USH& hashAddr, UCH ch)
{
    hashAddr = (((hashAddr) << H_SHIFT()) ^ (ch)) & HASH_MASK;
}

USH LZHashTable::H_SHIFT()
{
    return (HASH_BITS + MIN_MATCH - 1) / MIN_MATCH;
}

USH LZHashTable::GetNext(USH matchHead)
{
    return _prev[matchHead & HASH_MASK];
}

void LZHashTable::Update()
{
    for(USH i = 0; i < WSIZE; ++i)
    {
        //先更新head
        if(_head[i] >= WSIZE)
            _head[i] -= WSIZE;
        else
            _head[i] = 0;

        //更新prev
        if(_prev[i] >= WSIZE)
            _prev[i] -= WSIZE;
        else
            _prev[i] = 0;
    }
}
