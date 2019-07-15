#include <QtCore/QCoreApplication>
#include <QDebug>
#include <QString>
#include <QFile>
#include "aes.h"
#include "aesutil.h"
#include "base64cryptutil.h"

void print(unsigned char* value)
{
    int i;
    for(i=0; i<16; i++)
    {
        printf("%s%#x ", value[i]>15 ? "" : "0", value[i]);
    }
    printf("\n");
}

void print(const char *key, unsigned char* value)
{
    int i;
    printf("%s:\n", key);
    for(i=0; i<16; i++)
    {
        printf("%s%#x ", value[i]>15 ? "" : "0", value[i]);
    }
    printf("\n");
}

void print_uchar(void *data, int len)
{
    for(int j=0; j<len; j++)
        printf("%X ", ((unsigned char *)data)[j]);
    printf("\n");
}

/**
 * 直接使用 AES 工具
 */
int test0(int argc, char **argv)
{
    unsigned char input[] =
    {
        0x32, 0x43, 0xf6, 0xa8,
        0x88, 0x5a, 0x30, 0x8d,
        0x31, 0x31, 0x98, 0xa2,
        0xe0, 0x37, 0x07, 0x34
    };
    unsigned char output[16] = { 0 };
    unsigned char key[] =
    {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    AES aes(Bits128,key);

    //-1-
    print("Input", input);

    aes.Cipher(input,output);
    print("After Cipher", output);

    aes.InvCipher(output,input);
    print("After InvCipher", input);

    unsigned char strTemp[32]={0};
    char str[32] = "Hello,My AES Cipher!";
    print_uchar(str, 32);

    aes.Cipher((unsigned char*)str,strTemp);
    aes.Cipher((unsigned char*)&str[16],&strTemp[16]);
    print_uchar(strTemp, 32);

    aes.InvCipher(strTemp,(unsigned char*)str);
    print_uchar(str, 32);

    //-2-
    QFile *file=new QFile("data.tk");
    QFile *file1=new QFile("data1.tk");
    QFile *file2 = new QFile("data2.tk");

    aes.FileCipher(file, file1);
    aes.FileInvCipher(file1, file2);
}

/**
 * 测试 AES Util
 */
void test1()
{
    unsigned char key[] =
    {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };

    AES aes(Bits128,key);

    // ==== 加密 ====
    QString text("test123456789123aaaaaaaaaabbbbbb");
    qDebug() << "QString:" << text;
    int len = strlen((char*)(text.toLatin1().data()));
    int malloc_len = len * sizeof(char) + 1;
    qDebug() << "len:" << len << "malloc_len:" << malloc_len;
    char* string = (char*)malloc(malloc_len);
    memset((void*)string, 0, malloc_len);
    strcpy(string, (char*)(text.toLatin1().data()));
    printf("char:%s\n", string);

    char*  out_string = (char*)malloc(malloc_len);
    memset((void*)out_string, 0, malloc_len);

    char* out_string2 = (char*) malloc(malloc_len);
    memset((void*)out_string2, 0, malloc_len);

    // 加密
    {
        unsigned char input[17]; // 这些是16就够了，留下最后一位0用来调试输出
        unsigned char output [17];

        for (int i = 0; i < len; i+=16)
        {
            memset((void*)input, 0, 17);
            strncpy((char *)input,(char *)(string+i), len-i<16?len-i:16);
            aes.Cipher(input, output);
            strncpy(out_string+i, (char*)output, 16);

            printf(" >> %d:%s\n", i, input);
            print_uchar(input, 16);
            print_uchar(output, 16);
        }
        printf("out_string:%s\n", out_string);
    }
qDebug() << "========================";
    // 解密
    {
        unsigned char input[17];
        unsigned char output [17];

        for (int i = 0; i < len; i+=16)
        {
            memset((void*)input, 0, 17);
            strncpy((char *)input,(char *)(out_string+i), len-i<16?len-i:16);
            aes.InvCipher(input, output);
            strncpy(out_string2+i, (char*)output, 16);

            printf(" >> %d:%s\n", i, output);
            print_uchar(input, 16);
            print_uchar(output, 16);
        }
        printf("out_string2:%s\n", out_string2);
    }

    free(string);
    free(out_string);
    free(out_string2);

qDebug() << "结束";
}

/**
 * 使用 AES Util
 */
void test2()
{
    /*unsigned char key[] =
    {
        0x2b, 0x7e, 0x15, 0x16,
        0x28, 0xae, 0xd2, 0xa6,
        0xab, 0xf7, 0x15, 0x88,
        0x09, 0xcf, 0x4f, 0x3c
    };*/

    QString key("xpzotmxxmkrjxbyi");
    AESUtil au(key);
    QString text = "texttexttttttttttttttttt";
    qDebug() << "原来的：" << text;
    QString enc = au.encryption(text);
    qDebug() << "加密后：" << enc;
    QString dec = au.decryption(enc);
    qDebug() << "解密后：" << dec;
}

/**
 * 使用 Base64 + XOR
 */
void test3()
{
    QString text("<USERID>ID</USERID>");
    qDebug() << "原来：" << text;
    BXORCryptUtil bu("hhhhhhh~\nhhhhhh~~\nhhhh~~~\nhhh~~~~\nhh~~~~~\nh~~~~~~");
    QString jiami = bu.encrypt(text);
    qDebug() << "加密：" << jiami;
    QString jiemi = bu.decrypt(jiami);
    qDebug() << "解密：" << jiemi;
}

/**
 * 测试 Base64 + XOR
 */
void test4()
{
    QString text("待加密的字符串");

    QByteArray base64 = text.toLocal8Bit().toBase64();
    base64 = BXORCryptUtil::getXorEncryptDecrypt(base64, ',');

    QString jiami_str = QString(base64);
    qDebug() << "加密后：" << jiami_str;

    QByteArray jiemi_qian = jiami_str.toLocal8Bit();
    jiemi_qian = BXORCryptUtil::getXorEncryptDecrypt(jiemi_qian, ',');
    QByteArray jiemi_ba = QByteArray::fromBase64(jiemi_qian);
    qDebug() << QString::fromLocal8Bit(jiemi_ba);
}

int main(int argc, char **argv)
{
    QCoreApplication app(argc, argv);

//    test1();
//    test2();

    test3();
//    test4();


    return app.exec();
}


