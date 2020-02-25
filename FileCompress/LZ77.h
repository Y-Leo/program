#pragma once

#include "LZHashTable.h"
#include <string>

class LZ77
{
public:
    LZ77();
    ~LZ77();
    void CompressFile(const std::string& strFilePath);
    void UNCompressFile(const std::string& strFilePath);

private:
    USH LongestMatch(USH matchHead, USH& MatchDist, USH start);
    void WriteFlage(FILE* fOUT, UCH& chFlag, UCH& bitCount, bool isLen);
private:
    UCH* _pWin;  //用来保存待压缩数据的缓冲区
    LZHashTable _ht;
};
