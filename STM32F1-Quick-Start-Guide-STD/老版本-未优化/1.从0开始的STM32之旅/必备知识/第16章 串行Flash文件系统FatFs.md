# 第十六章 串行Flash文件系统FatFs

## 1. FatFs文件系统简介

上面提到的逻辑转换部分代码(文件系统)即为本章的要点，文件系统庞大而复杂，它需要根据应用的文件系统格式而编写， 而且一般与驱动层分离开来，很方便移植，所以工程应用中一般是移植现成的文件系统源码。

FatFs是面向小型嵌入式系统的一种通用的FAT文件系统。它完全是由ANSIC语言编写并且完全独立于底层的I/O介质。 因此它可以很容易地不加修改地移植到其他的处理器当中，如8051、PIC、AVR、SH、Z80、H8、ARM等。FatFs支持FAT12、FAT16、FAT32等格式， 所以我们利用前面写好的SPI Flash芯片驱动，把FatFs文件系统代码移植到工程之中， 就可以利用文件系统的各种函数，对SPI Flash芯片以“文件”格式进行读写操作了。

FatFs文件系统的源码可以从fatfs官网下载:[FatFs - Generic FAT Filesystem Module (elm-chan.org)](http://elm-chan.org/fsw/ff/00index_e.html)

### 1.1 FatFs的目录结构

在移植FatFs文件系统到开发板之前，我们先要到FatFs的官网获取源码，官网有对FatFs做详细的介绍，有兴趣可以了解。 解压之后可看到里面有 doc 和 src 这两个文件夹。doc 文件夹里面是一些使用帮助文档； src 才是FatFs文件系统的源码。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF003.png)

### 1.2 FatFs帮助文档

打开 doc 文件夹，可看到如图的文件目录：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF004.png)

其中 en 和 ja 这两个文件夹里面是编译好的html文档，讲的是FATFS里面各个函数的使用方法，这些函数都是封装得非常好的函数， 利用这些函数我们就可以操作SPI Flash芯片。有关具体的函数我们在用到的时候再讲解。这两个文件夹的唯一区别就是 en 文件夹下的文档是英文的， ja文件夹下的是日文的。img文件夹包含en和ja文件夹下文件需要用到的图片，还有四个名为app.c文件，内容都是FatFs具体应用例程。 00index_e.html和00index_j.html是一些关于FATFS的简介，至于另外两个文件可以不看。

### 1.3 FatFs源码

打开 src 文件夹，可看到如图的文件目录：

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHF005.png)

option 文件夹下是一些可选的外部c文件，包含了多语言支持需要用到的文件和转换函数。

diskio.c文件是FatFs移植最关键的文件，它为文件系统提供了最底层的访问SPI Flash芯片的方法， FatFs有且仅有它需要用到与SPI Flash芯片相关的函数。 diskio.h定义了FatFs用到的宏，以及diskio.c文件内与底层硬件接口相关的函数声明。

00history.txt介绍了FatFs的版本更新情况。

00readme.txt说明了当前目录下 diskio.c 、diskio.h、ff.c、ff.h、integer.h的功能。

src文件夹下的源码文件功能简介如下：

- integer.h：文件中包含了一些数值类型定义。

- diskio.c：包含底层存储介质的操作函数，这些函数需要用户自己实现，主要添加底层驱动函数。

- ff.c： FatFs核心文件，文件管理的实现方法。该文件独立于底层介质操作文件的函数，利用这些函数实现文件的读写。

- cc936.c：本文件在option目录下，是简体中文支持所需要添加的文件，包含了简体中文的GBK和Unicode相互转换功能函数。

- ffconf.h: 这个头文件包含了对FatFs功能配置的宏定义，通过修改这些宏定义就可以裁剪FatFs的功能。如需要支持简体中文， 需要把 ffconf.h中的_CODE_PAGE 的宏改成936并把上面的cc936.c文件加入到工程之中。

更多内容可参考：[FatFs文件系统 - 孤情剑客 - 博客园](https://www.cnblogs.com/The-explosion/p/13170732.html)

---

2024.9.6 第一次修订，后期不再维护

2025.2.18 简化内容
