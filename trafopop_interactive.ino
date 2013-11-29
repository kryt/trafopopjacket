// LED-jacket code, trafopop -workshop
// interactive version, contacts on both sleeves
// 2013 jvm
// SPI-code and the colorstruct from Stefan Hinz
// https://github.com/Stefan-Hintz

// "bap" and "parp" are on loan from Hyperbole and a half. :)

#include <SPI.h>

#define NUM 50

typedef struct color
{
  byte r, g, b;
}
color;

color pixels[NUM];
/* ************************************** */
int normalmode=1; // controls the modes
int slownessvalue[50]; // these values can be used to slow down the effects on individual LEDs
/* ******************************************* */
/* the stuff below is for the led light engine */
int rand_seed_pin=4; // or a1, if possible. It is not connected, and should not be connected.

unsigned char sucRAND0=0x01; // pseudo-random generator (modified from some generic example)
unsigned char sucRAND1=0x01; // four variables
unsigned char sucRAND2=0x01;
unsigned char sucRAND3=0x01;

int val[50];   // calculated brightness
int trg[50];   // target brightness
int stepspeed[50];  // speed of the effect
int dir[50];  // direction of the effect
int slowness[50];  // delay that can be used to further slow down individual LEDs


byte ltarm[15];
byte rtarm[15];
byte uleft[6];
byte uright[6];
byte back[8];

// variables for charging stuff
int charging=0;
int charging_vaist=0;
int chgcnt=0;
int chargevalue=0;
int spark_now=0;
int spark_propagation=0;
int sparkmoves=0;
int sparkspeed=3;
int old_normalmode=0;

// interaction pin definitions
int left_hand=3;      //input//determine the led, and the time
int right_hand = 4;        //output//determine the led, and the time
int old_hands=0;
int current_hands=0;
int pos=00;


/* ************************************** */
// inits arrays and stuff
void init_arrs()
{
  color foo;
  foo.r=1;
  foo.g=1;
  foo.b=1;

  for( int blerg=0;blerg<50;blerg++)
  {
    //    fdelaysright[blerg]=fdelaysleft[blerg]=0;
    slownessvalue[blerg]=15;
    stepspeed[blerg]=1;
    foo.r=blerg+1;
    foo.g=blerg+1;
    foo.b=blerg+1;

    pixels[blerg]=foo;
  }

  slownessvalue[0]=6;
  slownessvalue[1]=6;
  slownessvalue[2]=6;
  slownessvalue[3]=6;
  slownessvalue[4]=6;
  slownessvalue[5]=6;
  slownessvalue[49]=6;
  slownessvalue[48]=6;
  slownessvalue[47]=6;
  slownessvalue[46]=6;
  slownessvalue[45]=6;
  slownessvalue[44]=6;

}

//0-3, 47-49
//7-23

// "gauss" table, to control the flicker max brightness to a more natural looking one
// modify with e.g. excel or similar
// can be also used to create a candle effect quite nicely
const unsigned char pgauss_table[256]=
{
  0	,
  1	,
  2	,
  3	,
  4	,
  5	,
  6	,
  7	,
  8	,
  9	,
  10	,
  11	,
  11	,
  12	,
  13	,
  14	,
  15	,
  16	,
  17	,
  18	,
  19	,
  20	,
  21	,
  22	,
  23	,
  23	,
  24	,
  25	,
  26	,
  27	,
  28	,
  29	,
  30	,
  31	,
  32	,
  32	,
  33	,
  34	,
  35	,
  36	,
  37	,
  38	,
  39	,
  40	,
  41	,
  41	,
  42	,
  43	,
  44	,
  45	,
  46	,
  47	,
  48	,
  48	,
  49	,
  50	,
  51	,
  52	,
  53	,
  54	,
  55	,
  55	,
  56	,
  57	,
  58	,
  59	,
  60	,
  61	,
  61	,
  62	,
  63	,
  64	,
  65	,
  66	,
  66	,
  67	,
  68	,
  69	,
  70	,
  70	,
  71	,
  72	,
  73	,
  74	,
  75	,
  75	,
  76	,
  77	,
  78	,
  78	,
  79	,
  80	,
  81	,
  82	,
  82	,
  83	,
  84	,
  85	,
  85	,
  86	,
  87	,
  88	,
  88	,
  89	,
  90	,
  91	,
  91	,
  92	,
  93	,
  94	,
  94	,
  95	,
  96	,
  96	,
  97	,
  98	,
  98	,
  99	,
  100	,
  101	,
  101	,
  102	,
  103	,
  103	,
  104	,
  105	,
  105	,
  106	,
  107	,
  107	,
  108	,
  108	,
  109	,
  110	,
  110	,
  111	,
  112	,
  112	,
  113	,
  113	,
  114	,
  115	,
  115	,
  116	,
  116	,
  117	,
  118	,
  118	,
  119	,
  119	,
  120	,
  120	,
  121	,
  121	,
  122	,
  122	,
  123	,
  124	,
  124	,
  125	,
  125	,
  126	,
  126	,
  127	,
  127	,
  128	,
  128	,
  128	,
  129	,
  129	,
  130	,
  130	,
  131	,
  131	,
  132	,
  132	,
  133	,
  133	,
  133	,
  134	,
  134	,
  135	,
  135	,
  135	,
  136	,
  136	,
  137	,
  137	,
  137	,
  138	,
  138	,
  138	,
  139	,
  139	,
  139	,
  140	,
  140	,
  140	,
  141	,
  141	,
  141	,
  142	,
  142	,
  142	,
  142	,
  143	,
  143	,
  143	,
  144	,
  144	,
  144	,
  144	,
  145	,
  145	,
  145	,
  145	,
  145	,
  146	,
  146	,
  146	,
  146	,
  146	,
  147	,
  147	,
  147	,
  147	,
  147	,
  147	,
  148	,
  148	,
  148	,
  148	,
  148	,
  148	,
  148	,
  148	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  149	,
  150	,
  150
};

