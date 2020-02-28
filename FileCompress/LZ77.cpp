#include "LZ77.h"
#include <iostream>
#include <string.h>
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
    size_t lookAhead = fread(_pWin, 1, 2 * WSIZE, fIn);
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
            --curMatchLength;  //当前字符已经插入了
            while(curMatchLength)
            {
                start++;
                _ht.Insert(matchHead, _pWin[start +  2], start, hashAddr);
                curMatchLength--;
            }
            ++start;  //在循环中,start 少加了一次
        }
    }

    //检测先行缓冲区中剩余字符个数
    if(lookAhead <= MIN_LOOKAHEAD)
        FillWindow(fIn, lookAhead, start);

    //标记位数如果不够8个比特位：
    if(bitCount > 0 && bitCount < 8)
    {
        chFlag <<= (8 - bitCount);
        fputc(chFlag, fOutF);
    }

    fclose(fOutF);

    //将压缩文件和标记文件合为一个文件
    MergeFile(fOUT,fileSize);

    fclose(fIn);
    fclose(fOUT);

    //TODO：删除原来的标记信息的临时文件
}

void LZ77::FillWindow(FILE* fIn, size_t& lookAhead, USH& start)
{
    //start 压缩已经进行到右窗，先行缓冲区剩余数据不够MIN_LOOKAHEAD
    if(start >= WSIZE)
    {
        //1、将右窗中的数据搬移到左侧
        memcpy(_pWin, _pWin + WSIZE, WSIZE);
        memset(_pWin + WSIZE, 0, WSIZE);
        start -= WSIZE;

        //2、更新哈希表
        _ht.Update();

        //3、向右窗中补充WSIZE个的待压缩数据
        if(!feof(fIn))
            lookAhead += fread(_pWin + WSIZE, 1, WSIZE, fIn);
    }
}

void LZ77::MergeFile(FILE* fOut, ULL fileSize)
{
    //将压缩文件和标记文件合成一个
    //1、读取标记信息中的文件内容，然后将结果写入到压缩文件中
    FILE* fInF = fopen("3.txt", "rb");
    size_t flagSize = 0;
    UCH* pReadbuff = new UCH[1024];
    while(true)
    {
        size_t rdSize = fread(pReadbuff, 1, 1024, fInF);
        if(0 == rdSize)
            break;

        fwrite(pReadbuff, 1, rdSize, fOut);
        flagSize += rdSize;
    }

    fwrite(&flagSize, sizeof(flagSize), 1, fOut);
    fwrite(&fileSize, sizeof(fileSize), 1, fOut);
    fclose(fInF);
    delete[] pReadbuff;
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

//解压缩
void LZ77::UNCompressFile(const std::string& strFilePath)
{
    //打开压缩文件
    FILE* fInD = fopen(strFilePath.c_str(),"rb");
    if(nullptr == fInD)
    {
        cout << "压缩文件打开失败" << endl;
        return;
    }

    //操作标记数据的文件指针
    FILE* fInF = fopen(strFilePath.c_str(),"rb");
    if(nullptr == fInF)
    {
        cout << "压缩文件打开失败" << endl;
        return;
    }

    //获取原文件的大小
    ULL fileSize = 0;
    fseek(fInF, 0 - sizeof(fileSize), SEEK_END);
    fread(&fileSize, sizeof(fileSize), 1, fInF);

    //获取标记信息的大小
    size_t flagSize = 0;
    fseek(fInF, 0 - sizeof(fileSize) - sizeof(flagSize), SEEK_END);
    fread(&flagSize, sizeof(flagSize), 1, fInF);

    //将读取标记信息的文件指针移动到保存标记数据的起始位置
    fseek(fInF, 0 - sizeof(flagSize) - sizeof(fileSize) - flagSize, SEEK_END);

    //开始解压缩
    //打开一个文件保存解压缩信息
    FILE* fOut = fopen("5.txt","wb");
    assert(fOut);

    FILE* fR = fopen("5.txt","rb");

    UCH bitCount = 0;
    UCH chFlag = 0;
    ULL encodeCount = 0;
    while(encodeCount < fileSize)
    {
        //读取标记
        if(0 == bitCount)
        {
            chFlag = fgetc(fInF);
            bitCount = 8;
        }

        if(chFlag & 0X80)
        {
            //为1：读取<距离，长度>对
            USH matchLen = fgetc(fInD) + 3;
            USH matchDist = 0;
            fread(&matchDist, sizeof(matchDist), 1, fInD);

            //清空缓冲区：系统会将缓冲区中的数据写入到文件中
            fflush(fOut);

            //更新已经解码的字节数大小
            encodeCount += matchLen;

            //fR:读取前文中匹配的内容
            UCH ch;
            fseek(fR, 0 - matchDist, SEEK_END);
            while(matchLen)
            {
                ch = fgetc(fR);
                fputc(ch, fOut);
                matchLen--;

                //在还原<距离，长度>对时，一定要清空缓冲区，否则可能导致还原出错
                fflush(fOut);
            }
        }
        else
        {
            //为0: 读取的是原字符
            UCH ch = fgetc(fInD);
            fputc(ch, fOut);
            encodeCount += 1;
        }

        chFlag <<= 1;
        bitCount--;
    }

    fclose(fInD);
    fclose(fInF);
    fclose(fOut);
    fclose(fR);
}
