# FATFS-SDcard

开机的时候先初始化 SD 卡，初始化成功之后，注册两个磁盘：一个给 SD 卡用，一个给 SPI FLASH 用，之所以把 SPI FLASH 也当成磁盘来用，一方面是为了演示大容量的 SPI Flash 也可以用 FATFS 管理， 说明 FATFS 的灵活性；另一方面可以展示 FATFS 方式比原来直接按地址管理数据便利性， 使板载 SPI Flash 的使用更具灵活性。挂载成功后获取SD 卡的容量和剩余空间，并显示在 LCD 模块上，最后定义 USMART 输入指令进行各项测试。本实验通过 LED0 指示程序运行状态。
