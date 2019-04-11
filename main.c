/*rpi 20.2300.13 by Peter Ungsunan, 12/93*/
#include <H11REG.h>	/*Header Files*/
#include <stdio.h>
#include <string.h>
#include <interrupt.h>
#include <math.h>
void dwrt(char d);		/*Function Declarations */
void cwrt(int c);
void print(char txt[40]);
int keypad();
void waitstate();
void delay(int k);
int scan();
void act();
void deact();
void background();
void range();
void show(int c);
int temp();
void motor1cw();
void motor1ccw();
void motor2cw();
void motor2ccw();
void title();
extern int TimerRate;		/* This is to vary the Timer rate */
/* Variable & constant declarations */
int DeadB=3,inc=1, upd, Maxsteps = 290,MWait, t, Dt, Dflow, mode, maxt, mint,f1,f2; float flow;

void main()
 {
	int menu;
	flow = 0;		/* Initializing variables */
	t = 0;			/* temperature */
	Dt = 0;		/* desired temperature */
	Dflow = 0;		/* desired flow rate */
	f1=0;			/* step count of motor 1 */
	f2=0;			/* step count of motor 2 */
	mode = 0;		/* this tells the background interrupt function if OK to touch motors */
	upd=0;		/* tells the background function whether or not  to touch the display */
	TimerRate=4;		/* set the rate of speed to 4 motor adjustments per second */
	MWait=300;		/* and screen updates */
	H11DDRC=0xFF;	/* set the LCD port to all output */
cwrt(0x01);     /*lcd clear display*/ 
cwrt(0x38);     /*lcd function set*/
 cwrt(0x0F);     /*lcd display & cursor set*/
 cwrt(0x06);     /*lcd entry mode set*/
 title(); 	/*title screen */
ATTACH(background,13); 		/* start interrupt */
BeginTimerInterrupt();
	cwrt(0x01);	/* clear LCD */
	range();	/* find high & low temps */
	while(1)	/* main menu loop */
		{
			cwrt(0x01);     /*lcd clear display*/
			print("1) set temp($"); 	   /* this prints out the menu */
			show(Dt);		    /* & desired parameters */
			print("ß)    3) range            $");
			print("2) set flow($");
			show(Dflow);
			print("%) Tmax:$");
			show(maxt);
			print("ß Tmin:$");
			show(mint);
			print("ß$");

			act();			/* activates the background program */
			menu=keypad();	/* read the keypad */
/*menu choices: (1) change Desired Temp (2) change desired flow
						(3) find temp range again (4) title screen */
			cwrt(0x01);     /*lcd clear display*/
			switch(menu) 
				{
					case 1:
print("Input 0ßC to 99ßC temperature:$"); 
/* change desired temp*/
cwrt(128+40);     /*lcd lf&cr*/
						act();		  /* activate background */
						Dt=scan();	/* read desired temp */
						break;
					case 2:
print("Input 00% to 99% flow rate:$"); 
/* change desired temp*/
cwrt(128+40);     /*lcd lf&cr*/
						act();		/* activate background */
						Dflow=scan(); 	/* read Desired flow */
						mode=1;	/* activate motors */
						break;
					case 3:
						range();	/* find temp range */
						break;
					case 4:
						title();		/* title screen */
						break;
				}
		}
 }









void dwrt(char d)	/* write text to the display */
 {
	deact();	
	/*deactivate background program to avoid interference from it*/
	H11PORTC=d; /* send character */
/* handshaking */
	H11PORTA=0x18;  /*E and RS on; RS=A4=pin 30; E=A3=pin31 */
	waitstate();	/*waitstate */
	H11PORTA=0x10;  /*E low*/
	waitstate();
 }
void cwrt(int c)		/* send control character to display */
 {							/* ie. clear screen */
	deact();	
/* deactivate background program to avoid interference from it*/
	H11PORTC=c;
/* handshaking */
	H11PORTA=0x08;
	waitstate();
	H11PORTA=0x00;
	waitstate();
 }
