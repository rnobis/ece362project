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
void UserSays();
void SimonSays();
void compare();
int getPushButton();
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
int wrong   = 0;  // incorrect user input flag
int ready	= 0;  // user ready flag
int tenths	= 0;  // tenth of a second flag
int simon	= 0;  // flag for when simons turn begins
int user 	= 0;  // flag for when users turn begins
int simoncnt = 0; 
int simonmax = 1000; // 
int usercnt = 0;
int usermax = 0; 
int tencnt	= 0;  // TENCNT (variable)
int random 	= 100;  // random number
int wait 	= ;  // length of time to wait before user can input
int Simon[] = [4,4,4,4]; //Simon random output
int User[]  = [4,4,4,4]; //User input

//;LCD COMMUNICATION BIT MASKS
int RS = 0x02;     //;RS pin mask (PTT[1])
int RW = 0x04;     //;R/W pin mask (PTT[2])
int LCDCLK  = 0x08;     //;LCD EN/CLK pin mask (PTT[3])

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

         
//SPI Initializations for LCD Output
  SPICR1  = 0x50; //initialize for master mode, interrupts off, CPOL=0, CPHA=0, slave select disabled
  SPICR2  = 0x00; //initializes for normal mode
  SPIBR =   0x01; // Initialize the SPI to 6.25 MHz

//ATD initializations for LED intensity
  ATDCTL2 = 0x80;
  ATDCTL3 = 0x08;
  ATDCTL4 = 0x85; //8 bit mode
  ATDCTL5 = 0x00; //unsigned, left justified, input channel 0

//PWM Initializations for LED intensity
  PWME_PWME0 	= 1;     //enables channel 0 clock
  PWMPOL_PPOL0 	= 0;     //sets channel 0 to negative polarity
  PWMCAE_CAE0 	= 0;     //sets channel 0 to left allignment
  PWMCLK_PCLK0 	= 1;     //sets channel 0 to clock SA
  PWMPRCLK 		= 0x01;	 //sets clock to 12 MHz
  PWMSCLA		= 600;   //sets clock SA to 12 MHz/(2*600) = 10000 Hz    
  PWMPER0 		= 0xFF; 
  PWMDTY0		= 0x00;			 
 		  			 		  		
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
  PTT_PTT3 = 1;
  PTT_PTT2 = 0;
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

//Initialize port T to PWM to control brightness
  MODRR_MODRR0 = 1; 

