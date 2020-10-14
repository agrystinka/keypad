# GL BaseCamp Final Project
# Keypad lock
### Based on Global Logic Starter Kit with STM32F4DISCOVERY

Provides access by password. Has 2 modes of work, user settings menu.

Has protection from brute force attack and protection from settings reset.


### Build & Flash

1. If device was used before, firstly do: ``` make TARGET=first_flash PROFILE=release clean target flash ```

It erase flash memory sectors needed for keypad lock. You will see message on LCD when it will have finished work (it takes few seconds).

2. Flash (with keypad lock firmware): ``` make PROFILE=release clean target flash ```

#### Notes for developers

While code debugging it is better to use: ``` PROFILE=debug ```

To have more debug options and to use gdb: ``` make PROFILE=debug SEMIHOSTING=1 clean target flash gdb```

If you want to use already written code for debug through semihosting, define  ```SEMIHOSTING``` as ```1``` in  ```keypad.h```.

### Usage

**To unlock keypad lock (Mode 1) or to switch the state (Mode 2):**

Input User Password: use UP and DOWN buttons to set the value, RIGHT and LEFT to move and MENU (central button) to confirm.

**To open settings:**

1. Fill User Password with '0' or left empty.

2. Input Master Password.

3. Chose option using buttons (UP, DOWN, MENU).

*More in documentation*

### [Toolchain](README.rst)
