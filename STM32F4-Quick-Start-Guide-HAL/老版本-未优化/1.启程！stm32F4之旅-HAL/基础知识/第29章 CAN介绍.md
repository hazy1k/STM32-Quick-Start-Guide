# 第二十九章 CAN介绍

## 1. CAN简介

CAN 是 Controller Area Network 的缩写（以下称为 CAN），是 ISO 国际标准化的串行通信协议。在当前的汽车产业中，出于对安全性、舒适性、方便性、低公害、低成本的要求，各种各样的电子控制系统被开发了出来。由于这些系统之间通信所用的数据类型及对可靠性的要求不尽相同，由多条总线构成的情况很多，线束的数量也随之增加。为适应“减少线束的数量”、“通过多个 LAN，进行大量数据的高速通信”的需要， 1986 年德国电气商博世公司开发出面向汽车的 CAN 通信协议。此后， CAN 通过 ISO11898 及 ISO11519 进行了标准化，现在在欧洲已是汽车网络的标准协议。

现在， CAN 的高性能和可靠性已被认同，并被广泛地应用于工业自动化、船舶、医疗设备、工业设备等方面。现场总线是当今自动化领域技术发展的热点之一，被誉为自动化领域的计算机局域网。它的出现为分布式控制系统实现各节点之间实时、可靠的数据通信提供了强有力的技术支持。

CAN 控制器根据两根线上的电位差来判断总线电平。总线电平分为显性电平和隐性电平，二者必居其一。发送方通过使总线电平发生变化，将消息发送给接收方。

CAN 协议经过 ISO 标准化后有两个标准： ISO11898标准和 ISO11519-2 标准。其中 ISO11898是针对通信速率为 125Kbps~1Mbps 的高速通信标准，而 ISO11519-2 是针对通信速率为 125Kbps以下的低速通信标准。

本章，我们使用的是 500Kbps 的通信速率，使用的是 ISO11898 标准，该标准的物理层特征如图所示：

![屏幕截图 2024-10-21 225634.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-22-56-37-屏幕截图%202024-10-21%20225634.png)

从该特性可以看出，显性电平对应逻辑 0， CAN_H 和 CAN_L 之差为 2.5V 左右。而隐性电平对应逻辑 1， CAN_H 和 CAN_L 之差为 0V。在总线上显性电平具有优先权，只要有一个单元输出显性电平，总线上即为显性电平。而隐形电平则具有包容的意味，只有所有的单元都输出隐性电平，总线上才为隐性电平（显性电平比隐性电平更强）。另外，在 CAN 总线的起止端都有一个 120Ω的终端电阻，来做阻抗匹配，以减少回波反射。

## 2. 帧格式

CAN 协议是通过以下 5 种类型的帧进行的：

- 数据帧

- 遥控帧

- 错误帧

- 过载帧

- 间隔帧

另外，数据帧和遥控帧有标准格式和扩展格式两种格式。标准格式有 11 个位的标识符（ID），扩展格式有 29 个位的 ID。各种帧的用途如表所示：

![屏幕截图 2024-10-21 225853.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-22-58-58-屏幕截图%202024-10-21%20225853.png)

由于篇幅所限，我们这里仅对数据帧进行详细介绍，数据帧一般由 7 个段构成，即：

1. 起始帧：表示数据帧开始的段

2. 仲裁段：表示该帧优先级的段

3. 控制段：表示数据的字节数及保留位的段

4. 数据段：数据的内容，一帧可发送0~8个字节的数据

5. CRC段：检查帧的传输错误的段

6. ACK段：表示确认正常接收的段

7. 帧结束：表示数据帧结束的段

数据帧的构成如图所示：

![屏幕截图 2024-10-21 230152.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-01-56-屏幕截图%202024-10-21%20230152.png)

图中 D 表示显性电平， R 表示隐形电平（下同）。

### 2.1 帧起始

帧起始，这个比较简单，标准帧和扩展帧都是由 1 个位的显性电平表示帧起始。

### 2.2 仲裁段

表示数据优先级的段，标准帧和扩展帧格式在本段有所区别，如图所示：

![屏幕截图 2024-10-21 230315.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-03-18-屏幕截图%202024-10-21%20230315.png)

