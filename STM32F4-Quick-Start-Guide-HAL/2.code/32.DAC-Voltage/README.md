# DAC-输出电压

使用 KEY1/KEY_UP 两个按键，控制 STM32 内部 DAC 的通道 1 输出电压大小，然后通过ADC1 的通道 5 采集 DAC 输出的电压，在 LCD 模块上面显示 ADC 采集到的电压值以及 DAC的设定输出电压值等信息。也可以通过 usmart 调用 dac_set_voltage 函数，来直接设置 DAC 输出电压。 LED0 闪烁， 提示程序运行。