/***************************************************/
/***************************************************/
/***************************************************/
/*****************program below*********************/
/***************************************************/
/***************************************************/
/***************************************************/
void setup()
{
  pinMode(left_hand,INPUT);
  pinMode(right_hand,OUTPUT);

  digitalWrite(left_hand,HIGH);    //pull_up resistors
  digitalWrite(right_hand,LOW);    // sink in idle/normal  

  Serial.begin(57600);

  SPI.begin();
  SPI.setBitOrder(MSBFIRST);
  SPI.setDataMode(SPI_MODE0);
  // SPI.setClockDivider(SPI_CLOCK_DIV16);  // 1 MHz
  SPI.setClockDivider(SPI_CLOCK_DIV8);  // 2 MHz
  // SPI.setClockDivider(SPI_CLOCK_DIV4);  // 4 MHz 

  init_arrs();
  // initialise the prandom-seed with noise, and take care of zero-result
  sucRAND0=(char)analogRead(rand_seed_pin);
  if( sucRAND0==0)
  {
    sucRAND0=1;
  }
  Serial.print("Seed: ");  // to show seed value
  Serial.println(sucRAND0);
}
/***************************************************/
unsigned char prand(void) // pseudorandom generator (modified from some example from the internet)
{
  unsigned char ucX=0;
  unsigned char ucY=0;

  if( (sucRAND3&0x80)==0x80 )
  {
    ucX=0x01;
  }
  if( (sucRAND2&0x02)==0x02 )
  {
    ucY=0x01;
  }
  ucX=(ucX^ucY);

  sucRAND3=sucRAND3<<1;
  sucRAND3=sucRAND3&0xFE;
  if( (sucRAND2&0x80)==0x80)sucRAND3|=0x01;
  sucRAND2=sucRAND2<<1;
  sucRAND2=sucRAND2&0xFE;
  if( (sucRAND1&0x80)==0x80)sucRAND2|=0x01;
  sucRAND1=sucRAND1<<1;
  sucRAND1=sucRAND1&0xFE;
  if( (sucRAND0&0x80)==0x80)sucRAND1|=0x01;
  sucRAND0=sucRAND0<<1;
  sucRAND0=sucRAND0&0xFE;

  if( (ucX&0x01) == 0x01 )
  {
    sucRAND0|=0x01;
  }
  return sucRAND3;
}
/***************************************************/
unsigned char pgauss(unsigned char ucNum)
{// gaussify the random function output
  ucNum=pgauss_table[ucNum];
  return ucNum;
}
/***************************************************/
// this controls the blinking of the leds
void target(int unit)
{

  if ( dir[unit]==1)// rise
  {
    if ( trg[unit] > val[unit])
    {
      if ( slowness[unit]>0)
      {
        slowness[unit]--;
      }
      else
      { 
        val[unit]+=stepspeed[unit];
        slowness[unit]=slownessvalue[unit];
      }
    }
    else
    {

      if ((chargevalue>0) && (spark_now==0))
      {
        if ( ((unit < 4) || (unit > 46) || (unit==25)) )
        {
          stepspeed[unit]=chargevalue/8;
          trg[unit]=pgauss(prand())+chargevalue;
        }
        else
        {
          trg[unit]=16;//(pgauss(prand())+1)/((chargevalue/2)+1);

          //          trg[unit]=pgauss(prand());//+90;
          stepspeed[unit]=(prand()/16)+1;
        }
      }
      else
      {
        trg[unit]=pgauss(prand());//+90;
        stepspeed[unit]=(prand()/32)+1;
      }
      if ( trg[unit]>val[unit])
      {
        dir[unit]=1;
      }
      else
      {
        dir[unit]=0;
      }
    }
  }
  else // fall
  {
    if ( trg[unit] < val[unit])
    {
      if ( slowness[unit]>0)
      {
        slowness[unit]--;
      }
      else
      { 
        val[unit]-=stepspeed[unit];
        slowness[unit]=slownessvalue[unit];
      }
    }
    else
    {

      if ((chargevalue>0) && (spark_now==0))
      {
        if ( ((unit < 4) || (unit > 46) || (unit==25)) )
        {
          stepspeed[unit]=chargevalue/8;
          trg[unit]=pgauss(prand())+chargevalue;
        }
        else
        {
          trg[unit]=16;//(pgauss(prand())+1)/((chargevalue/2)+1);

          //          trg[unit]=pgauss(prand());//+90;
          stepspeed[unit]=2;
        }
      }
      else
      {
        trg[unit]=pgauss(prand());//+90;
        stepspeed[unit]=(prand()/32)+1;
      }

      if ( trg[unit]>val[unit])
      {
        dir[unit]=1;
      }
      else
      {
        dir[unit]=0;
      }
    }
  }
}
/***************************************************/
/***************************************************/
void chargecheck()
{
  current_hands=check_hands();
  if ( current_hands==11 )
  {
    charging=1;
    charging_vaist=1;
    normalmode=1;

    chgcnt++;

    if ( chgcnt>5 )
    {
      chgcnt=0;
      chargevalue++;

      if ( chargevalue > 120)
      {
        chargevalue=120;
      }
    }
  }
  else
  {
    if( old_hands!=current_hands)
    {
      //start sparking!!!  
      charging=0;
      normalmode=2;
      spark_now=1;
      spark_propagation=0;
    }

  }
  old_hands=current_hands;
}
/***************************************************/
/***************************************************/
void spark()
{
  if ( spark_now==1)
  {
    sparkspeed=10;
    if ( spark_propagation > 2)
    {
      for ( int f=0;f<8;f++)
      {
        if ( back[f]>6)
        {
          back[f]-=5;
        }
        else
        {
          back[f]=0;
        }
      }
    }

    if ( sparkmoves<sparkspeed)
    {
      sparkmoves++;
    }
    else
    {
      sparkmoves=0;

      if ( spark_propagation < 1)
      {
        uleft[spark_propagation]=chargevalue;
        uright[spark_propagation]=chargevalue;
      }
      else if (spark_propagation < 2)
      {
        uleft[spark_propagation]=chargevalue;
        uright[spark_propagation]=chargevalue;
        uleft[spark_propagation-1]=chargevalue/2;
        uright[spark_propagation-1]=chargevalue/2;
      }
      else if ( spark_propagation < 6)
      {
        uleft[spark_propagation]=chargevalue;
        uright[spark_propagation]=chargevalue;
        uleft[spark_propagation-1]=chargevalue/2;
        uright[spark_propagation-1]=chargevalue/2;        
        uleft[spark_propagation-2]=chargevalue/64;
        uright[spark_propagation-2]=chargevalue/64;
      }
      else if ( spark_propagation < 7)
      {
        ltarm[spark_propagation-6]=chargevalue;
        rtarm[spark_propagation-6]=chargevalue;
        uleft[spark_propagation-1]=chargevalue/2;
        uright[spark_propagation-1]=chargevalue/2;        
        uleft[spark_propagation-2]=chargevalue/64;
        uright[spark_propagation-2]=chargevalue/64;
      }
      else if ( spark_propagation < 8)
      {
        ltarm[spark_propagation-6]=chargevalue;
        rtarm[spark_propagation-6]=chargevalue;
        ltarm[spark_propagation-7]=chargevalue/2;
        rtarm[spark_propagation-7]=chargevalue/2;        
        uleft[spark_propagation-2]=chargevalue/64;
        uright[spark_propagation-2]=chargevalue/64;
      }
      else if ( spark_propagation<21)
      {
        ltarm[spark_propagation-6]=chargevalue;
        rtarm[spark_propagation-6]=chargevalue;
        ltarm[spark_propagation-7]=chargevalue/2;
        rtarm[spark_propagation-7]=chargevalue/2;        
        ltarm[spark_propagation-8]=chargevalue/64;
        rtarm[spark_propagation-8]=chargevalue/64;
      }
      else if ( spark_propagation<22)
      {
        ltarm[spark_propagation-7]=chargevalue+chargevalue;
        rtarm[spark_propagation-7]=chargevalue+chargevalue;
        ltarm[spark_propagation-8]=chargevalue;
        rtarm[spark_propagation-8]=chargevalue;
      }
      else if ( spark_propagation<23)
      {
        ltarm[spark_propagation-8]=chargevalue*4+14;
        rtarm[spark_propagation-8]=chargevalue*4+14;
        ltarm[spark_propagation-9]=chargevalue/4;
        rtarm[spark_propagation-9]=chargevalue/4;
      }
      else
      {
        asm("nop");
      }
      spark_propagation++;
    }

    for ( int g=0;g<50;g++)
    {
      trg[g]=2;
      dir[g]=0;
      stepspeed[g]=1;
    }

/*    if ( spark_propagation > 22)
    {
      stepspeed[36]=stepspeed[14]=4;
      stepspeed[36]=stepspeed[14]=4;
    }
  */
    if( spark_propagation > 40)
    {
      spark_now=0;
      chargevalue=0;
      normalmode=0; // back to normality
      spark_propagation=0;
    }
  }
}
/****************************************************/
/****************************************************/
//returns the configuration of hands
int check_hands()
{
  int lh_le=0; // left hand left eye
  int lh_temp=0;
  int retval=0;

  digitalWrite( right_hand, HIGH);
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  lh_le=digitalRead(left_hand);

  digitalWrite( right_hand, LOW);
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  asm("nop");
  lh_temp=digitalRead(left_hand);

  // both conditions must be checked if it is to be trusted
  if( (lh_temp==0) && (lh_le == 1) ) 
  {
    lh_le=1;
  }
  else
  {
    lh_le=0;
  }

  pinMode(right_hand,OUTPUT); //as input and...
  digitalWrite(right_hand,LOW);//pull_up off  

  if((lh_le==1))
  {
    // Serial.println("connection");
    retval=11;
  }

  return retval;
}
/****************************************************/

