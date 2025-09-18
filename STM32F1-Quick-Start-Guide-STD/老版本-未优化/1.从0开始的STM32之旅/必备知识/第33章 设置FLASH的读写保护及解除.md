# 第三十三章 设置FLASH的读写保护及解除

## 1. 选项字节与读写保护

在实际发布的产品中，在STM32芯片的内部FLASH存储了控制程序，如果不作任何保护措施的话，可以使用下载器直接把内部FLASH的内容读取回来， 得到bin或hex文件格式的代码拷贝，别有用心的厂商会利用该方法山寨产品。为此，STM32芯片提供了多种方式保护内部FLASH的程序不被非法读取， 但在默认情况下该保护功能是不开启的，若要开启该功能，需要改写内部FLASH选项字节(Option Bytes)中的配置。

### 1.1 选项字节的内容

选项字节是一段特殊的FLASH空间，STM32芯片会根据它的内容进行读写保护配置， 选项字节的构成见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHp01.png)

STM32F103系列芯片的选项字节有8个配置项，即上表中的USER、RDP、DATA0/1及WRP0/1/2/3，而表中带n的同类项是该项的反码， 即nUSER的值等于(~USER)、nRDP的值等于(~RDP)，STM32利用反码来确保选项字节内容的正确性。

选项字节的8个配置项具体的数据位配置说明见表

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHp02.png)

我们主要讲解选项字节配置中的RDP位和WRP位，它们分别用于配置读保护和写保护。

### 1.2 RDP读保护

修改选项字节的RDP位的值可设置内部FLASH为以下保护级别：

- 0xA5：无保护

> 这是STM32的默认保护级别，它没有任何读保护，读取内部FLASH的内容都没有任何限制。也就是说，第三方可以使用调试器等工具， 获取该芯片FLASH中存储的程序，然后可以把获得的程序以bin和hex的格式下载到另一块STM32芯片中，加上PCB抄板技术，轻易复制出同样的产品。

- 其它值：使能读保护

> 把RDP配置成除0xA5外的任意数值， 都会使能读保护。在这种情况下， 若使用调试功能(使用下载器、仿真器)或者从内部SRAM自举时都不能对内部FLASH作任何访问(读写、擦除都被禁止)； 而如果STM32是从内部FLASH自举时，它允许对内部FLASH的任意访问。也就是说，任何尝试从外部访问内部FLASH内容的操作都被禁止。 例如，无法通过下载器读取它的内容，或编写一个从内部SRAM启动的程序，若该SRAM启动的程序读取内部FLASH，会被禁止。 而如果是芯片原本的内部FLASH程序自己访问内部FLASH（即从FLASH自举的程序），是完全没有问题的，例如芯片本身的程序， 若包含有指针对内部FLASH某个地址进行的读取操作，它能获取正常的数据。
> 
> 另外，被设置成读保护后，FLASH前4K字节的空间会强制加上写保护，也就是说，即使是从FLASH启动的程序，也无法擦写这4K字节空间的内容； 而对于前4K字节以外的空间，读保护并不影响它对其它空间的擦除/写入操作。利用这个特性， 可以编写IAP代码(In Application Program)更新FLASH中的程序， 它的原理是通过某个通讯接口获取将要更新的程序内容，然后利用内部FLASH擦写操作把这些内容烧录到自己的内部FLASH中，实现应用程序的更新， 该原理类似串口ISP程序下载功能，只不过ISP这个接收数据并更新的代码由ST提供，且存放在系统存储区域，而IAP是由用户自行编写的， 存放在用户自定义的FLASH区域，且通讯方式可根据用户自身的需求定制，如IIC、SPI等，只要能接收到数据均可。

- 解除保护

> 当需要解除芯片的读保护时，要把选项字节的RDP位重新设置为0xA5。在解除保护前，芯片会自动触发擦除主FLASH存储器的全部内容， 即解除保护后原内部FLASH的代码会丢失，从而防止降级后原内容被读取到。

芯片被配置成读保护后根据不同的使用情况，访问权限不同，总结。

