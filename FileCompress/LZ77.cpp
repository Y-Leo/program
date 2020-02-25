#include "LZ77.h"
#include <iostream>
#include <assert.h>
using namespace std;

const USH MIN_LOOKAHEAD = MAX_MATCH + MIN_MATCH + 1;
const USH MAX_DIST = WSIZE - MIN_LOOKAHEAD;

LZ77::LZ77()
    :_pWin(new UCH[WSIZE * 2])
     ,_ht(WSIZE)
{}

LZ77::~LZ77()
{
    delete[] _pWin;
    _pWin = nullptr;
}

void LZ77::CompressFile(const std::string& strFilePath)
{
    //打开源文件
    FILE* fIn = fopen(strFilePath.c_str(),"rb");
    if(nullptr == fIn)
    {
        cout << "打开文件失败" << endl;
        return;
    }
    
    //获取文件大小
    fseek(fIn, 0, SEEK_END);
    ULL fileSize = ftell(fIn);

    //如果源文件的大小小于MIN_MATCH，则不进行处理
    if(fileSize <= MIN_MATCH)
    {
        cout << "文件太小，不压缩"  << endl;
        return;
    }

    //调整文件指针位置
    fseek(fIn, 0, SEEK_SET);

    //从压缩文件中读取一个缓冲区的数据到窗口中
    USH lookAhead = fread(_pWin, 1, 2 * WSIZE, fIn);
    USH hashAddr = 0;
    
    //abcdefg.....设置起始的hashAddr
    for(USH i = 0; i < MIN_MATCH - 1; ++i)
        _ht.HashFunc(hashAddr, _pWin[i]);

    //压缩
    
    //打开一个文件，用来保存压缩文件
    FILE* fOUT = fopen("2.lzp","wb");
    assert(fOUT);

    USH start = 0;

    //与查找相关的变量
    USH matchHead = 0;
    USH curMatchLength = 0;
    USH curMatchDist = 0;

    //与写标记相关的变量
    UCH chFlag = 0;
    UCH bitCount = 0;
    bool IsLen = false;

    //写标记的文件
    FILE* fOutF = fopen("3.txt","wb");
    assert(fOutF);

    //lookAhead表示先行缓冲区中剩余字节的个数
    while(lookAhead)
    {
        //1、将当前三个字符(start,start+1,start+2)插入到哈希表中，并获取获取匹配链的头
        _ht.Insert(matchHead, _pWin[start + 2], start, hashAddr);

        curMatchLength = 0;
        curMatchDist = 0;

        //2、验证在查找缓冲区中是否找到匹配，如果有匹配则找最长匹配
        if(matchHead)
        {
            //顺着匹配链找最长匹配,最终带出<长度，距离>对
            curMatchLength = LongestMatch(matchHead, curMatchDist, start);
        }

        //3、验证是否找到匹配
        if(curMatchLength < MIN_MATCH)
        {
            //在查找缓冲区中未找到重复字符串
            //将start位置的字符写入到压缩文件中
            fputc(_pWin[start], fOUT);

            //写当前原字符对应的标记
            WriteFlage(fOutF, chFlag, bitCount, false);
            ++start;
            lookAhead--;
        }
        else
        {
            //找到匹配
            //将<长度,距离>对写入到压缩文件中去
            //写长度
            UCH chLen = curMatchLength - 3;
            fputc(chLen, fOUT);

            //写距离
            fwrite(&curMatchDist, sizeof(curMatchDist), 1, fOUT);

            //写当前原字符对应的标记
            WriteFlage(fOutF, chFlag, bitCount, true);

            //更行先行缓冲区中剩余的字节数
            lookAhead -= curMatchLength;

            //将已经匹配的字符串按三个一组插入到哈希表中
            --curMatchDist;  //当前字符已经插入了
            while(curMatchLength)
            {
                start++;
                _ht.Insert(matchHead, _pWin[start +  2], start, hashAddr);
                curMatchLength--;
            }
            ++start;  //在循环中,start 少加了一次
        }
    }
    //标记位数如果不够8个比特位：
    if(bitCount > 0 && bitCount < 8)
    {
        chFlag <<= (8 - bitCount);
        fputc(chFlag, fOutF);
    }
    //将压缩文件和标记文件合成一个
    fclose(fIn);
    fclose(fOUT);
    fclose(fOutF);
}


//chFlag:该字节中的每个比特位是用来区分当前字节是原字符还是长度
//0：原字符
//1：长度

//bitCount：该字节中的多少个比特位已经被设置
//isCharOrLen: 代表该字节是原字符还是长度
void LZ77::WriteFlage(FILE* fOUT, UCH& chFlag, UCH& bitCount, bool isLen)
{
    chFlag <<= 1;
    if(isLen)
        chFlag |= 1;

    bitCount++;
    if(bitCount == 8)
    {
        //将该标记写入压缩文件中
        fputc(chFlag, fOUT);
        chFlag = 0;
        bitCount = 0;
    }
}

//匹配：是在查找缓冲区中进行的，查找缓冲区中可能会找到多个匹配
//输出：需要最长匹配
//注意：可能会遇到环状链---解决：设置最大的匹次数
//      匹配是在MAX_DIST 范围内进行匹配的，太远的距离则不进行匹配

//在找的过程中，需要将每次找到的匹配结果进行比较，保持最长匹配
USH LZ77::LongestMatch(USH matchHead, USH& MatchDist, USH start)
{
    USH curMatchLen = 0; // 一次匹配的长度
    USH maxMatchLen = 0;
    UCH maxMatchCount = 255; // 最大的匹配次数，解决环状链
    USH curMatchStart = 0;    //当前匹配在查找缓冲区中的起始位置

    //在先行缓冲区中查找匹配时，不能太远即不能超过MAX_DIST
    USH limit = start > MAX_DIST ? start - MAX_DIST : 0;

    do
    {
        //匹配范围
        //先行缓冲区
        UCH* pstart = _pWin + start;
        UCH* pend = pstart + MAX_MATCH;

        //查找缓冲区匹配串的起始
        UCH* pMatchStart = _pWin + matchHead;

        curMatchLen = 0;

        //可以进行本次匹配
        while(pstart < pend && *pstart == *pMatchStart)
        {
            curMatchLen++;
            pstart++;
            pMatchStart++;
        }

        //一次匹配结束
        if(curMatchLen > maxMatchLen)
        {
            maxMatchLen = curMatchLen;
            curMatchStart = matchHead;
        }
    }while((matchHead = _ht.GetNext(matchHead)) > limit && maxMatchCount--);

    MatchDist = start - curMatchStart;
    return maxMatchLen;
}
