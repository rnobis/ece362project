/***********************************************************************
; ECE 362 - Mini-Project
;***********************************************************************
;	 	   		  		
; Completed by: Ryan Nobis, Chris Liow, Tom Doddridge, Kyle 
;
;                             
;***********************************************************************
;
;
;***********************************************************************/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

// All funtions after main should be initialiezed here
char inchar(void);
void outchar(char x);
void fdisp();
void sdisp();
void shiftout(void);
void lcdwait(void);
void send_byte(char ch);
void send_i(char ch);
void chgline(char ch);
void print_c(char ch);
void pmsglcd(char ch[]);

//  Variable declarations  	   			 		  			 		       
int redpb	= 0;  // red pushbutton flag
int bluepb	= 0;  // blue pushbutton flag
int yellpb	= 0;  // yellow pushbutton flag
int greenpb	= 0;  // green pushbutton flag
int prevpb	= 0;  // previous pushbutton state
int runstp	= 0;  // game run/stop flag
int onesec 	= 0;  //  one second flag
int tenths	= 0;  // tenth of a second flag
int onecnt	= 0;  // ONECNT (variable)
int tencnt	= 0;  // TENCNT (variable)

//;LCD COMMUNICATION BIT MASKS
int RS = 0x04;     //;RS pin mask (PTT[2])
int RW = 0x08;     //;R/W pin mask (PTT[3])
int LCDCLK  = 0x10;     //;LCD EN/CLK pin mask (PTT[4])

//;LCD INSTRUCTION CHARACTERS
int LCDON = 0x0F;     //;LCD initialization command
int LCDCLR = 0x01;     //;LCD clear display command
int TWOLINE = 0x38;     //;LCD 2-line enable command

int CURMOV = 0xFE;     //;LCD cursor move instruction

int LINE1 = 0x80;     //;LCD line 1 cursor position
int LINE2 = 0xC0;     //;LCD line 2 cursor position

	 	   		
/***********************************************************************
Initializations
***********************************************************************/
void  initializations(void) {

//; Set the PLL speed (bus clock = 24 MHz)
  CLKSEL = CLKSEL & 0x80; //; disengage PLL from system
  PLLCTL = PLLCTL | 0x40; //; turn on PLL
  SYNR = 0x02;            //; set PLL multiplier
  REFDV = 0;              //; set PLL divider
  while (!(CRGFLG & 0x08)){  }
  CLKSEL = CLKSEL | 0x80; //; engage PLL


// Disable watchdog timer (COPCTL register)
  COPCTL = 0x40   ; //COP off; RTI and COP stopped in BDM-mode

         
//  Add additional port pin initializations here
  SPICR1  = 0x50; //initialize for master mode, interrupts off, CPOL=0, CPHA=0, slave select disabled
  SPICR2  = 0x00; //initializes for normal mode

// Initialize the SPI to 6.25 MHz
  SPIBR = 0x01;

//ATD initializations for LED intensity
  ATDCTL2 = 0x80;
  ATDCTL3 = 0x08;
  ATDCTL4 = 0x85; //8 bit mode
  ATDCTL5 = 0x00; //unsigned, left justified, input channel 0

//PWM Initializations for LED intensity
  PWME_PWME0 	= 1; //enables channel 0 clock
  PWMPOL_PPOL0 	= 0; //sets channel 0 to negative polarity
  PWMCAE_CAE0 	= 0; //sets channel 0 to left allignment
  PWMCLK_PCLK0 	= 1; //sets channel 0 to clock SA
  PWMPRCLK 	= 0x01;	//sets clock to 12 MHz
  PWMSCLA	= 600;  //sets clock SA to 12 MHz/(2*600) = 10000 HZ    
  PWMPER0 	= 0xFF;
  PWMDTY0	= 0x00;			 		  			 		  		
// Initialize digital I/O port pins
  DDRT = 0xFF;
  DDRM_DDRM4 = 1;
  DDRM_DDRM5 = 1;


/* Initialize the LCD
       ; - pull LCDCLK high (idle)
       ; - pull R/W' low (write state)
       ; - turn on LCD (LCDON instruction)
       ; - enable two-line mode (TWOLINE instruction)
       ; - clear LCD (LCDCLR instruction)
       ; - wait for 2ms so that the LCD can wake up
*/
  PTT_PTT4 = 1;
  PTT_PTT3 = 0;
  send_i(LCDON);
  send_i(TWOLINE);
  send_i(LCDCLR);
  lcdwait();

	 	   			 		  			 		  		
// Initialize RTI for 2.048 ms interrupt rate	
  RTICTL = 0x1F;
  CRGINT = 0x80;


/*
 Initialize TIM Ch 7 (TC7) for appropriate interrupt rate
;    Enable timer subsystem
;    Set channel 7 for output compare
;    Set appropriate pre-scale factor and enable counter reset after OC7
;    Set up channel 7 to generate 1 ms interrupt rate
;    Initially disable TIM Ch 7 interrupts
*/
  TSCR1 = 0x80;
  TSCR2 = 0x0C;
  TIOS = 0x80;
  TIE_C7I = 0;
  TC7 = 1500;
 
	      
}
	 		  			 		  		
