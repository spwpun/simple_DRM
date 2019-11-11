# simple_DRM
- status: Finished.
## 功能：
- 能产生电脑唯一的序列号，使用SM3杂凑算法，结合电脑的网卡MAC地址和CPU信息以及硬盘序列号而得，并将此序列号经过MD5哈希后得到后续的SM4加密的密钥。
- 能使用SM4算法对文件进行加解密。
- 实现了pdf文件隐写算法，嵌入版权信息，但目前嵌入的信息只是明文的，后续可改进。
- 实现文件信息嵌入，文件信息是以一个结构体的方式嵌入加密后的文件头部。



Updated the last project in [PdfDRM_basic](https://github.com/spwpun/PdfDRM_basic.git)

And deleted private files from old project, maybe will occur error in your compiling, but that must be file not found. 

