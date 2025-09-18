# DAC-PWM模拟

我们将设计一个 8 位的 DAC， 使用按键（或 USMART）控制 STM32F407 的 PWM 输出（占空比），从而控制 PWM DAC 的输出电压。为了得知 PWM 的输出电压， 通过 ADC1 的通道 1 采集 PWM DAC 的输出电压，并在 LCD 模块上面显示 ADC 获取到的电压值以及 PWM DAC 的设定输出电压值等信息。