/***********************************************************************
Main
***********************************************************************/
void main(void) {
  	DisableInterrupts;
	initializations(); 		  			 		  		
	EnableInterrupts;



  while(1) {
  //loop

  /* write your code here */
    if(tenths == 1)
      {
         while(ATDSTAT0_SCF != 1){}
	 PWMDTY0 = ATDDR0H;
      }




    
    _FEED_COP(); /* feeds the dog */
  } /* loop forever */
  /* please make sure that you never leave main */
}




/***********************************************************************                       
; RTI interrupt service routine: RTI_ISR
;
;  Initialized for 2.048 ms interrupt rate
;
;  Samples state of pushbuttons (PAD7 = left, PAD6 = right)
;
;  If change in state from "high" to "low" detected, set pushbutton flag
;     leftpb (for PAD7 H -> L), rghtpb (for PAD6 H -> L)
;     Recall that pushbuttons are momentary contact closures to ground
;
	 		  			

 		  		
;***********************************************************************/
interrupt 7 void RTI_ISR(void)
{
  	// set CRGFLG bit 
  	CRGFLG = CRGFLG | 0x80; 

}

/***********************************************************************                       
;  TIM interrupt service routine
;
;  Uses variable "onecnt" to track if one second has accumulated and
;  sets "onesec" flag	
;	 		  			 		  		
;***********************************************************************/
interrupt 15 void TIM_ISR(void)
{
  // set TFLG1 bit 
 	TFLG1 = TFLG1 | 0x80;  
  
  onecnt++;
  if(onecnt == 100)
    {
       onesec = 1;
       onecnt = 0;
    }

}

/***********************************************************************        
;                    
;  fdisp: Welcomes players and outputs selected difficulty       
;
;***********************************************************************/
void fdisp()
{
 
 
}
/***********************************************************************        
;                    
;  sdisp: Displays player score       
;
;***********************************************************************/
void sdisp()
{
 
 
}

/***********************************************************************                              
;  shiftout: Transmits the contents of register A to external shift 
;            register using the SPI.  It should shift MSB first.  
;             
;            MISO = PM[4]
;            SCK  = PM[5]
;***********************************************************************/
void shiftout(char ch)
{
  int i;
  //read the SPTEF bit, continue if bit is 1
  while(SPISR_SPTEF == 0);
	//write data to SPI data register
  SPIDR = ch;
  //wait for 30 cycles for SPI data to shift out 
  for(i = 0; i < 30; i++);
}

/***********************************************************************                              
;  lcdwait: Delay for 2 ms
;***********************************************************************/
void lcdwait()
{
   int j;
   for(j = 0; j <4800; j++);
}

/***********************************************************************                              
;  send_byte: writes contents of register A to the LCD
;***********************************************************************/
void send_byte(char ch)
{
   //Shift out character
   shiftout(ch);
   //Pulse LCD clock line low->high
   //LCDCLK = 1;
   PTT_PTT4 = 0;
   PTT_PTT4 = 1;  
     //Wait 2 ms for LCD to process data
   lcdwait();
}
/***********************************************************************                              
;  send_i: Sends instruction passed in register A to LCD  
;***********************************************************************/
void send_i(char ch)
{
        //Set the register select line low (instruction data)
        //Send byte
  PTT_PTT2 = 0;
  send_byte(ch);
  PTT_PTT2 = 1;
}

/***********************************************************************                        
;  chgline: Move LCD cursor to the cursor position passed in A
;  NOTE: Cursor positions are encoded in the LINE1/LINE2 variables
;***********************************************************************/
void chgline(char ch)
{
  send_i(CURMOV);
  send_i(ch);
 }

/***********************************************************************                       
;  print_c: Print character passed in register A on LCD ,            
;***********************************************************************/
void print_c(char ch)
{
  while(!(SPISR & 0x80));
  send_byte(ch);
}

/***********************************************************************                              
;  pmsglcd: pmsg, now for the LCD! Expect characters to be passed
;           by call.  Registers should return unmodified.  Should use
;           print_c to print characters.  
;***********************************************************************/
void pmsglcd(char ch[])
{
  int i = 0;
  while(ch[i] != 0x00){
    print_c(ch[i]);
    i = i++;
  }

}