/***************************************************/
// flicker does... flickering.
void flicker()
{
  struct color egh;

  if ( normalmode>1 )
  {
    // transformation
    for ( int d=0;d<3;d++)
    {
      val[d]=back[7-d*2];     //7-0  5-1  3-2 1-3
      val[d+47]=back[d*2+2];    //2-47 4-48 6-49
    }
    val[3]=back[1];
    val[25]=back[0];

    for ( int d=0;d<3;d++ )
    {  
      val[d+4]=uleft[d*2];
      val[d+22]=uleft[5-d*2];       

      val[d+26]=uright[d*2];
      val[46-d]=uright[d*2+1];
    }

    for ( int d=0;d<7;d++ )
    {  
      val[d+7]=ltarm[d*2];
      val[21-d]=ltarm[d*2+1];       

      val[d+29]=rtarm[d*2+1];
      val[43-d]=rtarm[d*2];
    }
    val[36]=rtarm[14];
    val[14]=ltarm[14];
  }


  if ( normalmode<2)
  {
    for ( int parp=0;parp<50;parp++)
    {
      target(parp);
    }

    for ( int d=0;d<3;d++)
    {
      back[7-d*2]=val[d];     //7-0  5-1  3-2 1-3
      back[d*2+2]=val[d+47];    //2-47 4-48 6-49
    }
    back[1]=val[3];
    back[0]=val[25];

    for ( int d=0;d<3;d++ )
    {
      uleft[d*2]=val[d+4];
      uleft[5-d*2]=val[d+22];       

      uright[d*2]=val[d+26];
      uright[d*2+1]=val[46-d];
    }

    for ( int d=0;d<7;d++ )
    {  
      ltarm[d*2]=val[d+7];
      ltarm[d*2+1]=val[21-d];

      rtarm[d*2+1]=val[d+29];
      rtarm[d*2]=val[43-d];
    }
    rtarm[14]=val[36];
    ltarm[14]=val[14];
  }



  /* this part updates the color values to pixel values*/
  for( int parp=0;parp<50;parp++)
  {
    target(parp);
    egh.r=val[parp];
    egh.b=val[parp];
    egh.g=val[parp];
    pixels[parp]=egh;
  }
}
/***************************************************/
void show2(byte *bytes, int size)
{
  for (int index=0; index<size; index++)
  {
    byte c = bytes[index];//>>2;
    for (SPDR = c; !(SPSR & _BV(SPIF)););
  }
}
/***************************************************/
void loop() 
{
  chargecheck(); // checking if and how much to charge
  spark();    // take care of what happens during sparking
  flicker(); // does the mapping+target

  show2((byte *)pixels, sizeof(pixels));  // SPI-routine from Stefan Hinz
  delay(3); // delay between 5 and 10 works fine
}
/***************************************************/
/***************************************************/
/*

 */