//initialize ATD ports as digital inputs
  ATDDIEN_IEN1 = 1; //start pb mapped to AN1
  ATDDIEN_IEN2 = 1; //red pb mapped to AN2
  ATDDIEN_IEN3 = 1; //yellow pb mapped to AN3
  ATDDIEN_IEN4 = 1; //green pb mapped to AN4
  ATDDIEN_IEN5 = 1;	//blue pb mapped to AN5
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
      	ATDCTL5 = 0x80; //Begin ATD conversion
        while(ATDSTAT0_SCF != 1){}
	 	PWMDTY0 = ATDDR0L;
      }

	//Ask user to press the start button, outputs random LED output, asks user to enter their attempt
	//output appropriate message, and update wait time if correct
	if(runstp == 1)
	  {
		fdisp();
		SimonSays();
		simon = 0; //set simon flag back to zero
		ready = 1;
		fdisp();
		UserSays();
		user = 0;  //set user flag back to zero
		ready = 0;
		compare();
		runstp = 0;
	  }
	else
	  {
		fdisp();
	  }
	//Generate Game Over, wait five seconds, and then set flag back to zero
	if(wrong == 1)
	  {
		int i;
		fdisp();
		while(fivecnt<5000);
		wrong = 0;
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
;  Samples state of pushbuttons (PAD1 = runstp, PAD2 = redpb, PAD3 = yellpb
;  PAD4 = greenpb, PAD5 = yellpb)
;
;  If change in state from "high" to "low" detected, set pushbutton flag
;     Recall that pushbuttons are momentary contact closures to ground
;
;***********************************************************************/
interrupt 7 void RTI_ISR(void)
{
  	// set CRGFLG bit 
  	CRGFLG = CRGFLG | 0x80; 
	
	random++;
	if (random == 255)
	{
		random = 100;
	}
	if (PORTAD0_PTAD1 == 1)
	{
		runstp = 1;
	}
	if (PORTAD0_PTAD2 == 1)
	{
		redpb = 1;
	}
	if (PORTAD0_PTAD3 == 1)
	{
		yellpb = 1;
	}
	if (PORTAD0_PTAD4 == 1)
	{
		greenpb = 1;
	}
	if (PORTAD0_PTAD5 == 1)
	{
		bluepb = 1;
	}
	
}

/***********************************************************************                       
;  TIM interrupt service routine
;
;  Uses variable "tencnt" to track if one tenth of a second has 
;  accumulated and sets "tenths" flag. Also use to update wait time.
;	 		  			 		  		
;***********************************************************************/
interrupt 15 void TIM_ISR(void)
{
  // set TFLG1 bit 
 	TFLG1 = TFLG1 | 0x80;  
  
  tencnt++;
  if(tencnt == 100)
    {
       tenths = 1;
       tencnt = 0;
    }
  if(simon == 1)
	{
	   simoncnt++;
	}
  if(user == 1)
	{
	   usercnt++;
	}
  //counter for five seconds
  if(wrong == 1)
	{
	   fivecnt++;
	}

}
//Function for generating the random LED output for the game Simon Says
void SimonSays()
{
	int i = 0;
	int j = 0;
	int k = 0;

	//set simon flag
	simon = 1;
	
	//Fill array
	while(i < 4)
	{
		Simon[i] = random % 4; 
		//random = random / 4;   
		i++;
	}
	
	//Output array for user to see
	while(i < 4)
	{
		if(Simon[i] == 0)
		{
			//Red LED on then off (active low)
			PTT_PTT4 = 0;
			for(k=0;k < 24000;k++); //wait 10 ms
			PTT_PTT4 = 1;
		}
		if(Simon[i] == 1)
		{
			//Yellow LED on then off (active low)
			PTT_PTT5 = 0;
			for(k=0;k < 24000;k++); //wait 10 ms
			PTT_PTT5 = 1;
		}
		if(Simon[i] == 2)
		{
			//Green LED on then off (active low)
			PTT_PTT6 = 0;
			for(k=0;k < 24000;k++); //wait 10 ms
			PTT_PTT6 = 1;
		}
		if(Simon[i] == 3)
		{
			//Blue LED on then off (active low)
			PTT_PTT7 = 0;
			for(k=0;k < 24000;k++); //wait 10 ms
			PTT_PTT7 = 1;
		}
		while(simoncnt < simonmax);
		simoncnt = 0;
		i++;
	}		
}
//Keep track of buttons user inputs
void UserSays()
{	
	int i = 0;
	int j = 0;
	//set user flag
	user = 1;

	while(i < 4)
	{
		User[i] = getPushButton();
		i++;
	}
}
int getPushButton()
{
	int i = 0;
	
	if(redpb == 1)
	{
		redpb = 0; //set redpb flag back to 0
		return 0;
	}
	if(yellpb == 1)
	{
		yellpb = 0; //set yellpb flag back to 0
		return 1;
	}
	if(greenpb == 1)
	{
		greenpb = 0; //set greenpb flag back to 0
		return 2;
	}
	if(bluepb == 1)
	{
		bluepb = 0; //set bluepb flag back to 0
		return 3;
	}
}
//Compares user and simon arrays
void compare()
{
	int i;

	while(i < 4)
	{
		if(Simon[i] != User[i])
		{
			wrong = 1;
		}
		i++;
	}
	//Coud add scoring function here if wrong == 0
	
}
/***********************************************************************        
;                    
;  fdisp: Welcomes players and outputs selected difficulty, also asks
;         user if they want to play again       
;
;***********************************************************************/
void fdisp()
{
	char gameOver[] = {'G','a','m','e',' ','O','v','e','r','\0'};
	char welcome[] = {'P','r','e','s','s',' ','S','T','A','R','T','\0'};
	char Simon[]	= {'S','i','m','o','n',' ','s','a','y','s','\0'};
	char user[]		= {'Y','o','u','r',' ','t','u','r','n','\0'};
	
	//changes to line 1, prints 'Game Over' message
 	if(wrong == 1)
	{
		chgline(LINE1);
		pmsglcd(gameOver[]);
	}
	//changes to line 1, prints 'Press START'
	if(runstp == 0)
	{
		chgline(LINE1);
		pmsglcd(welcome[]);
	}
	//changes to line 1, prints 'Simon says', during simon's turn
	if(runstp == 1)
	{
		chgline(LINE1);
		pmsglcd(Simon[]);
	}
	//changes to line 1, prints 'Your turn', during user's turn
	if(runstp == 1 && ready == 1)
	{
		chgline(LINE1);
		pmsglcd(user[]);
	}

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
   for(j = 0; j < 4800; j++);
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
   PTT_PTT3 = 0;
   PTT_PTT3 = 1;  
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
  PTT_PTT1 = 0;
  send_byte(ch);
  PTT_PTT1 = 1;
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


