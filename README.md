<!-- Please do not change this logo with link -->

[![MCHP](images/microchip.png)](https://www.microchip.com)

# USB Human Interface Device (HID) Communication Demo Using On-Board Button
This example shows how the HID class can be used on an AVR DU microcontroller with the AVR64DU32 Curiosity Nano Board to simulate a set of key-presses by pushing the physical button on the Curiosity Nano Board.

## Related Documentation
- [AVR® DU Family Product Page](https://www.microchip.com/en-us/products/microcontrollers-and-microprocessors/8-bit-mcus/avr-mcus/avr-du?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_AVR-DU&utm_content=avr64du32-cnano-usb-hid-keyboard-mplab-mcc-github&utm_bu=MCU08)

### USB Specifications

- [USB Human Interface Device (HID) Specification](https://www.usb.org/document-library/device-class-definition-hid-111)
- [Device Class Definition for HID 1.11](https://usb.org/document-library/hid-usage-tables-15)
- [USB 2.0 Specification](https://www.usb.org/document-library/usb-20-specification)

## Software Used

- [MPLAB® X IDE 6.20.0 or newer](https://www.microchip.com/en-us/tools-resources/develop/mplab-x-ide?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_AVR-DU&utm_content=avr64du32-cnano-usb-hid-keyboard-mplab-mcc-github&utm_bu=MCU08)
- [MPLAB® XC8 2.46.0 or newer compiler](https://www.microchip.com/en-us/tools-resources/develop/mplab-xc-compilers?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_AVR-DU&utm_content=avr64du32-cnano-usb-hid-keyboard-mplab-mcc-github&utm_bu=MCU08)
- [MPLAB® Code Configurator (MCC) plugin 5.5.0 or newer](https://www.microchip.com/en-us/tools-resources/configure/mplab-code-configurator?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_AVR-DU&utm_content=avr64du32-cnano-usb-hid-keyboard-mplab-mcc-github&utm_bu=MCU08)

## Hardware Used

- [AVR64DU32 Curiosity Nano (EV59F82A)](https://www.microchip.com/en-us/development-tool/EV59F82A?utm_source=GitHub&utm_medium=TextLink&utm_campaign=MCU8_AVR-DU&utm_content=avr64du32-cnano-usb-hid-keyboard-mplab-mcc-github&utm_bu=MCU08)


## Setup

### Physical Setup

The AVR DU Curiosity Nano development board has two USB-C ports. The port labeled "Debugger" is used to program the device with the example code. This port must be connected to the PC when uploading the code to the device. After the programming is done, the cable connected to the "Debugger" port may be disconnected if you wish, but it will not affect the example if it remains plugged in. To test the device after the code has been uploaded, the port "Target" needs to be connected to the PC while testing.

The Curiosity Nano has an Adjustable Target Regulator that regulates voltage to 3.3V by default. Since the USB peripheral in the AVR64DU32 needs to run at 3.3V, it either needs a 3.3V supply connected on the VUSB pin or a 5.0V supply on VDD with the internal USB Voltage Regulator active. With the Curiosity Nano default settings, the VBUS pinheader on the board must be connected with the included jumper to supply 3.3V directly to VUSB. 

### MCC Setup

This section shows how the example is set up in MCC.

#### USB General Settings
The configuration is set so that the AVR DU will identify as an HID device on the highest level. The Keyboard protocol is selected to allow the device to mimic a keyboard.
<p><img src="images/mcc_usb_general_settings.jpg" width="350"/></p>

#### USB Device Descriptors
The Device Descriptors can be left as default.
<p><img src="images/mcc_usb_device_descriptors.jpg" width="350"/></p>

#### USB Interfaces
For this example project, an HID interface is needed. It does not need any modification beyond being added.
<p><img src="images/mcc_usb_interfaces.jpg" width="400"/></p>

#### USB HID Interface
The interface created in the last step does not need any modification, and is best left to default.
<p><img src="images/mcc_usb_interface_HID.jpg" width="500"/></p>

#### USB Configurations
Go to the **Configurations** tab in the USB Device Stack and change the country code. This example will use the US keyboard.
<p><img src="images/usb_hid_descriptors.jpg" width="500"/></p>

### Device Clock Setup
This example is configured to run on 12 MHz. The minimum oscillator frequency available for USB on the AVR DU is 12MHz.
<p><img src="images/mcc_clock_control.jpg" width="350"/></p>

### Device Pin-out Setup
Three pins are used for this example, with VBUS on PC3 set as input with "Digital Input Buffer disabled", SW0 on PF6 set to input with "Sense Falling Edge", and LED0 on PF2 set to output with "Interrupt disabled but input buffer enabled".
<p><img src="images/mcc_pinout.jpg" width="500"/></p>
<p><img src="images/mcc_pinout_locks.jpg" width="500"/></p>

### USB Voltage Detection Setup
To ensure that the USB peripheral only tries to attach to the bus when plugging in the Curiosity Nano to the PC, the Analog Comparator will periodically check if the voltage is within the acceptable range for VBUS.

#### Voltage Reference Setup
In the VREF peripheral, the Voltage Reference is set to 2.048V.
<p><img src="images/mcc_voltage_reference.jpg" width="500"/></p>

#### Analog Comparator (AC) Setup

##### Analog Comparator Hardware Settings
The Analog Comparator must be enabled by toggling Enable under Hardware Settings.

The positive input is set to the *USB DETECT* pin on the curiosity nano which is connected to *Positive Pin 4* on the AC by default. The detection threshold is set using the Digital-to-Analog Comparator (DAC) reference (DACREF) which is selected as the negative input to the AC.

To calculate the DACREF value we can use the formula from the datasheet:

*V_DACREF = (DACREF / 256) * V_REF*

If we know the threshold we want, we can use the built in-feature of MCC to do it for us. We want to use the value 0.32V as a threshold due to the voltage divider implemented on the Curiosity Nano. Inputting this value into the *Requested Voltage* field will populate the DACREF value.

Refer to the [AVR64DU32 Curiosity Nano User Guide](https://ww1.microchip.com/downloads/aemDocuments/documents/MCU08/ProductDocuments/UserGuides/AVR64DU32-Curiosity-Nano-UserGuide-DS50003671.pdf) for more information on how the *USB DETECT* is implemented on the Curiosity Nano.

<p><img src="images/mcc_ac_hardware_settings.jpg" width="600"/></p>

#### Real-Time Counter Setup

##### Real-Time Counter Period Settings
On the Hardware settings, the "Enable RTC" can be disabled (Enabled by default), and note down the RTC Clock Source Selection, as it will impact the Period Selection value needed for the Periodic Interrupt Timer.
<p><img src="images/mcc_rtc_per.jpg" width="350"/></p>

##### Real-Time Counter Periodic Interrupt Control Settings
The Periodic Interrupt Timer (PIT) is enabled for this example, and the period selection is set to RTC Clock Cycles 1024. This value must be changed according to the clock source selection.
<p><img src="images/mcc_rtc_pitctrla.jpg" width="350"/></p>

##### Real-Time Counter Periodic Interrupt Interrupt Control Settings
The Periodic Interrupt Timer's Interrupt Flag is also enabled to allow for an interrupt routine.
<p><img src="images/mcc_rtc_pitintctrl.jpg" width="350"/></p>

#### Interrupt Manager Setup
Enable global interrupts in the Interrupt Manager under System Firmware to recognize the Button and RTC interrupts.
<p><img src="images/mcc_isr_interrupt_settings.jpg" width="600"/></p>

## Operation

This example can be tested by following these steps:

1. Upload code by connecting the hardware as explained in "Physical Setup".
2. Connect the PC to the "Target" plug on the Curiosity Nano Board by using a USB-C cable.
3. Open any program that allows for text input, and press the button "SW0-PF6" on the Curiosity Nano-board.

## Summary

This example has shown how the USB HID class can be used on an AVR DU to simulate a set of key-presses using only the on-board button on the Curiosity Nano kit.
