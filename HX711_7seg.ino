/* Calibration sketch for HX711 */
/*=============================*/ 
#include "HX711.h"  // Library needed to communicate with HX711 https://github.com/bogde/HX711
/*=============================*/

/*=============HX711 define================*/
#define DOUT  11  // Arduino pin 11 connect to HX711 DOUT
#define CLK  10  //  Arduino pin 10 connect to HX711 CLK

HX711 scale(DOUT, CLK);  // Init of library
/*=============HX711 define end================*/

int weight;

/*=============7 segment define================*/
//Pins on the 74HC595 shift reg chip - correspondence with Arduino digital pins
unsigned int SH_CP = 6;     // SCLK
unsigned int ST_CP = 5;     // RCLK
unsigned int DS    = 4;     // DIO
/*=============7 segment define end================*/

void setup() {
  /* HX711 set===================================*/
  Serial.begin(9600);
  scale.set_scale();  // Start scale
  scale.tare();       // Reset scale to zero
  /*HX711 set end================================*/
  
  /* 7 segment set===================================*/
  // Set Arduino pins as outputs
  pinMode(SH_CP, OUTPUT); 
  pinMode(ST_CP, OUTPUT); 
  pinMode(DS,    OUTPUT);
  /* 7 segment set end===================================*/
  
}

/* 7 segment func===================================*/
// Table to convert a hex digit into the matching 7-seg display segments
int hexDigitValue[] = {
  0xFC,    /* Segments to light for 0  */
  0x60,    /* Segments to light for 1  */
  0xDA,    /* Segments to light for 2  */
  0xF2,    /* Segments to light for 3  */
  0x66,    /* Segments to light for 4  */
  0xB6,    /* Segments to light for 5  */
  0xBE,    /* Segments to light for 6  */
  0xE0,    /* Segments to light for 7  */
  0xFE,    /* Segments to light for 8  */
  0xF6     /* Segments to light for 9  */
};

/* Set display digit to a hexadecimal value '0'..'9','A'..'F'
 *  
 *    row = row number of digit to light up 0..3 where 3 is most significant (leftmost) display digit
 *    dgit = value 0 to 15 
 *    decimalPoint = true/false : whether to light thde decimal point or not
 */
void setDigit(unsigned int row, unsigned int digit, boolean decimalPoint)
{
  unsigned int rowSelector;
  unsigned int data;

  rowSelector = bit(3-row)<<4;
  data =  ~  hexDigitValue[ digit & 0xF ] ;
  if(decimalPoint)
    data &= 0xFE;
        
  // First 8 data bits all the way into the second 74HC595 
  shiftOut(DS, SH_CP, LSBFIRST, data );

  // Now shift 4 row bits into the first 74HC595 and latch
  digitalWrite(ST_CP, LOW);
  shiftOut(DS, SH_CP, LSBFIRST, rowSelector );
  digitalWrite(ST_CP, HIGH);
  
}

/* Displays a number as a 4-digit decimal number on the display
 *   Note this is multiplexed, so you need to keep calling it
 *   or you'll end up with just one digit lit.
 */


void displayNumber(int number){

  int thousands,hundreds,tens,units;
  thousands = number/1000;
  hundreds = number/100%10;
  tens = number/10%10; 
  units = number%10;

  int i=0;
  for(i=0;i<300;i++){
      setDigit(3, thousands, false); 
      delayms(1);
      setDigit(2, hundreds, false);
      delayms(1);  
      setDigit(1, tens, false);
      delayms(1);
      setDigit(0, units, false); 
      delayms(1);
    }

}

void delayms(unsigned int time)
{
 unsigned int n;
while (time > 0)
 {
 n=120;
 while(n>0) n--;
 time--;
 }
}

/* 7 segment func end===================================*/

/*=============HX711 func================*/
void getWeight(){
      float current_weight=scale.get_units(20);  // get average of 20 scale readings
      float scale_factor=(current_weight/407);  // divide the result by a known weight 407

      weight = floor(scale_factor);//四捨五入,floor(x)傳回的是小於或等於x的最大整數
      Serial.println(weight);  // Print the scale factor to use
      displayNumber(weight);
}
/*=============HX711 func end================*/

void loop() {
  getWeight();
}