标准格式的 ID 有 11 个位。从 ID28 到 ID18 被依次发送。禁止高 7 位都为隐性（禁止设定： ID=1111111XXXX）。扩展格式的 ID 有 29 个位。基本 ID 从 ID28 到 ID18，扩展 ID 由ID17 到 ID0 表示。基本 ID 和标准格式的 ID 相同。禁止高 7 位都为隐性（禁止设定：基本ID=1111111XXXX）。

其中 RTR 位用于标识是否是远程帧（0，数据帧； 1，远程帧）， IDE 位为标识符选择位（0，使用标准标识符； 1，使用扩展标识符）， SRR 位为代替远程请求位，为隐性位，它代替了标准帧中的 RTR 位。

### 2.3 控制段

由 6 个位构成，表示数据段的字节数。标准帧和扩展帧的控制段稍有不同，如图所示：

![屏幕截图 2024-10-21 230430.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-04-36-屏幕截图%202024-10-21%20230430.png)

上图中， r0 和 r1 为保留位，必须全部以显性电平发送，但是接收端可以接收显性、隐性及任意组合的电平。 DLC 段为数据长度表示段，高位在前， DLC 段有效值为 0~8，但是接收方接收到 9~15 的时候并不认为是错误。

### 2.4 数据段

该段可包含 0~8 个字节的数据。从最高位（MSB）开始输出，标准帧和扩展帧在这个段的定义都是一样的。如图所示：

![屏幕截图 2024-10-21 230512.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-05-15-屏幕截图%202024-10-21%20230512.png)

### 2.5 CRC段

该段用于检查帧传输错误。由 15 个位的 CRC 顺序和 1 个位的 CRC 界定符（用于分隔的位）组成，标准帧和扩展帧在这个段的格式也是相同的。如图所示：

![屏幕截图 2024-10-21 230542.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-05-45-屏幕截图%202024-10-21%20230542.png)

此段 CRC 的值计算范围包括： 帧起始、仲裁段、控制段、数据段。接收方以同样的算法计算 CRC 值并进行比较，不一致时会通报错误。

### 2.6 ACK段

ACK 段，此段用来确认是否正常接收。由 ACK 槽(ACK Slot)和 ACK 界定符 2 个位组成。标准帧和扩展帧在这个段的格式也是相同的。

![屏幕截图 2024-10-21 230617.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-06-20-屏幕截图%202024-10-21%20230617.png)

发送单元的 ACK，发送 2 个位的隐性位，而接收到正确消息的单元在 ACK 槽（ACK Slot）发送显性位，通知发送单元正常接收结束，这个过程叫发送 ACK/返回 ACK。发送 ACK 的是在既不处于总线关闭态也不处于休眠态的所有接收单元中，接收到正常消息的单元（发送单元不发送 ACK）。所谓正常消息是指不含填充错误、格式错误、 CRC 错误的消息。

### 2.7 帧结束

帧结束，这个段也比较简单，标准帧和扩展帧在这个段格式一样，由 7 个位的隐性位组成。

## 3. CAN的位时序

由发送单元在非同步的情况下发送的每秒钟的位数称为位速率。一个位可分为 4 段。

- 同步段（SS）

- 传播时间段（RTS）

- 相位缓冲段1（PBS1）

- 相位缓冲段2（PBS2）

这些段又由可称为 Time Quantum（以下称为 Tq）的最小时间单位构成。

1 位分为 4 个段，每个段又由若干个 Tq 构成，这称为位时序。

1 位由多少个 Tq 构成、每个段又由多少个 Tq 构成等，可以任意设定位时序。通过设定位时序，多个单元可同时采样，也可任意设定采样点。各段的作用和 Tq 数如表所示：

![屏幕截图 2024-10-21 230952.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/21-23-09-55-屏幕截图%202024-10-21%20230952.png)

1 个位的构成如图所示：

![屏幕截图 2024-10-22 092429.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-24-40-屏幕截图%202024-10-22%20092429.png)

上图的采样点，是指读取总线电平，并将读到的电平作为位值的点。位置在 PBS1 结束处。根据这个位时序，我们就可以计算 CAN 通信的波特率了。具体计算方法，我们等下再介绍，前面提到的 CAN 协议具有仲裁功能，下面我们来看看是如何实现的。

在总线空闲态，最先开始发送消息的单元获得发送权。

当多个单元同时开始发送时，各发送单元从仲裁段的第一位开始进行仲裁。连续输出显性电平最多的单元可继续发送。实现过程，如图所示：

