#include "FileCompressHuff.h"

#include <assert.h>
#include <algorithm>
#include <stdlib.h>

FileCompressHuff::FileCompressHuff()
{
    _fileInfo.resize(256);
    for(int i = 0; i < 256; ++i)
    {
        _fileInfo[i]._ch = i;  //字符和i的值对应，直接用i初始化
        _fileInfo[i]._count = 0;
    }
}

void FileCompressHuff::CompressFile(const std::string& path)
{
    //1、统计源文件中每个字符出现的次数
    FILE* fIn = fopen(path.c_str(),"rb");
    if(nullptr == fIn)
    {
        assert(false);
        return;
    }

    unsigned char* pReadBuff = new unsigned char[1024];
    int rdSize = 0;
    while(true)
    {
        rdSize = fread(pReadBuff,1,1024,fIn);
        //读到末尾，读不出来则退出
        if(0 == rdSize)
            break;

        for(int i = 0; i < rdSize; ++i)
        {
            _fileInfo[pReadBuff[i]]._count++;
        }
    }

    //2、以字符出现的次数为权值，创建HuffManTree
    HuffManTree<CharInfo> t(_fileInfo,CharInfo(0));

    //3、获取每个字符的编码
    GenerateHuffManCode(t.GetRoot());

    //4、用获取到的字符编码重新改写源文件
    FILE* fOut = fopen("2.txt","wb");
    if(nullptr == fOut)
    {
        assert(false);
        return;
    }

    //把文件指针放在开头，因为之前统计字符个数时已经放在文件末尾了，不然读不出来数据
    WriteHead(fOut,path);
    fseek(fIn,0,SEEK_SET);
    char ch = 0;
    int bitcount = 0;
    while(true)
    {
        rdSize = fread(pReadBuff,1,1024,fIn);
        if(0 == rdSize)
            break;

        //根据字节的编码，对读取到的内容进行重写
        for(size_t i = 0; i < rdSize; ++i)
        {
            std::string strCode =  _fileInfo[pReadBuff[i]]._strCode;
            //A:"110"  B:"101"
            for(size_t j = 0; j < strCode.size();++j)
            {
                ch <<= 1;
                if('1' == strCode[j])
                    ch |= 1;

                bitcount++;
                if(8 == bitcount)
                {
                    fputc(ch,fOut);
                    bitcount = 0;
                    ch = 0;
                }
            }
        }
    }

    //最后一次 ch 中可能不够8个bit 位
    if(bitcount < 8)
    {
        ch <<= (8 - bitcount);
        fputc(ch,fOut);
    }

    delete[] pReadBuff;
    fclose(fIn);
    fclose(fOut);
}

//解压缩过程
void FileCompressHuff::UNCompressFile(const std::string& path)
{
    FILE* fIn = fopen(path.c_str(),"rb");
    if(nullptr == fIn)
    {
        printf("false\n");
        return;
    }

    //文件后缀
    std::string strFilePostFix;
    ReadLine(fIn,strFilePostFix);

    //字符信息总行数
    std::string strCount;
    ReadLine(fIn,strCount);
    int lineCount = atoi(strCount.c_str());

    //字符信息
    for(int i = 0; i < lineCount; ++i)
    {
        std::string strchCount;
        ReadLine(fIn, strchCount);
        
        //如果读到的是换行符号
        if(strchCount.empty())
        {
            strchCount += '\n';
            ReadLine(fIn,strchCount);
        }

        //A:1
        _fileInfo[(unsigned char)strchCount[0]]._count = atoi(strchCount.c_str() + 2);
    }

    //还原哈夫曼树
    HuffManTree<CharInfo> t;
    t.CreateHuffManTree(_fileInfo,CharInfo(0));

    FILE* fOut = fopen("3.txt","wb");
    assert(fOut);

    //解压缩
    char* pReadBuff = new char[1024];
    char ch = 0;
    HuffManTreeNode<CharInfo>* pCur = t.GetRoot();
    size_t fileSize = pCur->_weight._count;
    size_t unCount = 0;
    while(true)
    {
        size_t rdSize = fread(pReadBuff,1,1024,fIn);
        if(0 == rdSize)
            break;

        for(size_t i = 0; i < rdSize; ++i)
        {
            ch = pReadBuff[i];
            //只需要将一个字节中的8个bit 位单独处理
            for(int pos = 0; pos < 8; ++pos)
            {
                if(ch & 0X80)
                    pCur = pCur->_pRight;
                else
                    pCur = pCur->_pLeft;

                ch <<= 1;
                if(nullptr == pCur->_pLeft && nullptr == pCur->_pRight)
                {
                    unCount++;
                    fputc(pCur->_weight._ch,fOut);
                    if(unCount == fileSize)
                        break;
                    pCur = t.GetRoot();
                }
            }
        }
    }
    
    delete[] pReadBuff;
    fclose(fIn);
    fclose(fOut);
}

//读一行
void FileCompressHuff::ReadLine(FILE* fIn,std::string& strInfo)
{
    assert(fIn);

    while(!feof(fIn))//没到文件末尾就一直读
    {
        char ch = fgetc(fIn);
        if(ch == '\n')
            break;

        strInfo += ch;
    }

}

//压缩文件头部信息（方便解压缩）
void FileCompressHuff::WriteHead(FILE* fOut,const std::string& fileName)
{
    assert(fOut);

    //写文件的后缀
    std::string strHead;
    strHead += GetFilePostFix(fileName);
    strHead += '\n';

    //写压缩信息总行数
    size_t lineCount = 0;
    std::string strChCount;
    char szValue[32] = {0};
    for(int i = 0;i < 256; ++i)
    {
        CharInfo& charInfo = _fileInfo[i];
        if(_fileInfo[i]._count)
        {
            lineCount++;
            strChCount += charInfo._ch;
            strChCount += ':';
           // _itoa(charInfo._count,szValue,10);
            sprintf(szValue,"%lu",charInfo._count);
            strChCount += szValue;
            strChCount += '\n';
        }
    }

    //_itoa(lineCount, szValue , 10);
    sprintf(szValue,"%lu",lineCount);
    strHead += szValue;
    strHead += '\n';

    strHead += strChCount;

    fwrite(strHead.c_str(), 1, strHead.size(), fOut);
}

//获取文件后缀
//2.txt
//F:\123\2.txt
std::string FileCompressHuff::GetFilePostFix(const std::string& fileName)
{
  return  fileName.substr(fileName.rfind('.'));
}

void FileCompressHuff::GenerateHuffManCode(HuffManTreeNode<CharInfo>* pRoot)
{
    if(nullptr == pRoot)
        return ;

    GenerateHuffManCode(pRoot->_pLeft);
    GenerateHuffManCode(pRoot->_pRight);

    if(nullptr == pRoot->_pLeft && nullptr == pRoot->_pRight)
    {
        std::string& strCode = _fileInfo[pRoot->_weight._ch]._strCode;
        HuffManTreeNode<CharInfo>* pCur = pRoot;
        HuffManTreeNode<CharInfo>* pParent = pCur->_pParent;

        while(pParent)
        {
            if(pCur == pParent->_pLeft)
            { 
                strCode += '0';
            }
            else
            {
                strCode += '1';
            }

            pCur = pParent;
            pParent = pCur->_pParent;
        }

        reverse(strCode.begin(),strCode.end());
    }
}
