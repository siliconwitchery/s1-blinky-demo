# S1 ECG Reference Kit

**Features:**

- Single chip frontend based on the [AD8233](https://www.analog.com/en/products/ad8233.html)
- S1 Module with Bluetooth 5.2 and FPGA
- Integrated lithium battery
- Ultra low power auto-sleep down to 175uA
- Open source design

**Applications:**

- Learning and teaching around heart health
- Research of ML based ECG algorithms
- Wireless ECG data logging
- New product development

A powerful analog frontend combined with the S1 Module makes this an all-in-one kit to deploy and test algorithms with ease. Be that for learning, research, or as the starting point for your next product.


<br>

![S1 ECG Board](images/s1-ecg-kit-front.png)
![S1 ECG Board](images/s1-ecg-kit-back.png)

<br>

## Getting started

The S1 ECG reference kit uses a three probe measurement to determine heart activity. By holding the board with both hands, the device automatically wakes from sleep, and begins calibrating the amplifier sensitivity. After a few seconds, you will see your pulse displayed on the LED bar graph. Letting go of the terminals puts the system back into sleep mode.

The base firmware is highly expandable and a perfect starting point for new designs. To get started, clone this repository:

``` bash
git clone --recurse-submodules https://github.com/siliconwitchery/s1-ecg-demo.git
```

You may need to follow the steps [here](https://github.com/siliconwitchery/s1-sdk) in order to set up the S1 SDK if you haven't already.

To build the project, run make:

``` bash
make -C firmware/
make flash -C firmware/
```

In order to flash the board. You will need a suitable SWD flasher such as a [JLink Edu Mini](https://www.digikey.se/product-detail/en/segger-microcontroller-systems/8-08-91-J-LINK-EDU-MINI/899-1061-ND/7387472), [nRF devkit](https://infocenter.nordicsemi.com/topic/ug_nrf52832_dk/UG/nrf52_DK/hw_debug_out.html) or [Blackmagic probe](https://github.com/blacksphere/blackmagic/wiki).

You will also need a Tag Connect TC2030-CTX [6 pin cable](https://www.digikey.se/product-detail/en/tag-connect-llc/TC2030-CTX/TC2030-CTX-ND/5023324).

To learn more about the S1 Module. Visit our [documentation center](https://docs.siliconwitchery.com).