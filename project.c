/***********************************************************************
; ECE 362 - Mini-Project C Source File - Fall 2013                    
;***********************************************************************
;	 	   			 		  			 		  		
; Team ID: 36
;
; Project Name: Digital Simon Says
;
; Team Members:
;
;   - Team/Doc Leader: Kyle Fesmire	      Signature: ______________________
;   
;   - Software Leader: Ryan Nobis	      Signature: ______________________
;
;   - Interface Leader: Tom Doddridge     Signature: ______________________
;
;   - Peripheral Leader: Chris Liow    	  Signature: ______________________
;
;
; Academic Honesty Statement:  In signing above, we hereby certify that we 
; are the individuals who created this HC(S)12 source file and that we have
; not copied the work of any other student (past or present) while completing 
; it. We understand that if we fail to honor this agreement, we will receive 
; a grade of ZERO and be subject to possible disciplinary action.
;
;***********************************************************************
;
; The objective of this Mini-Project is to create a Simon says games
; that generates random patterns for the user to mimic. Keeps track of
; the user score, and makes the game progressively harder. Also allows
; user to adjust display and LED brightness.
;
;***********************************************************************
;
; List of project-specific success criteria (functionality that will be
; demonstrated):
;
; 1. Random Pattern Generation
;
; 2. User input, and comparison to generated pattern
;
; 3. Scoring based on time, and ability to mimic
;
; 4. Ability to control LED and LCD brightness
;
;
;***********************************************************************
;
;  Date code started: November 19, 2013
;
;  Update history (add an entry every time a significant change is made):
;
;  Date: < ? >  Name: < ? >   Update: < ? >
;
;  Date: < ? >  Name: < ? >   Update: < ? >
;
;  Date: < ? >  Name: < ? >   Update: < ? >
;
;
;***********************************************************************/
#include <hidef.h>      /* common defines and macros */
#include "derivative.h"      /* derivative-specific definitions */
#include <mc9s12c32.h>

// All funtions after main should be initialiezed here
char inchar(void);
void outchar(char x);
void wrdisp();
void udisp();
void simdisp();
void weldisp();
void sdisp();
void cldisp();
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
void delayGen(int length);
int  randomGen();

//  Variable declarations                                                                                                        
int redpb        = 0;  // red pushbutton flag
int bluepb        = 0;  // blue pushbutton flag
int yellpb        = 0;  // yellow pushbutton flag
int greenpb        = 0;  // green pushbutton flag
int pb                = 0;  // keeps track of when one of the pushbuttons is pushed
int prevpb        = 0;  // previous pushbutton state
int runstp        = 0;  // game run/stop flag
int wrong   = 0;  // incorrect user input flag
int tenths        = 0;  // tenth of a second flag
int tencnt        = 0;  // TENCNT (variable)
int long score   = 0;
int simonflag   = 0;  // flag set to track simon time
int simoncnt = 0; // keeps track of simon time
int userflag        = 0;  // flag set to track user time
int usercnt = 0;  // keeps track of user time
int random         = 100;  // random number
int Simon[4] = {5,5,5,5}; //Simon random output
int User[4]  = {4,4,4,4}; //User input
int delay   = 0;
int difficulty = 0;

//;LCD COMMUNICATION BIT MASKS
int RS = 0x02;     //;RS pin mask (PTT[5])
int RW = 0x04;     //;R/W pin mask (PTT[6])
int LCDCLK  = 0x08;     //;LCD EN/CLK pin mask (PTT[7])

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

//PWM Initializations for LCD intensity
  PWME_PWME0         = 1;     //enables channel 0 clock
  PWMPOL_PPOL0         = 0;     //sets channel 0 to negative polarity
  PWMCAE_CAE0         = 0;     //sets channel 0 to left allignment
  PWMCLK_PCLK0         = 1;     //sets channel 0 to clock SA
  PWMPRCLK                 = 0x01;         //sets clock to 12 MHz
  PWMSCLA                = 6;   //sets clock SA to 12 MHz/(2*6) = 100000 Hz    
  PWMPER0                 = 0xFF; 
  PWMDTY0                = 0x00;                         
