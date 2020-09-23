User Manual
================

Keypad lock provide access by user password.

The open mode in simulated by switching on green led.

**To unlock keypad lock (Mode 1) or to switch the state (Mode 2):**

Input User Password: use UP and DOWN buttons to set the value, RIGHT and LEFT to move and MENU (central button) to confirm.

**To open settings:**

1. Fill User Password with '0' or left empty.

2. Input Master Password.

3. Chose option using buttons (UP, DOWN, MENU).


**Menu (settings) map**

=============  ==============================================
   Option                       Function
=============  ==============================================
Go back        Save changes and go back
Password       Change User Password and its length
Master code    Change Master Password and its length
Work mode      Change Mode and set 'Welcome delay'
Security       Change waiting delay, coefficient, number of failed attempts without waiting etc.
=============  ==============================================

**Default settings**

* User password: 2222

* Master password: 123456

* Menu code: 0000

* Mode: 1 (one-time opening)

* Welcome delay: 10s

* Minimal wait delay: 30s

* Coefficient: 2

* Number of attempts after which keypad lock start to be blocked: 3

**Tips**

1. Avoid changing both User and Master passwords in one 'session' if Keypad Lock works in High Secure mode. Change one password, go back to the User Password input page, then go to settings ones more and change another password.

2. Avoid using a lot of '0' in passwords. (Empty spaces are equal to '0').

3. Set Master password with max length (8 symbols).