void print(char txt[40])	/* prints out strings */
 {
	int i, stlen = 0;           	 /*find string length*/
	while(txt[stlen]!='$')	/* $ is end of message character  */
		stlen++;
	for(i=0; i<stlen; i++)	/* print out the string to LCD */
			dwrt(txt[i]);
 }
int keypad()				/* reads keypad */
 {  
	int tmp,j;
	tmp=60;					/* the value the keypad sends when no key is pressed */
	while(tmp==60)		/* wait for a key */
			tmp=H11PORTD&60;
	for(j=0;j<3;j++)		/* debouncing pause */
			delay(1000);
		return tmp>>2;
 }
void waitstate()				/* artificial wait state because the 68HC11 is too fast for the LCD controller */
 {
	int ws;					
	for (ws=0; ws<100; ws++);
 }
void delay(int c)			/* general purpose variable length delay  for the motors, etc */
 {
	int de;
	for (de=0; de<c; de++);
 }








int scan()						/* reads in two digits from the keypad */
 {  									/* & outputs the resulting # */
	int a,b,j;
	a=keypad();
	cwrt(128+40);
	dwrt(a+48);
	b=keypad();
	cwrt(128+41);
	dwrt(b+48);
	print("        (paused)$");
	j=keypad();
	return a*10+b;
 }
void act()						/* activates the background program */
 {
 upd=1;
 }

void deact()					/* deactivates the background program */
 {
	upd=0;
 }








































void background()		/* the background interrupt function */
 {									/* it updates the display if necessary  */
	int t;						/* and adjusts the motors if necessary */
	if (upd)
		{
			cwrt(128+35);	/* update the display */
			print("T:  ß$");
			cwrt(128+29);
			print("F:  %$");
t=(int)(temp()*.305405405); flow=100*((float)(f1+f2)/(float)(Maxsteps*2));
			cwrt(128+31);
			show((int)(flow));
			cwrt(128+37);
			show(t);

/*update motor algorithm if inthe correct mode (active mode) */
if ((mode) && (Dflow!=(int)flow) || (abs(Dt-t)>DeadB) && (mode)) 
					if ((t< Dt) && ((int)flow< Dflow))
						if (f2<Maxsteps)
							motor2ccw();
if ((t> Dt) && ((int)flow< Dflow)) if (f1<Maxsteps)
							motor1ccw();
if ((t< Dt) && ((int)flow> Dflow)) if (f2>0)
							motor2cw();
if ((t< Dt) && ((int)flow> Dflow)) if (f1>0) 
							motor1cw();
if ((t<Dt) && ((int)flow== Dflow)) {
							if (f1>0) 
								motor1cw();
							if (f2<Maxsteps)
								motor2ccw();
						}
					if (((int)flow> Dflow) && (t<=Dt))
						if (f2>0)
							motor2cw();
					if (((int)flow> Dflow) && (t>Dt))
						if (f1>0)
							motor1cw();
					if ((t>Dt) && ((int)flow== Dflow))
						{
							if (f2>0)
								motor2cw();
							if (f1<Maxsteps)
								motor1ccw();
						}
					/* adjusts the update rate if the target flow is close */
					/* this is a type of derivative adjustment */
					if ((int)flow==Dflow)
							TimerRate=1;
						else
							TimerRate=4;
				}
		}
	act(); /*reactivate the background program */
	EndTimerInterrupt();
 }







void show(int c) 				/*converts to BCD & displays it*/
{
	int a,b;
a=(int)((c/10)+48); 	    /* a is the first upper digit from  two digits of c in ASCII*/ 
b=(int)((c-(a-48)*10)+48);    /* b is the second digit from two digits of c */
dwrt(a);
	dwrt(b);
}
int temp() /* reads one  ADC from the thermocouple*/ 
 {
	H11ADCTL=0X01;
	while (!(H11ADCTL&0x80))
		return H11ADR1;
 }