//PWM Initializations for LED Intensity
  PWME_PWME1 = 1;
  PWME_PWME2 = 1;
  PWME_PWME3 = 1;
  PWME_PWME4 = 1;
  PWMPOL_PPOL1 = 1;
  PWMPOL_PPOL2 = 1;
  PWMPOL_PPOL3 = 1;
  PWMPOL_PPOL4 = 1;
  PWMCAE_CAE1 = 1;
  PWMCAE_CAE2 = 1;
  PWMCAE_CAE3 = 1;
  PWMCAE_CAE4 = 1;
  PWMCLK_PCLK1 = 1;
  PWMCLK_PCLK2 = 1;
  PWMCLK_PCLK3 = 1;
  PWMCLK_PCLK4 = 1;
  PWMSCLB = 6;
  PWMPER1 = 0xFF;
  PWMPER2 = 0xFF;
  PWMPER3 = 0xFF;
  PWMPER4 = 0xFF;
  PWMDTY1 = 0x00;
  PWMDTY2 = 0x00;
  PWMDTY3 = 0x00;
  PWMDTY4 = 0x00;
                                                                              
// Initialize digital I/O port pins
  DDRT = 0xFF;
//Initializes Port M 4 and 5 to be outputs
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
  PTT_PTT7 = 1;
  PTT_PTT6 = 0;
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
  TIE = 0x80;
  TC7 = 1500;

//Initialize port T to PWM to control brightness
  MODRR_MODRR0 = 1;
  MODRR_MODRR1 = 1;
  MODRR_MODRR2 = 1;
  MODRR_MODRR3 = 1;
  MODRR_MODRR4 = 1;
  

//initialize ATD ports as digital inputs
  ATDDIEN_IEN1 = 1; //start pb mapped to AN1
  ATDDIEN_IEN2 = 1; //red pb mapped to AN2
  ATDDIEN_IEN3 = 1; //yellow pb mapped to AN3
  ATDDIEN_IEN4 = 1; //green pb mapped to AN4
  ATDDIEN_IEN5 = 1;        //blue pb mapped to AN5
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
        while(ATDSTAT0_SCF == 0){}
                    PWMDTY0 = ATDDR0L / 2 + 128; 
      }

        //Ask user to press the start button, outputs random LED output, asks user to enter their attempt
        //output appropriate message, and update wait time if correct
        if(runstp == 1)
          {
                simdisp();
                delayGen(1000);    
                SimonSays();
                simonflag = 0; //end of simon function
                udisp();
                UserSays();
                userflag = 0; //end of user turn
                delayGen(500);
                compare();
                cldisp();
          }
        else
          {
                weldisp();
          }
        //Generate Game Over, wait five seconds, and then set flag back to zero
        if(wrong == 1)
          {
                wrdisp();
                sdisp();
                delayGen(5000);
                cldisp();
                wrong = 0;
                runstp = 0;
                difficulty = 0;
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
        
        
        random ++;
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
          if(prevpb == 0) {
                redpb = 1;
                pb = 1;
          }
        }
        if (PORTAD0_PTAD3 == 1)
        {
          if(prevpb == 0) {
                yellpb = 1;
                pb = 1;
          }
        }
        if (PORTAD0_PTAD4 == 1)
        {
          if(prevpb == 0) {
                greenpb = 1;
                pb = 1;
          }
        }
        if (PORTAD0_PTAD5 == 1)
        {
          if(prevpb == 0) {
                bluepb = 1;
                pb = 1;
          }
        }
        prevpb = PORTAD0_PTAD2 | PORTAD0_PTAD3 | PORTAD0_PTAD4 | PORTAD0_PTAD5;       
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
  delay++;
  
  if(tencnt == 100) 
  {
    tenths = 1;
    tencnt = 0; 
  }
  if(simonflag == 1)
  {
        simoncnt++;
  }
  if(userflag == 1)
  {
        usercnt++;
  }

}
//Function for generating specific delays
void delayGen(int length)
{
  delay = 0;
  while(delay<length);
  delay = 0;
}
//Function for generating random numbers between 0 and 3
int randomGen()
{
}
//Function for generating the random LED output for the game Simon Says
void SimonSays()
{

        int i = 0;
        int j = 0;
        simonflag = 1; //set simon flag to keep track of time for function
        
        //Fill array
        for(i=0;i<4;i++) 
        {
          Simon[i] = random % 4;
          random = random / 4;
        }

        //Output array for user to see
        while(j < 4)
        {
                if(Simon[j] == 0)
                {
                        //Red LED on then off (active high)
                        PWMDTY1 = ATDDR0L / 5;
                        delayGen(100);
                        PWMDTY1 = 0x00;
                } 
                else if(Simon[j] == 1)
                {
                        //Yellow LED on then off (active high)
                        PWMDTY2 = ATDDR0L / 5;
                        delayGen(100);
                        PWMDTY2 = 0x00;
                } 
                else if(Simon[j] == 2)
                {
                        //Green LED on then off (active high)
                        PWMDTY3 = ATDDR0L /5;
                        delayGen(100);
                        PWMDTY3 = 0x00;
                } 
                else if(Simon[j] == 3)
                {
                        //Blue LED on then off (active high)
                        PWMDTY4 = ATDDR0L / 5;
                        delayGen(100);
                  PWMDTY4 = 0x00;
                }
                delayGen(1000 - difficulty);
                j++;
        }                
        return;
}

