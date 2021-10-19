# S1 Blinky Demo

Our most basic example, running on the [S1 module](https://www.siliconwitchery.com/module), and the [S1 Popout Board](https://github.com/siliconwitchery/s1-popout-board).

It's a perfect starting point for your own projects, and demonstrates how to build some Verilog and C code for a minimal running setup.

![S1 Popout Board Blinky Example](images/s1-popout-board-blinky.gif)

## Building the project

Clone this repository

``` bash
git clone --recurse-submodules https://github.com/siliconwitchery/s1-blinky-demo.git
cd s1-blinky-demo
```

Everything can be built with Make. Though you'll need to install some tools first. 

Check out the instructions [here](https://github.com/siliconwitchery/s1-sdk/blob/main/README.md#setting-up-the-tools).

You can then build the project. Be sure to include the path to your NRF SDK folder.

``` bash
make build-verilog NRF_SDK_PATH=${HOME}/nRF5_SDK
make flash NRF_SDK_PATH=${HOME}/nRF5_SDK
```

The first `make` command will build the Verilog project, and convert the binary file into a .h file. This .h file is used in the nRF project and a state machine flashes the binary to the FPGA once the module boots.

In your application, you could download this binary over bluetooth rather than permanently storing it in the nRF memory.

The second `make` command builds the nRF code, and flashes the module using a JLink debugger. If you don't have a JLink, you can also use an [nRF52 devkit](https://www.nordicsemi.com/Products/Development-hardware/nrf52-dk) and a 10pin debug cable to flash your board. Or modify the script to use any debugger of your choice.

![S1 Flashing using nRF Development Kit](images/s1-nrfdk-wiring.jpg)

## Creating your own project

This example is a great starting point for your own project.

Here's an overview of the project structure and how it works.

- `s1-sdk` - This folder is our SDK and features all the API to interact with the flash memory, PMIC and FPGA control lines. It's included as a git submodule, so you can do the same, and update it as we update the features within. It also includes the core makefile `s1.mk` as well as the pin configuration for the FPGA. These are needed for both the Verilog build process, and nRF build process.

- `Makefile` - This is your makefile where you can include your application .c/.h files, settings, as well as any other tasks you may want. For example, here we demonstrate how to create the `build-verilog` task. Just be sure to include s1.mk when creating your own.

- `main.c` - This is your own application code. In this example, we demonstrate how to create a state machine that boots the module, flashes the on board flash, and starts the FPGA. Have a read through it, and you should begin to understand how the module can be configured for different modes.

- `fpga_binfile.h` - This is the compiled FPGA binary which is converted into a header file. You could instead of hardcoding this into your application, transfer it over bluetooth and save it onto the Flash chip. In this simple example, we've just hardcoded it.

- `fpga_blinky.v` - This is your Verilog project. In this example, we show you how to configure the internal oscillator to blink an LED on the Popout Board. 

- `sdk_config.h` - This is the nRF SDK configuration file. It can include a lot of settings and determines what peripherals and Bluetooth settings are configured on the nRF52 chip. It's best to read this in conjunction with the nRF SDK documentation and look at the examples in the nRF SDK itself where you'll find separate sdk_config.h files for each example.

- `.vscode` - These are some handy scripts for those of you who like to use VSCode. You can configure build tasks, debug launches and intellisense from within the templates.

That's it. As always, if you have any questions, or if something is unclear. Feel free to create an [issue](https://github.com/siliconwitchery/s1-blinky-demo/issues) and we'll try our best to improve things 🌈