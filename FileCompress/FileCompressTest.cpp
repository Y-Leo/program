#include "FileCompressHuff.h"
#include "LZ77.h"

int main()
{
    LZ77 lz;
    lz.CompressFile("1.txt");
    FileCompressHuff fc;
    fc.CompressFile("2.lzp");

    fc.UNCompressFile("3.lzp");
    lz.UNCompressFile("4.txt");
    return 0;
}