![屏幕截图 2024-10-22 092516.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-25-19-屏幕截图%202024-10-22%20092516.png)

上图中，单元 1 和单元 2 同时开始向总线发送数据，开始部分他们的数据格式是一样的，故无法区分优先级，直到 T 时刻，单元 1 输出隐性电平，而单元 2 输出显性电平，此时单元 1仲裁失利，立刻转入接收状态工作，不再与单元 2 竞争，而单元 2 则顺利获得总线使用权，继续发送自己的数据。这就实现了仲裁，让连续发送显性电平多的单元获得总线使用权。

## 4. STM32F4的CAN控制器

STM32F4 自带的是 bxCAN，即基本扩展 CAN。它支持 CAN 协议 2.0A 和 2.0B。它的设计目标是，以最小的 CPU 负荷来高效处理大量收到的报文。它也支持报文发送的优先级要求(优先级特性可软件配置)。对于安全紧要的应用， bxCAN 提供所有支持时间触发通信模式所需的硬件功能。

在 STM32F407ZGT6 中，带有 2 个 CAN 控制器，而我们本章只用了 1 个 CAN，即 CAN1。双 CAN 的框图如图所示：

![屏幕截图 2024-10-22 092636.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-26-39-屏幕截图%202024-10-22%20092636.png)

从图中可以看出两个 CAN 都分别拥有自己的发送邮箱和接收 FIFO，但是他们共用 28 个滤波器。通过 CAN_FMR 寄存器的设置，可以设置滤波器的分配方式。

STM32F4 的标识符过滤是一个比较复杂的东西，它的存在减少了 CPU 处理 CAN 通信的开销。 STM32F4 的过滤器（也称筛选器）组最多有 28 个，每个滤波器组 x 由 2 个 32 为寄存器， CAN_FxR1 和 CAN_FxR2 组成。

STM32F4 每个过滤器组的位宽都可以独立配置，以满足应用程序的不同需求。根据位宽的不同，每个过滤器组可提供：

- 1 个 32 位过滤器，包括： STDID[10:0]、 EXTID[17:0]、 IDE 和 RTR 位

- 2 个 16 位过滤器，包括： STDID[10:0]、 IDE、 RTR 和 EXTID[17:15]位

此外过滤器可配置为，屏蔽位模式和标识符列表模式。

在屏蔽位模式下，标识符寄存器和屏蔽寄存器一起，指定报文标识符的任何一位，应该按照“必须匹配”或“不用关心”处理。

而在标识符列表模式下，屏蔽寄存器也被当作标识符寄存器用。因此，不是采用一个标识符加一个屏蔽位的方式，而是使用 2 个标识符寄存器。接收报文标识符的每一位都必须跟过滤器标识符相同。

通过 CAN_FMR 寄存器，可以配置过滤器组的位宽和工作模式，如图所示：

![屏幕截图 2024-10-22 092831.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-28-35-屏幕截图%202024-10-22%20092831.png)

为了过滤出一组标识符，应该设置过滤器组工作在屏蔽位模式。

为了过滤出一个标识符，应该设置过滤器组工作在标识符列表模式。

应用程序不用的过滤器组，应该保持在禁用状态。

举个简单的例子，我们设置过滤器组 0 工作在： 1 个 32 位过滤器-标识符屏蔽模式，然后设置 CAN_F0R1=0XFFFF0000， CAN_F0R2=0XFF00FF00。其中存放到 CAN_F0R1 的值就是期望收到的 ID，即我们希望收到的 ID（ STID+EXTID+IDE+RTR）最好是： 0XFFFF0000。而0XFF00FF00 就是设置我们需要必须关心的 ID，表示收到的 ID，其位[31:24]和位[15:8]这 16 个位的必须和 CAN_F0R1 中对应的位一模一样，而另外的 16 个位则不关心，可以一样，也可以不一样，都认为是正确的 ID，即收到的 ID 必须是 0XFFxx00xx，才算是正确的（x 表示不关心）。

### 4.1 CAN发送流程

CAN 发送流程为：程序选择 1 个空置的邮箱（TME=1） →设置标识符（ID），数据长度和发送数据→设置 CAN_TIxR 的 TXRQ 位为 1，请求发送→邮箱挂号（等待成为最高优先级） →预定发送（等待总线空闲） →发送→邮箱空置。整个流程如图所示：