void range()				/* finds the high and low temperatures */
 {
	int j;
	mode=0;	/* dont let the background program fight this one trying to open the motors*/
	print("Finding temperature range.$");
	cwrt(128+40);     /*lcd lf&cr*/
	for(j=f1;j>0;j--)				/* closing motor 1*/
			motor1cw();
	for(j=f2;j<Maxsteps;j++)			/* opening motor 2 */
		motor2ccw();
	for (j=0;< 10;. j++) 			/* wait for temp to stabalize */
	delay(10000);
maxt=(int)(temp()*.305405405); cwrt(128+40); /* read  max t */
	print("Max T: $");			/* print it */
	show(maxt);
	act();									/* activate background display*/
	for(j=Maxsteps;j>0;j--)			/* same process as above, but for other motor */
			motor2cw();
for(j=f1;j< Maxsteps;j++) motor1ccw();
	for(j=0; j<10; j++)
		delay(10000); mint=(int)(temp()*.305405405);; cwrt(128+50);
	print("   Min T: $");
	show(mint);
	dwrt('-');
	for(j=0;j< Maxsteps;j++)
			motor1cw();
	cwrt(128+63);
	print("   (paused)$");			
	mode=1;							/* reactivate background motor program */
	act();									
	j=keypad();	
	act();									/* press a key to continue */
 }

















void motor1cw()					/* moves the cold motor clockwise */
 {
	int i;
	deact();							/* deactivate any interfering interrupt functions */
	for(i=0; i< inc; i++)
		{
			f1--;						/* step count */
			H11PORTB=0x55;		/* step */
			delay(MWait);		/* motor delay */
			H11PORTB=0x59;		/* etc...*/
			delay(MWait);
			H11PORTB=0x5A;
			delay(MWait);
			H11PORTB=0x56;
			delay(MWait);
			H11PORTB=0x55;
			delay(MWait);
		}
	act();								/* reactivate background motor program */
 }
void motor1ccw()				/* moves the cold motor counterclockwise */
 {											/* same as above but ccw */
	int i;
	deact();
	for(i=0; i< inc; i++)
		{
			f1++;
			flow = flow+1/(Maxsteps*2)*100;
			H11PORTB=0x55;
			delay(MWait);
			H11PORTB=0x56;
			delay(MWait);
			H11PORTB=0x5A;
			delay(MWait);
			H11PORTB=0x59;
			delay(MWait);
			H11PORTB=0x55;
			delay(MWait);
	}
	act();
 }
void motor2cw()				/* moves the hot motor clockwise */
 {										/* same as above but motor 2 cw */
	int i;
	deact();
	for(i=0; i< inc; i++)
		{
			f2--;
flow = flow-1/(Maxsteps*2)*100; H11PORTB=0x55;
			delay(MWait);
			H11PORTB=0x95;
			delay(MWait);
			H11PORTB=0xA5;
			delay(MWait);
			H11PORTB=0x65;
			delay(MWait);
			H11PORTB=0x55;
			delay(MWait);
		}
	act();
 }

void motor2ccw()			/* moves the hot motor counterclockwise */
 {										/* same as above, but motor 2 ccw */
	int i;
	deact();
	for(i=0; i< inc; i++)
		{
			f2++;
flow = flow+1/(Maxsteps*2)*100; H11PORTB=0x55;
			delay(MWait);
			H11PORTB=0x65;
			delay(MWait);
			H11PORTB=0xA5;
			delay(MWait);
			H11PORTB=0x95;
			delay(MWait);
			H11PORTB=0x55;
			delay(MWait);
		}
	act();
 }
void title()					/* title screen & credits*/
 {
	int j;
print("Embedded Control Lab Final Project.$"); 
cwrt(128+40);     /*lcd lf&cr*/ 
	print("-Peter Ungsunan, Judy Parks.$"); 
	j=keypad();
 }
