Abstract

The goal of this project was to design and build an intelligent faucet using the M68HC11EVB as the microcontroller circuit. It was able to find the hottest and coldest water temperatures, and allowed the user to program desired temperatures and flow rates via menus and prompts on an LCD and keypad while the system ran. Additional features included a keymap programmable keypad, an LCD display, and interrupt driven foreground and background programs. The project worked succesfully.

Introduction

Hardware

Software

The software and flowchart for the final project is located in Appendix A. It was written modularly, and 18 subroutine functions supplimented the main function. There were also five header files to suppliment the main program.

At the start of the main function, the variables are initialized, and the LCD display is initialized by sending four control codes to it that set the display modes. Then the title screen is shown by calling the title() function. It then waits for a key to be pressed. After that, the background interrupt program is started, which updates a portion of the screen that displays the temperature and flow rate,  and controls the motors if necessary. Then the temperature range is found using the range() function. The range() function tells the background program to leave the motors alone, but keep displaying the temperature, as it opens and closes the respective valves for high and low temperatures. When it has found the high and low temperatures, and stored them into the global variables maxt and mint, it displays the temperature range at the bottom of the LCD screen, and closes the valves. Then the function prints "(paused)" and waits for a key to be pressed. Next the menu is displayed, with the high and low temperatures, desired flow rate, desired temperature, actual flow rate, and actual temperature in the menu. The actual flow rate and temperature are being updated in real time in the corner at this point, while the menu is waiting for a key to be pressed. The menu looks like  this:

   1) set flow (00%)	2) range   F=00% T=23o
3) set temp (00o)	Tmax=45o Tmin=09o        

	The temperature and flow rate will continue to be updated 	at all times regardless of what the foreground menu program is doing. The desired temperatures and flow rates are stored in global variables Dt and Dflow, and passed on the to background program, where the motor control is done,  instantaneously. They are displayed in parenthesis in the menu.

	When the selection (1), set temp, is chosen, the print() function displays a prompt asking for the temperature to be typed in, and scan() reads in the numbers, 	and converts it to an integer. The value is stored immediately into Dt, the desired temperature global variable, where it is availiable to the background motor control program and other functions. If selection (2), set flow, is chosen, the program asks for desired flow and stores the result into the Dflow global variable in exactly the same way.

	If selection (3) is chosen, the range() funcfion is called again, and new maxt and mint  variables are defined in the same way as described when the program is started. 

	Selection (4) will again display the title screen, but this time, the background program is active, and while the main screen is paused, in the corner, the temperature and flow rate are being updated and the motors are being adjusted in real time, in the background.

	The background() function was the motor control and display program. It was activiated by a timer interrupt generated on the 68HC11EVB. Depending on the global variables mode and upd, the function would update the motor position and/or update the display. If upd were 1, then the function would be active, and the corner of the display with the current temperature and flow rate would be updated. If  upd were 0, then the function would not do anything. This variable was necessary because if the background function were always active, the background would interfere with the foreground, and this led to messy results on the display screen. The mode variable told the main program if the background was to adjust the motors. This was necessary because if the range() function was running, where a valve had to be open, the background program would try to close it.

	The motor control algorithm was designed to take as little time as possible, so that it would not slow down the foreground program excessively. The processor load from th background program was evident even with this algorithm. The algorithm was a step control that moved one or both of the motors four steps in directions determined by the current flow rate and temperature. If the temperature and flow rate approached their target, the background program would reduce its update rate to once per second. This was done to prevent oscillation.

	The actual stepper motor control functions and thermocouple functions were taken directly from their respective labs. The motor control functions were modifiied by adding step count variables to keep track of the step count.

	Because of  course rules and regulations, the LCD and keypad were connected to the ports and not to the 68HC11 bus as originally anticipated. This meant that the LCD's bus control signals which needed to be emulated by the program. This was a very tedious task, because the microcontroller ran at a much higher speed than the LCD could read it, and after hours of troubleshooting, it was found that the LCD driver functions needed wait states. This was the purpose of the function waitstate().