![屏幕截图 2024-10-22 093018.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-30-21-屏幕截图%202024-10-22%20093018.png)

上图中，还包含了很多其他处理，终止发送（ABRQ=1）和发送失败处理等。通过这个流程图，我们大致了解了 CAN 的发送流程，后面的数据发送，我们基本就是按照此流程来走。接下来再看看 CAN 的接收流程。

### 4.2 CAN接收流程

CAN 接收到的有效报文，被存储在 3 级邮箱深度的 FIFO 中。 FIFO 完全由硬件来管理，从而节省了 CPU 的处理负荷，简化了软件并保证了数据的一致性。应用程序只能通过读取 FIFO输出邮箱，来读取 FIFO 中最先收到的报文。 这里的有效报文是指那些正确被接收的（直到 EOF都没有错误）且通过了标识符过滤的报文。前面我们知道 CAN 的接收有 2 个 FIFO，我们每个滤波器组都可以设置其关联的 FIFO，通过 CAN_FFA1R 的设置，可以将滤波器组关联到FIFO0/FIFO1。

CAN 接收流程为： FIFO 空→收到有效报文→挂号_1（存入 FIFO 的一个邮箱，这个由硬件控制，我们不需要理会） →收到有效报文→挂号_2→收到有效报文→挂号_3→收到有效报文→溢出。

这个流程里面，我们没有考虑从 FIFO 读出报文的情况，实际情况是：我们必须在 FIFO 溢出之前，读出至少 1 个报文，否则下个报文到来，将导致 FIFO 溢出，从而出现报文丢失。每读出 1 个报文，相应的挂号就减 1，直到 FIFO 空。 CAN 接收流程如图所示：

![屏幕截图 2024-10-22 093315.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-33-18-屏幕截图%202024-10-22%20093315.png)

FIFO 接收到的报文数，我们可以通过查询 CAN_RFxR 的 FMP 寄存器来得到，只要 FMP不为 0，我们就可以从 FIFO 读出收到的报文。

### 4.3 CAN位时间特性

接下来，我们简单看看 STM32F4 的 CAN 位时间特性， STM32F4 的 CAN 位时间特性和之前我们介绍的，稍有点区别。 STM32F4 把传播时间段和相位缓冲段 1（STM32F4 称之为时间段1）合并了，所以 STM32F4 的 CAN 一个位只有 3 段：同步段（SYNC_SEG）、时间段 1（BS1）和时间段 2（BS2）。 STM32F4 的 BS1 段可以设置为 1~16 个时间单元，刚好等于我们上面介绍的传播时间段和相位缓冲段 1 之和。 STM32F4 的 CAN 位时序如图所示：

![屏幕截图 2024-10-22 093411.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-34-14-屏幕截图%202024-10-22%20093411.png)

图中还给出了 CAN 波特率的计算公式，我们只需要知道 BS1 和 BS2 的设置，以及 APB1的时钟频率（一般为 42Mhz），就可以方便的计算出波特率。比如设置 TS1=6、 TS2=5 和 BRP=5，在 APB1 频率为 42Mhz 的条件下，即可得到 CAN 通信的波特率=42000/[(7+6+1)*6]=500Kbps。

## 5. CAN相关寄存器

### 5.1 主控制寄存器（CAN_MCR）

![屏幕截图 2024-10-22 093503.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-35-10-屏幕截图%202024-10-22%20093503.png)

这里我们仅介绍下 INRQ 位，该位用来控制初始化请求。

软件对该位清 0，可使 CAN 从初始化模式进入正常工作模式：当 CAN 在接收引脚检测到连续的 11 个隐性位后， CAN 就达到同步，并为接收和发送数据作好准备了。为此，硬件相应地对 CAN_MSR 寄存器的 INAK 位清’ 0’。

软件对该位置 1 可使 CAN 从正常工作模式进入初始化模式：一旦当前的 CAN 活动(发送或接收)结束， CAN 就进入初始化模式。相应地，硬件对 CAN_MSR 寄存器的 INAK 位置’ 1’。

所以我们在 CAN 初始化的时候，先要设置该位为 1，然后进行初始化（尤其是 CAN_BTR的设置，该寄存器，必须在 CAN 正常工作之前设置），之后再设置该位为 0，让 CAN 进入正常工作模式。

### 5.2 位时序寄存器（CAN_BTR）

