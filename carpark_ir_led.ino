
#define  XTC    TC0       // TC number
#define  XCHAN  0         // TC channel
#define  XID    ID_TC0    // Instance ID
//#define  PIN    40
#define  PINCHAN  25      // Digital pin 2 which is on PIOB channel 25;

int receiver_right = 42;
int receiver_mid = 41;
int receiver_left = 40;
void squarewave(unsigned int freq) {
  unsigned int  tcclk = VARIANT_MCK / freq / 4;
  
  TC_Configure(XTC, XCHAN, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_ACPC_TOGGLE);
  TC_SetRC(XTC, XCHAN, tcclk);
  TC_Start(XTC, XCHAN);
}
void setup() {
  unsigned int chanmask = (1 << PINCHAN);
  Serial.begin(9600);
  // put your setup code here, to run once:
  REG_PIOB_PDR = chanmask;
  REG_PIOB_ABSR = chanmask;
  REG_PIOB_MDDR = chanmask;
  
  // set up TC
  pmc_set_writeprotect(false);
  pmc_enable_periph_clk(XID);
  squarewave(36000);


  pinMode(receiver_left,INPUT);
  pinMode(receiver_right,INPUT);
}

void loop() {
//   put your main code here, to run repeatedly:
  int left = digitalRead(receiver_left);
  Serial.print( "left : ");
  Serial.println( left);
  delay(500);
  int right = digitalRead(receiver_right);
    Serial.print( "Right : ");
  Serial.println(right);
  delay(500);
  int mid = digitalRead(receiver_mid);
    Serial.print( "Mid : ");
  Serial.println(mid);
  delay(500); 
}
