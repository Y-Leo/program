#include "FileCompressHuff.h"

int main()
{
    FileCompressHuff fc;
    fc.CompressFile("1.txt");
    fc.UNCompressFile("2.txt");
    return 0;
}