该寄存器用于设置分频、 Tbs1、 Tbs2以及 Tsjw 等非常重要的参数，直接决定了 CAN 的波特率。另外该寄存器还可以设置 CAN 的工作模式，该寄存器各位描述如图所示：

![屏幕截图 2024-10-22 093642.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-36-45-屏幕截图%202024-10-22%20093642.png)

STM32F4 提供了两种测试模式，环回模式和静默模式，当然他们组合还可以组合成环回静默模式。这里我们简单介绍下环回模式。

在环回模式下， bxCAN 把发送的报文当作接收的报文并保存(如果可以通过接收过滤)在接收邮箱里。 也就是环回模式是一个自发自收的模式，如图所示：

![屏幕截图 2024-10-22 093724.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-37-28-屏幕截图%202024-10-22%20093724.png)

环回模式可用于自测试。为了避免外部的影响，在环回模式下 CAN 内核忽略确认错误(在数据/远程帧的确认位时刻，不检测是否有显性位)。在环回模式下， bxCAN 在内部把 Tx 输出回馈到 Rx 输入上，而完全忽略 CANRX 引脚的实际状态。发送的报文可以在 CANTX 引脚上检测到。

### 5.3 发送邮箱标识符寄存器（CAN_TIxR）

![屏幕截图 2024-10-22 093820.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-38-23-屏幕截图%202024-10-22%20093820.png)

该寄存器主要用来设置标识符（包括扩展标识符），另外还可以设置帧类型，通过 TXRQ值 1，来请求邮箱发送。因为有 3 个发送邮箱，所以寄存器 CAN_TIxR 有 3 个。

### 5.4 发送邮箱数据长度和时间戳寄存器（CAN_TDTxR）

该寄存器我们本章仅用来设置数据长度，即最低 4 个位。比较简单，这里就不详细介绍了。

### 5.5 发送邮箱低字节数据寄存器 (CAN_TDLxR)

![屏幕截图 2024-10-22 094255.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-42-59-屏幕截图%202024-10-22%20094255.png)

该寄存器用来存储将要发送的数据，这里只能存储低 4 个字节，另外还有一个寄存器CAN_TDHxR，该寄存器用来存储高 4 个字节，这样总共就可以存储 8 个字节。 CAN_TDHxR的各位描述同 CAN_TDLxR 类似，我们就不单独介绍了。

### 5.6 接收 FIFO 邮箱标识符寄存器 (CAN_RIxR)

该寄存器各位描述同 CAN_TIxR 寄存器几乎一模一样，只是最低位为保留位，该寄存器用于保存接收到的报文标识符等信息，我们可以通过读该寄存器获取相关信息。

同样的， CAN 接收 FIFO 邮箱数据长度和时间戳寄存器 (CAN_RDTxR) 、 CAN 接收 FIFO邮 箱 低 字 节 数 据 寄 存 器 (CAN_RDLxR) 和 CAN 接 收 FIFO 邮 箱 高 字 节 数 据 寄 存 器(CAN_RDHxR) 分别和发送邮箱的： CAN_TDTxR、 CAN_TDLxR 以及 CAN_TDHxR 类似，这里我们就不单独一一介绍了。

### 5.7 过滤器模式寄存器（CAN_FM1R）

![屏幕截图 2024-10-22 094958.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-50-01-屏幕截图%202024-10-22%20094958.png)

该寄存器用于设置各滤波器组的工作模式，对 28 个滤波器组的工作模式，都可以通过该寄存器设置，不过该寄存器必须在过滤器处于初始化模式下（CAN_FMR 的 FINIT 位=1），才可以进行设置。

### 5.8 过滤器位宽寄存器(CAN_FS1R)

![屏幕截图 2024-10-22 095041.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-50-44-屏幕截图%202024-10-22%20095041.png)

该寄存器用于设置各滤波器组的位宽，对 28 个滤波器组的位宽设置，都可以通过该寄存器实现。该寄存器也只能在过滤器处于初始化模式下进行设置。

### 5.9 过滤器 FIFO 关联寄存器（CAN_FFA1R）

![屏幕截图 2024-10-22 095111.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-51-15-屏幕截图%202024-10-22%20095111.png)

该寄存器设置报文通过滤波器组之后，被存入的 FIFO，如果对应位为 0，则存放到 FIFO0；如果为 1，则存放到 FIFO1。该寄存器也只能在过滤器处于初始化模式下配置。