![](https://doc.embedfire.com/mcu/stm32/f103zhinanzhe/std/zh/latest/_images/FLASHp03.png)

### 1.3 WRP写保护

使用选项字节的WRP0/1/2/3可以设置主FLASH的写保护，防止它存储的程序内容被修改。

- 设置写保护

> 写保护的配置一般以4K字节为单位，除WRP3的最后一位比较特殊外，每个WRP选项字节的一位用于控制4K字节的写访问权限， 把对应WRP的位置0即可把它匹配的空间加入写保护。被设置成写保护后，主FLASH中的内容使用任何方式都不能被擦除和写入， 写保护不会影响读访问权限，读访问权限完全由前面介绍的读保护设置限制。

- 解除写保护

> 解除写保护是逆过程，把对应WRP的位置1即可把它匹配的空间解除写保护。解除写保护后，主FLASH中的内容不会像解读保护那样丢失，它会被原样保留。

## 2. 修改选项字节的过程

根据前面的说明，修改选项字节的内容可修改读写保护配置，不过选项字节复位后的默认状态是始终可以读但被写保护的， 因此它具有类似前面《读写内部FLASH》章节提到的FLASH_CR寄存器的访问限制，要想修改，需要先对FLASH_OPTKEYR寄存器写入解锁编码。 由于修改选项字节时也需要访问FLASH_CR寄存器，所以同样也要对FLASH_KEYR写入解锁编码。

修改选项字节的整个过程总结如下：

1. 解除FLASH_CR寄存器的访问限制
- 往FPEC键寄存器 FLASH_KEYR中写入 KEY1 = 0x45670123

- 再往FPEC键寄存器 FLASH_KEYR中写入 KEY2 = 0xCDEF89AB
2. 解除对选项字节的访问限制
- 往FLASH_OPTKEYR中写入 KEY1 = 0x45670123

- 再往FLASH_OPTKEYR中写入 KEY2 = 0xCDEF89AB
3. 配置FLASH_CR的OPTPG位，准备修改选项字节

4. 直接使用指针操作修改选项字节的内容，根据需要修改RDP、WRP等内容

5. 对于读保护的解除，由于它会擦除FLASH的内容，所以需要检测状态寄存器标志位以确认FLASH擦除操作完成。

6. 若是设置读保护及其解除，需要给芯片重新上电复位，以使新配置的选项字节生效；对于设置写保护及其解除， 需要给芯片进行系统复位，以使新配置的选项字节生效。

## 3. 操作选项字节的库函数

为简化编程，STM32标准库提供了一些库函数，它们封装了前面介绍的修改选项字节时的操作过程。

### 3.1 选项字结构体定义

```c
/**
* @brief 选项字节结构体
*/
typedef struct {
    __IO uint16_t RDP;  /*RDP及nRDP*/
    __IO uint16_t USER; /*USER及nUSER,下面类似*/
    __IO uint16_t Data0;
    __IO uint16_t Data1;
    __IO uint16_t WRP0;
    __IO uint16_t WRP1;
    __IO uint16_t WRP2;
    __IO uint16_t WRP3;
} OB_TypeDef;

/*强制转换为选项字节结构体指针*/
#define OB              ((OB_TypeDef *) OB_BASE)
/*选项字节基地址 */
#define OB_BASE         ((uint32_t)0x1FFFF800)
```

标准库中定义的选项字节结构体，包含了RDP、USER、DATA0/1及WRP0/1/2/3这些内容，每个结构体成员指向选项字节对应选项的原始配置码及反码。不过， 根据手册中的说明可了解到，当向选项字节的这些地址写入配置时，它会自动取低位字节计算出高位字节的值再存储，即自动取反码，非常方便。 例如程序中执行操作给结构体成员WRP0赋值为0x0011时，最终它会自动写入0xEE11（0xEE是0x11的反码）。最后， 从OB_BASE宏的定义可以确认它所指向的正是前面介绍的选项字节基地址，说明若在程序中使用该结构体赋值，会直接把内容写入到选项字节地址对应的空间中。

### 3.2 设置写保护及解除

```c
#define RDP_Key                  ((uint16_t)0x00A5)

/**
* @brief  使能或关闭读保护
* @note   若芯片本身有对选项字节进行其它操作，
            请先读出然后再重新写入，因为本函数会擦除所有选项字节的内容

* @param  Newstate: 使能（ENABLE）或关闭（DISABLE）
* @retval FLASH Status: 可能的返回值: FLASH_ERROR_PG,
*         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
*/
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState)
{
    FLASH_Status status = FLASH_COMPLETE;
    /* 检查参数 */
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    status = FLASH_WaitForLastOperation(EraseTimeout);
    if (status == FLASH_COMPLETE) {
        /* 写入选项字节解锁码 */
        FLASH->OPTKEYR = FLASH_KEY1;
        FLASH->OPTKEYR = FLASH_KEY2;
        FLASH->CR |= CR_OPTER_Set; //擦除选项字节
        FLASH->CR |= CR_STRT_Set;  //开始擦除
        /* 等待上一次操作完毕 */
        status = FLASH_WaitForLastOperation(EraseTimeout);
        if (status == FLASH_COMPLETE) {
            /* 若擦除操作完成，复位 OPTER 位 */
            FLASH->CR &= CR_OPTER_Reset;
            /* 准备写入选项字节 */
            FLASH->CR |= CR_OPTPG_Set;
            if (NewState != DISABLE) {
                OB->RDP = 0x00;//写入非0xA5值，进行读保护
            } else {
                OB->RDP = RDP_Key;  //写入0xA5，解除读保护
            }
            /* 等待上一次操作完毕 */
            status = FLASH_WaitForLastOperation(EraseTimeout);

            if (status != FLASH_TIMEOUT) {
                /* 若操作完毕，复位 OPTPG 位 */
                FLASH->CR &= CR_OPTPG_Reset;
            }
        } else {
            if (status != FLASH_TIMEOUT) {
                /* 复位 OPTER 位 */
                FLASH->CR &= CR_OPTER_Reset;
            }
        }
    }
    /* 返回设置结果 */
    return status;
}
```

该函数的输入参数可选FLASH_WRProt_Pages0to1至FLASH_WRProt_Pages62to511等宏，该参数用于指定要对哪些页进行写保护。

从该宏的定义方式可了解到，它用一个32位的数值表示WRP0/1/2/3，而宏名中的页码使用数据位1来在WRP0/1/2/3中对应的位作掩码指示。 如控制页0至页1的宏FLASH_WRProt_Pages0to1，它由WRP0最低位控制，所以其宏值为0x00000001（bit0为1）；类似地， 控制页2至页3的宏FLASH_WRProt_Pages2to3，由WRP0的bit1控制，所以其宏值为0x00000002（bit1为1）。

理解了输入参数宏的结构后，即可分析函数中的具体代码。其中最核心要理解的是对输入参数的运算，输入参数FLASH_Pages自身会进行取反操作， 从而用于指示要保护页的宏对应的数据位会被置0，而在选项字节WRP中，被写0的数据位对应的页会被保护。FLASH_Pages取反后的值被分解成WRP0/1/2/3_Data四个部分， 所以在后面的代码中，可以直接把WRP0/1/2/3_Data变量的值写入到选项字节中。关于这部分运算，您可以亲自代入几个宏进行运算，加深理解。

得到数据后，函数开始对FLASH_OPTKEYR寄存器写入解锁码，然后操作FLASH_CR寄存器的OPTPG位准备写入，写入的时候它直接往指向选项字节的结构体OB赋值， 如OB->WRP0 =WRP0_Data，注意在这部分写入的时候，根据前面的运算，可知WRP0_Data中只包含了WRP0的内容， 而nWRP0的值为0，这个nWRP0的值最终会由芯片自动产生。 代码后面的WRP1/2/3操作类似。

仔细研究了这个库函数后，可知它内部并没有对FLASH_CR的访问作解锁操作，所以在调用本函数前，需要先调用FLASH_Unlock解锁。另外，库文件中并没有直接的函数用于解除保护， 但实际上解除保护也可以使用这个函数来处理，例如使用输入参数0来调用函数FLASH_EnableWriteProtection(0)，根据代码的处理，它最终会向WRP0/1/2/3选项字节全写入1， 从而达到整片FLASH解除写保护的目的。

### 3.3 设置读保护及解除

```c
#define RDP_Key                  ((uint16_t)0x00A5)


/**
* @brief  使能或关闭读保护
* @note   若芯片本身有对选项字节进行其它操作，
            请先读出然后再重新写入，因为本函数会擦除所有选项字节的内容

* @param  Newstate: 使能（ENABLE）或关闭（DISABLE）
* @retval FLASH Status: 可能的返回值: FLASH_ERROR_PG,
*         FLASH_ERROR_WRP, FLASH_COMPLETE or FLASH_TIMEOUT.
*/
FLASH_Status FLASH_ReadOutProtection(FunctionalState NewState)
{
    FLASH_Status status = FLASH_COMPLETE;
    /* 检查参数 */
    assert_param(IS_FUNCTIONAL_STATE(NewState));
    status = FLASH_WaitForLastOperation(EraseTimeout);
    if (status == FLASH_COMPLETE) {
        /* 写入选项字节解锁码 */
        FLASH->OPTKEYR = FLASH_KEY1;
        FLASH->OPTKEYR = FLASH_KEY2;
        FLASH->CR |= CR_OPTER_Set; //擦除选项字节
        FLASH->CR |= CR_STRT_Set;  //开始擦除
        /* 等待上一次操作完毕 */
        status = FLASH_WaitForLastOperation(EraseTimeout);
        if (status == FLASH_COMPLETE) {
            /* 若擦除操作完成，复位 OPTER 位 */
            FLASH->CR &= CR_OPTER_Reset;
            /* 准备写入选项字节 */
            FLASH->CR |= CR_OPTPG_Set;
            if (NewState != DISABLE) {
                OB->RDP = 0x00;//写入非0xA5值，进行读保护
            } else {
                OB->RDP = RDP_Key;  //写入0xA5，解除读保护
            }
            /* 等待上一次操作完毕 */
            status = FLASH_WaitForLastOperation(EraseTimeout);

            if (status != FLASH_TIMEOUT) {
                /* 若操作完毕，复位 OPTPG 位 */
                FLASH->CR &= CR_OPTPG_Reset;
            }
        } else {
            if (status != FLASH_TIMEOUT) {
                /* 复位 OPTER 位 */
                FLASH->CR &= CR_OPTER_Reset;
            }
        }
    }
    /* 返回设置结果 */
    return status;
}
```

由于读保护都是针对整个芯片的，所以读保护的配置函数相对简单，它通过输入参数ENABLE或DISABL参数来进行保护或解除。它的内部处理与前面介绍的修改选项字节过程完全一致， 当要进行读保护时，往选项字节结构体OB->RDP写入0x00（实际上写入非0xA5的值均可达到目的），而要解除读保护时，则写入0xA5。

要注意的是，本函数同样有对FLASH_CR寄存器的访问，但并没有进行解锁操作，所以调用本函数前，同样需要先使用FLASH_Unlock函数解锁。
