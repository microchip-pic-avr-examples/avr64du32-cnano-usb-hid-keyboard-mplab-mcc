/*
 * MAIN Generated Driver File
 *
 * @file main.c
 *
 * @defgroup main MAIN
 *
 * @brief This is the generated driver implementation file for the MAIN driver.
 *
 * @version MAIN Driver Version 1.0.0
 */

/*
� [2023] Microchip Technology Inc. and its subsidiaries.

    Subject to your compliance with these terms, you may use Microchip
    software and any derivatives exclusively with Microchip products.
    You are responsible for complying with 3rd party license terms
    applicable to your use of 3rd party software (including open source
    software) that may accompany Microchip software. SOFTWARE IS ?AS IS.?
    NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS
    SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT,
    MERCHANTABILITY, OR FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT
    WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY
    KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
    MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE
    FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP?S
    TOTAL LIABILITY ON ALL CLAIMS RELATED TO THE SOFTWARE WILL NOT
    EXCEED AMOUNT OF FEES, IF ANY, YOU PAID DIRECTLY TO MICROCHIP FOR
    THIS SOFTWARE.
 */
#include "mcc_generated_files/system/system.h"
/*
    Main application
 */

#include <xc.h>
#include <usb_core.h>
#include <usb_hid_keycodes.h>
#include "usb_hid_keyboard.h"

// Set up definition for amount of consecutive PIT interrupts
#define CONSECUTIVE_EQUAL_PIT_INTERRUPTS 5

// USB status variable
RETURN_CODE_t status;

// Interrupt variables
volatile uint8_t vbusFlag = false;
bool switchPressFlag = false;

// "Hello, welcome to the world of USB!" represented in HID Keyboard Usage IDs
uint8_t message[] = {
    HID_CAPS_LOCK, HID_H, HID_CAPS_LOCK, HID_E, HID_L, HID_L, HID_O, HID_COMMA, //Hello,
    HID_SPACEBAR, HID_W, HID_E, HID_L, HID_C, HID_O, HID_M, HID_E, //welcome
    HID_SPACEBAR, HID_T, HID_O, //to
    HID_SPACEBAR, HID_T, HID_H, HID_E, //the
    HID_SPACEBAR, HID_W, HID_O, HID_R, HID_L, HID_D, //world
    HID_SPACEBAR, HID_O, HID_F, //of
    HID_SPACEBAR, HID_CAPS_LOCK, HID_U, HID_S, HID_B, HID_CAPS_LOCK, HID_1, HID_SPACEBAR, //USB!
};

// Function declarations
void USB_HIDKeyPressHandler(void);
void SW0_Handler();
void PITInterruptRoutine(void);

int main(void)
{
    SYSTEM_Initialize();

    // USB HID callback and descriptor initialization
    USB_SOFCallbackRegister(USB_HIDKeyPressHandler);
    SW0_SetInterruptHandler(SW0_Handler);
    RTC_SetPITIsrCallback(PITInterruptRoutine);

    // Tracking-helper variable for VBUS
    uint8_t prevVbusState = 0;


    while (1)
    {
        // Check if VBUS was true last check, indicating that USB was connected
        if (prevVbusState == true)
        {
            // Handle USB Transfers
            status = USBDevice_Handle();
        }
        // Get current status of VBUS
        uint8_t currentVbusState = vbusFlag;
        // If changes to USB VBUS state
        if (currentVbusState != prevVbusState)
        {
            // If USB has been connected
            if (currentVbusState == true)
            {
                // Start USB operations
                status = USB_Start();
            }
            else
            {
                //Stop USB operations
                status = USB_Stop();
            }
            //Save state
            prevVbusState = currentVbusState;
        }
        // If USB error detected, blink LED indicating fault
        if (SUCCESS != status)
        {
            LED0_Toggle();
        }
    }
}

// Function that handles the up/down events of a keypress

void USB_HIDKeyPressHandler()
{
    // Variables to keep track of keypress states
    static volatile bool down = true;
    static volatile uint8_t i = 0;

    // Detects any keypresses
    if (switchPressFlag)
    {
        // Checks if the button has been pressed
        if (true == down)
        {
            if (SUCCESS == status)
            {
                // Used to make exclamation mark at the end of the message
                if ((message[i] == HID_1))
                {
                    status = USB_HIDKeyModifierDown(HID_MODIFIER_LEFT_SHIFT);
                }

                // Simulates a keypress by sending the keypress down event
                if (SUCCESS == status)
                {
                    status = USB_HIDKeyPressDown(message[i]);
                    down = false;
                }
            }
        }
        else
        {
            if (SUCCESS == status)
            {
                // Sends the keypress up event
                status = USB_HIDKeyPressUp(message[i]);
                // Releases the shift key modifier if enabled
                USB_HIDKeyModifierUp(HID_MODIFIER_LEFT_SHIFT);

                down = true;
                i++;
            }
            // End of message
            if (i >= sizeof (message))
            {
                i = 0;
                switchPressFlag = false;
            }
        }

    }
}

// Interrupt handler for the switch (button) press event

void SW0_Handler()
{
    switchPressFlag = true;
}


// Interrupt routine used to check VBUS status using the Real-Time Counter Periodic Interrupt Timer and Analog Comparator

void PITInterruptRoutine(void)
{
    // Helper variables for PIT and AC
    static volatile uint8_t PIT_Counter = 0;
    static volatile uint8_t AC_prevState = 0;

    uint8_t AC_currentState = AC0_Read(); // Saves the current state of the AC status register ('1' means above threshold)

    // Makes sure that the AC state has been the same for a certain number of PIT interrupts
    if (PIT_Counter == CONSECUTIVE_EQUAL_PIT_INTERRUPTS)
    {
        if (AC_currentState) // When CMPSTATE is high, the AC output is high which means the VBUS is above the threshold and the pull-up on data+ should be enabled.
        {
            vbusFlag = true;
        }
        else
        {
            vbusFlag = false;
        }
        PIT_Counter++; // Increment the counter to not run start/stop multiple times without an actual state change
    }
        // Increments a counter if the AC has been in the same state for a certain number of PIT interrupts
    else if (AC_currentState == AC_prevState)
    {
        if (PIT_Counter <= CONSECUTIVE_EQUAL_PIT_INTERRUPTS + 1)
        {
            PIT_Counter++;
        }
        else
        {
            ; // Stops counting if the AC has been in the same state for a while.
        }
    }
    else
    {
        PIT_Counter = 0; // Resets the PIT counter if a state change has occurred.
    }
    AC_prevState = AC_currentState;
}