### 5.10 过滤器激活寄存器（CAN_FA1R）

该寄存器各位对应滤波器组和前面的几个寄存器类似，这里就不列出了，对对应位置 1，即开启对应的滤波器组；置 0 则关闭该滤波器组。

### 5.11 过滤器组 i 的寄存器 x（CAN_FiRx）

![屏幕截图 2024-10-22 095406.png](https://raw.githubusercontent.com/hazy1k/My-drawing-bed/main/2024/10/22-09-54-12-屏幕截图%202024-10-22%20095406.png)

每个滤波器组的 CAN_FiRx 都由 2 个 32 位寄存器构成，即： CAN_FiR1 和 CAN_FiR2。 根据过滤器位宽和模式的不同设置， 这两个寄存器的功能也不尽相同。

## 6. CAN初始化基本步骤

本章，我们通过 KEY_UP 按键选择 CAN 的工作模式（正常模式/环回模式），然后通过 KEY0控制数据发送，并通过查询的办法，将接收到的数据显示在 LCD 模块上。如果是环回模式，我们用一个开发板即可测试。如果是正常模式，我们就需要 2 个探索者 STM32F4 开发板，并且将他们的 CAN 接口对接起来，然后一个开发板发送数据，另外一个开发板将接收到的数据显示在 LCD 模块上。

### 6.1 配置相关引脚的复用功能（AF9），使能 CAN 时钟

我们要用 CAN，第一步就要使能 CAN 的时钟， CAN 的时钟通过 APB1ENR 的第 25 位来设置。其次要设置 CAN 的相关引脚为复用输出， 这里我们需要设置 PA11（CAN1_RX）和 PA12 （CAN1_TX）为复用功能（AF9），并使能 PA 口的时钟。具体配置过程如下：

```c
GPIO_InitTypeDef GPIO_Initure;
__HAL_RCC_CAN1_CLK_ENABLE();  // 使能 CAN1 时钟
__HAL_RCC_GPIOA_CLK_ENABLE(); // 开启 GPIOA 时钟
GPIO_Initure.Pin=GPIO_PIN_11|GPIO_PIN_12; // PA11,12
GPIO_Initure.Mode=GPIO_MODE_AF_PP; // 推挽复用
GPIO_Initure.Pull=GPIO_PULLUP;     // 上拉
GPIO_Initure.Speed=GPIO_SPEED_FAST;// 快速
GPIO_Initure.Alternate=GPIO_AF9_CAN1;// 复用为 CAN1
HAL_GPIO_Init(GPIOA,&GPIO_Initure);  // 初始化
```

### 6.2 设置CAN工作模式及波特率

这一步通过先设置 CAN_MCR 寄存器的 INRQ 位，让 CAN 进入初始化模式，然后设置CAN_MCR 的其他相关控制位。再通过 CAN_BTR 设置波特率和工作模式（正常模式/环回模式）等信息。 最后设置 INRQ 为 0，退出初始化模式。

这一步通过先设置 CAN_MCR 寄存器的 INRQ 位，让 CAN 进入初始化模式，然后设置CAN_MCR 的其他相关控制位。再通过 CAN_BTR 设置波特率和工作模式（正常模式/环回模式）等信息。 最后设置 INRQ 为 0，退出初始化模式。

在库函数中，提供了函数 HAL_CAN_Init 用来初始化 CAN 的工作模式以及波特率， HAL_CAN_Init 函数体中，在初始化之前，会设置 CAN_MCR 寄存器的 INRQ 为 1 让其进入初始化模式，然后初始化 CAN_MCR 寄存器和 CRN_BTR 寄存器之后，会设置 CAN_MCR 寄存器的 INRQ 为 0 让其退出初始化模式。所以我们在调用这个函数的前后不需要再进行初始化模式设置。下面我们来看看 HAL_CAN_Init 函数的声明：

```c
HAL_StatusTypeDef HAL_CAN_Init(CAN_HandleTypeDef *hcan)
```

该函数入口参数只有 hcan 一个，为 CAN_HandleTypeDef 结构体指针类型，接下来我们看看结构体 CAN_HandleTypeDef 定义：

```c
typedef struct
{
    CAN_TypeDef *Instance;
    CAN_InitTypeDef Init;
    __IO HAL_CAN_StateTypeDef State;
    __IO uint32_t ErrorCode;
}CAN_HandleTypeDef;
```

该结构体除了 State ， ErrorCode 两个 HAL 库处理状态过程变量之外，只有两个成员变量需要我们外部设置。

第一个成员变量 Instance 位寄存器基地址，这里我们使用 CAN1，设置为 CAN1 即可。第二个成员变量 Init，它是 CAN_InitTypeDef 结构体类型，该结构体定义为：

```c
typedef struct
{
    uint32_t Prescaler;
    uint32_t Mode;
    uint32_t SyncJumpWidth;
    uint32_t TimeSeg1;
    uint32_t TimeSeg2;
    FunctionalState TimeTriggeredMode;
    FunctionalState AutoBusOff;
    FunctionalState AutoWakeUp;
    FunctionalState AutoRetransmission;
    FunctionalState ReceiveFifoLocked;
    FunctionalState TransmitFifoPriority;
} CAN_InitTypeDef;
```

这个结构体看起来成员变量比较多，实际上参数可以分为两类。前面 5 个参数是用来设置寄存器 CAN_BTR，用来设置模式以及波特率相关的参数，这在前面有讲解过，设置模式的参数是Mode，我们实验中用到回环模式CAN_MODE_LOOPBACK和常规模式CAN_MODE_NORMAL，大家还可以选择静默模式以及静默回环模式测试。其他设置波特率相关的参数 Prescaler， SyncJumpWidth;， TimeSeg1 和 TimeSeg2 分别用来设置波特率分频器，重新同步跳跃宽度以及时间段 1 和时间段 2 占用的时间单元数。后面 6 个成员变量用来设置寄存器 CAN_MCR，也就是设置 CAN 通信相关的控制位。

初始化示例：

```c
CAN_HandleTypeDef CAN1_Handler;  // CAN1 句柄
CAN1_Handler.Init.Prescaler=brp; //分频系数(Fdiv)为 brp+1
CAN1_Handler.Init.Mode=mode;     // 模式设置
CAN1_Handler.Init.SyncJumpWidth=tsjw;
//重新同步跳跃宽度(Tsjw)为 tsjw+1 个时间单位 CAN_SJW_1TQ~CAN_SJW_4TQ
CAN1_Handler.Init.TimeSeg1=tbs1;
//tbs1 范围 CAN_BS1_1TQ~CAN_BS1_16TQ
CAN1_Handler.Init.TimeSeg2=tbs2;
//tbs2 范围 CAN_BS2_1TQ~CAN_BS2_8TQ
CAN1_Handler.Init.TimeTriggeredMode=DISABLE; //非时间触发通信模式
CAN1_Handler.Init.AutoBusOff=DISABLE; //软件自动离线管理
CAN1_Handler.Init.AutoWakeUp=DISABLE;
//睡眠模式通过软件唤醒(清除 CAN->MCR 的 SLEEP 位)
CAN1_Handler.Init.AutoRetransmission=ENABLE; //禁止报文自动传送
CAN1_Handler.Init.ReceiveFifoLocked=DISABLE; //报文不锁定,新的覆盖旧的
CAN1_Handler.Init.TransmitFifoPriority=DISABLE; //优先级由报文标识符决定
if(HAL_CAN_Init(&CAN1_Handler)!=HAL_OK) //初始化
    return 1;
return 0;
```

HAL 库通用提供了 MSP 初始化回调函数， CAN 回调函数为：

```c
void HAL_CAN_MspInit(CAN_HandleTypeDef* hcan);
```

该回调函数一般用来编写时钟使能， IO 初始化以及 NVIC 等配置。

### 6.3 设置滤波器

本章，我们将使用滤波器组 0，并工作在 32 位标识符屏蔽位模式下。先设置 CAN_FMR的 FINIT 位，让过滤器组工作在初始化模式下，然后设置滤波器组 0 的工作模式以及标识符 ID和屏蔽位。最后激活滤波器，并退出滤波器初始化模式

在 HAL 库中，提供了函数 HAL_CAN_ConfigFilter 用来初始化 CAN 的滤波器相关参数。HAL_CAN_ConfigFilter 函数体中，在初始化滤波器之前，会设置 CAN_FMR 寄存器的 FINIT位为 1 让其进入初始化模式，然后初始化 CAN 滤波器相关的寄存器之后，会设置 CAN_FMR寄存器的 FINIT 位为 0 让其退出初始化模式。所以我们在调用这个函数的前后不需要再进行初始化模式设置。下面我们来看看 HAL_CAN_ConfigFilter 函数的声明：

```c
HAL_StatusTypeDef HAL_CAN_ConfigFilter(CAN_HandleTypeDef *hcan,
                                       CAN_FilterTypeDef *sFilterConfig)
```

该函数有 2 个入口参数，第一个入口参数 hcan 这里就不多讲了。接下来看看第二个入口参数 sFilterConfig，它是 CAN_FilterTypeDef 结构体指针类型，用来设置滤波器相关参数，结构体CAN_FilterTypeDef 定义为：

```c
typedef struct
{
    uint32_t FilterIdHigh;
    uint32_t FilterIdLow;
    uint32_t FilterMaskIdHigh;
    uint32_t FilterMaskIdLow;
    uint32_t FilterFIFOAssignment;
    uint32_t FilterBank;
    uint32_t FilterMode;
    uint32_t FilterScale;
    uint32_t FilterActivation;
    uint32_t SlaveStartFilterBank;
} CAN_FilterTypeDef;
```

结构体一共有 10 个成员变量，第 1 个至第 4 个是用来设置过滤器的 32 位 id 以及 32 位 mask id，分别通过 2 个 16 位来组合的，这个在前面有讲解过它们的意义。

第 5 个成员变量 FilterFIFOAssignment 用来设置 FIFO 和过滤器的关联关系，我们的实验是关联的过滤器 0 到 FIFO0，值为 CAN_RX_FIFO0。

第 6 个成员变量 FilterBank 用来设置初始化的过滤器组，取值范围为 0~13 或 0~27。第 7 个成员变量 FilterMode 用来设置过滤器组的模式，取值为标识符列表模式CAN_FILTERMODE_IDLIST 和标识符屏蔽位模式 CAN_FILTERMODE_IDMASK。

第 8 个成员变量 FilterScale 用来设置过滤器的位宽为 2 个 16 位 CAN_FILTERSCALE_16BIT 还是 1 个 32 位 CAN_FILTERSCALE_32BIT。

第 9 个成员变量 FilterActivation 就很明了了，用来激活该过滤器。第 10 个成员变量用来设置 CAN 起始存储区。

过滤器初始化参考实例代码：

```c
CAN_FilterTypeDef sFilterConfig;
/*##-2- Configure the CAN Filter ###########################################*/
sFilterConfig.FilterBank = 0;
sFilterConfig.FilterMode = CAN_FILTERMODE_IDMASK;
sFilterConfig.FilterScale = CAN_FILTERSCALE_32BIT;
sFilterConfig.FilterIdHigh = 0x0000;
sFilterConfig.FilterIdLow = 0x0000;
sFilterConfig.FilterMaskIdHigh = 0x0000;
sFilterConfig.FilterMaskIdLow = 0x0000;
sFilterConfig.FilterFIFOAssignment = CAN_RX_FIFO0;
sFilterConfig.FilterActivation = ENABLE;
sFilterConfig.SlaveStartFilterBank = 14;
if (HAL_CAN_ConfigFilter(&CAN1_Handler, &sFilterConfig) != HAL_OK)
{
    /* Filter configuration Error */
    while(1)
    { 
    }
}
```

### 6.4 发送消息

在初始化 CAN 相关参数以及过滤器之后，接下来就是发送和接收消息了。 HAL 库中提供了发送和接受消息的函数。发送消息的函数是：

```c
HAL_StatusTypeDef HAL_CAN_AddTxMessage(CAN_HandleTypeDef *hcan,
CAN_TxHeaderTypeDef *pHeader, uint8_t aData[], uint32_t *pTxMailbox)
```

这个函数比较好理解， 入口参数 hcan，为 CAN_HandleTypeDef 结构体指针类型， pHeader 为发送的指针， aData 是待发送数据， pTxMailbox 为发送邮箱指针。

接收消息的函数是：

```c
HAL_StatusTypeDef HAL_CAN_GetRxMessage(CAN_HandleTypeDef *hcan,
uint32_t RxFifo, CAN_RxHeaderTypeDef *pHeader, uint8_t aData[])
```

第一个入口参数为 CAN 句柄，第二个为 FIFO 号，然后是接收指针及数据存放的地址。我们接受之后，只需要读取 pHeader 便可获取接收数据和相关信息。
