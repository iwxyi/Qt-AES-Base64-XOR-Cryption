Qt 字符串加密工具
===

## AES 加密

### 介绍

> AES算法来源：<https://blog.csdn.net/u012372584/article/details/89095484>

针对纯 C 语言版本的 AES 加密解密算法，加上了字符串类型转换，支持 Qt 最常用的 QString 类型。

原版只支持16位长度以下的char*，通过字符串分割、加密后拼接的方法，支持理论无限长度字符串。

并且在 Util 中保存秘钥 key，初始化后传入字符串即可加密解密。

### 用法

```C++
QString key("xpzotmxxmkrjxbyi");
AESUtil au(key);

QString text = "texttexttttttttttttttttt";
qDebug() << "原来的：" << text;

QString enc = au.encryption(text);
qDebug() << "加密后：" << enc;

QString dec = au.decryption(enc);
qDebug() << "解密后：" << dec;
```

### 结果

```
原来的： "texttexttttttttttttttttt"
开始加密 len= 24
 >> 0:texttexttttttttt
74 65 78 74 74 65 78 74 74 74 74 74 74 74 74 74
36 41 18 76 3B 44 F3 94 99 23 1E 16 DC E1 2D 32
 >> 16:tttttttt
74 74 74 74 74 74 74 74 0 0 0 0 0 0 0 0
60 96 87 5A A9 4E DA 4C DE 93 D1 64 1 A2 6A 82
out_string:6Av;D髷?茚-2`枃Z㎞贚迵裠
加密后： "6A\u0018v;Dó\u0094\u0099#\u001E\u0016üá-2`\u0096\u0087Z?NúLT\u0093?d\u0001￠j\u0082"
char:6Av;D髷?茚-2`枃Z㎞贚迵裠
out_string2:texttexttttttttttttttttt
解密后： "texttexttttttttttttttttt"
```

### 问题

1. 偶尔会遇到卡死的情况（解密结束的那一刹那会卡住一会儿，会输出char*格式的字符串，但不会返回以之为构造函数参数的QString字符串，可能是某个没发现的内存溢出？）
2. 不支持中文（但是先URL编码等方式）



---



## Base64 + XOR 加密

### 介绍

一种比较简单的加密方式，算法也是网上看到的

> 来源：https://blog.csdn.net/CLinuxF/article/details/81975923

原理：Base64编码（这不是加密），然后通过一个 key 对每个字符串进行`异或`操作。

如果没有 key 则无法解密（当然暴力也或许可以）。

原版只是一个 char 格式的秘钥，十分容易被穷举法破解，将至升级为字符串，并且允许多行，一行一个 key，即多个字符串多层加密。

> 当然，由于异或的特征，如果有两层(行)秘钥一模一样，或者同样长度秘钥对应位置字符相同且是偶数个，便不会有加密的效果。

### 原理

1. 先转换成字节数组 QByteArray，使用 Base64 编码

   ```C++
   QString text("待加密的字符串");
   QByteArray base64 = text.toLocal8Bit().toBase64();
   ```

2. 异或加密

   ```C++
   base64 = BXORCryptUtil::getXorEncryptDecrypt(base64, ',');
   ```

上面是加密的，解密顺序反过来。

### 用法

```C++
QString key("hhhhhhh~\nhhhhhh~~\nhhhh~~~\nhhh~~~~\nhh~~~~~\nh~~~~~~"); // 6个秘钥
BXORCryptUtil bu(key);

QString text("<USERID>ID</USERID>");
qDebug() << "原来：" << text;

QString jiami = bu.encrypt(text);
qDebug() << "加密：" << jiami;

QString jiemi = bu.decrypt(jiami);
qDebug() << "解密：" << jiemi;
```

### 结果

```
原来： "<USERID>ID</USERID>"
加密： "PPVBRV\\JDD#JRDa`V@NFUklEFg=="
解密： "<USERID>ID</USERID>"
```

### 特点

- 支持中文
- 支持多个秘钥多重加密
- 长度不限，但是部分秘钥会莫名导致解密后丢失一部分文字，需自行测试

