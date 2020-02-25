#pragma once

#include <string>
#include <vector>
#include "HuffmanTree.hpp"

struct CharInfo
{
   unsigned char _ch;  //具体的字符
   size_t _count;  //字符出现的次数
   std::string _strCode;  //字符编码

   CharInfo(size_t count = 0)
       :_count(count)
   {}
   
   CharInfo operator+(const CharInfo& c)const
   {
       return CharInfo(_count + c._count);
   }

   bool operator>(const CharInfo& c)const
   {
       return _count > c._count;
   }

   bool operator==(const CharInfo& c)const
   {
       return _count == c._count;
   }
};

class FileCompressHuff
{
public:
    //构造
    FileCompressHuff();
    //压缩
    void CompressFile(const std::string& path);
    //解压缩
    void UNCompressFile(const std::string& path);

private:
    //压缩信息
    void WriteHead(FILE* fOut,const std::string& filePostFix);
    std::string GetFilePostFix(const std::string& fileName);
    void ReadLine(FILE* fIn,std::string& strInfo);
    void GenerateHuffManCode(HuffManTreeNode<CharInfo>* pRoot);
private:
    std::vector<CharInfo> _fileInfo;
};