//Keep track of buttons user inputs
void UserSays()
{        
        int i = 0;
        userflag = 1;
        
        redpb = 0;     //reset pb flags to 0 before getting input
        yellpb = 0;
        greenpb = 0;
        bluepb = 0;
        pb = 0;

        while(i < 4)
        {
                User[i] = getPushButton();
                i++;
        }       
}
int getPushButton()
{
        
        if(redpb == 1)
        {
                redpb = 0; //set redpb flag back to 0
                pb = 0;    //set pb flag back to 0
                return 0;
        } 
        else if(yellpb == 1)
        {
                yellpb = 0; //set yellpb flag back to 0
                pb = 0;     //set pb flag back to 0
                return 1;
        } 
        else if(greenpb == 1)
        {
                greenpb = 0; //set greenpb flag back to 0
                pb = 0;      //set pb flag back to 0
                return 2;
        } 
        else if(bluepb == 1)
        {
                bluepb = 0; //set bluepb flag back to 0
                pb = 0;     //set pb flag back to 0
                return 3;
        }
        else 
        {
                while(pb == 0); //wait until a pushbutton is pressed
                return getPushButton();
        }
}
//Compares user and simon arrays
void compare()
{
        int i = 0;
        
        while(i < 4)
        {
                if(Simon[i] != User[i])
                {
                        wrong = 1;
                        return;
                }
                i++;
        }
        if(usercnt > simoncnt)
        {
                wrong = 1;
        }   
        else
        {
                score = score + 10;
                if(difficulty < 700)
                {
                        difficulty = difficulty + 50;
                }                
        }   

        usercnt  = 0;  //resets user time
        simoncnt = 0;  //resets simon time        
}
/***********************************************************************        
;                    
;  disp: Functions for displaying different messages      
;
;***********************************************************************/
void wrdisp()
{
        char gameOver[] = {'G','a','m','e',' ','O','v','e','r',' ',' ',' ',' ',' ',' ',' ','\0'};
        chgline(LINE1);
        pmsglcd(gameOver);
}
void udisp() 
{
  char user[]                  = {'Y','o','u','r',' ','t','u','r','n',' ',' ',' ',' ',' ',' ',' ','\0'};
  char ready[]                  = {'R','e','a','d','y',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  char ready1[]                  = {'R','e','a','d','y','.',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  char ready2[]                  = {'R','e','a','d','y','.','.',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  char ready3[]                  = {'R','e','a','d','y','.','.','.',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  char go[]                          = {'G','o','!',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  
  chgline(LINE1);
  pmsglcd(user);
  chgline(LINE2);
  pmsglcd(ready);
  delayGen(500);
  chgline(LINE2);
  pmsglcd(ready1);
  delayGen(1000);
  chgline(LINE2);
  pmsglcd(ready2);
  delayGen(1000);
  chgline(LINE2);
  pmsglcd(ready3);
  delayGen(1000);
  chgline(LINE2);
  pmsglcd(go);
  
}
void simdisp()
{
  char Simon[]          = {'S','i','m','o','n',' ','s','a','y','s',' ',' ',' ',' ',' ',' ','\0'};
  chgline(LINE1);
  pmsglcd(Simon);
}
void weldisp()
{
        char welcome[]  = {'P','r','e','s','s',' ','S','T','A','R','T',' ',' ',' ',' ',' ','\0'};
        chgline(LINE1);
        pmsglcd(welcome);
}
void sdisp()
{
  char scoreDisp[]    = {'S','c','o','r','e',':',' '};
  int long divisor = 100000000;
  int i = 0;
  int scoreStart = 0xC7;
  
  chgline(LINE2);
  pmsglcd(scoreDisp); 
  chgline(scoreStart);
  
  while(i < 9)
  {
    print_c((score / divisor) + 48);
    
    score = score % divisor;
    divisor = divisor / 10;
    i++;
  }    
  
}
void cldisp() 
{
  char clear[] = {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ','\0'};
  
  chgline(LINE1);
  pmsglcd(clear);
  chgline(LINE2);
  pmsglcd(clear);
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
   PTT_PTT7 = 0;
   PTT_PTT7 = 1;  
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
  PTT_PTT5 = 0;
  send_byte(ch);
  PTT_PTT5 = 1;
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
