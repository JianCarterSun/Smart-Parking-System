#define SSDSIZE 7               //number of segments in SSD
#define DIGSIZE 10              // number of different symbols to decode
#define no_of_parking_lot 3     //number of parking lot
#define blueToothSerial Serial2 
#define  XTC    TC0             // TC number
#define  XCHAN  0               // TC channel
#define  XID    ID_TC0          // Instance ID
#define  PINCHAN  25            // Digital pin 2 which is on PIOB channel 25
#define G_LED 30                //green LED
#define R_LED 29                //red LED
#define fir 42                  //first parking lot
#define sec 41                  //second parking lot
#define thi 40                  //third parking lot
int no_of_free_parking_lot = 3; // the variable that describes the number of free parking lot
int parking_lot[3];             // array of type int, indicate three car parks in our project, if vacant, then 1 and vice versa
int ssdtab[DIGSIZE][SSDSIZE] =
    {{1, 1, 1, 1, 1, 1, 0},
    {0, 1, 1, 0, 0, 0, 0},
    {1, 1, 0, 1, 1, 0, 1},
    {1, 1, 1, 1, 0, 0, 1},
    {0, 1, 1, 0, 0, 1, 1},
    {1, 0, 1, 1, 0, 1, 1},
    {1, 0, 1, 1, 1, 1, 1},
    {1, 1, 1, 0, 0, 0, 0},
    {1, 1, 1, 1, 1, 1, 1},
    {1, 1, 1, 1, 0, 1, 1}};     //part of seven-segment displays programs that specify the realtionshop between dispalyed number and coding

#define  ssdpin(i)  (i+22)      // a to pin 22, b to pin 23 etc so the pin offset is +22 

void setup() {
   
  unsigned int chanmask = (1 << PINCHAN);
  Serial.begin(9600);
  setupBlueToothConnection();
  pinMode(fir,INPUT);
  pinMode(sec,INPUT);
  pinMode(thi,INPUT);
  pinMode(G_LED,OUTPUT);
  pinMode(R_LED,OUTPUT);
  //initialize 7-segment-display's pinmode
  int j; 
  for(j = 0;j < SSDSIZE;j++){
    pinMode(ssdpin(j),OUTPUT);
  } 
   // set PIO to chan A
  REG_PIOB_PDR = chanmask;
  REG_PIOB_ABSR = chanmask;
  REG_PIOB_MDDR = chanmask;
  // set up TC
  pmc_set_writeprotect(false);    //enable write protect
  pmc_enable_periph_clk(XID);     //enable periphral clock
  squarewave(36000);              //set frequency to 36kHz
  //Initially set all parking lots to vacant 
  for(int i = 0;i < 3;i++){
    parking_lot[i] = 1;
  }
}

void loop() {
  // put your main code here, to run repeatedly
  if(blueToothSerial.available()){//if therer is input coming in from bluetooth, enable the project
   //initially set green led to low and red led to high, as we indicate the gate using LEDS
   //red led on means gate is closed and green led on means open 
   digitalWrite(R_LED,HIGH);
   digitalWrite(G_LED,LOW);
   //display the number of free parking lot on 7-segment-display
   ssddecode(no_of_free_parking_lot);
   //clear offset
   String content = blueToothSerial.readStringUntil('\n');
   //system sent greet information to customer through bluetooth;
   blueToothSerial.println("Hello, how are you?");
   blueToothSerial.println("Would you like to load your vehicle or pick up your vehicle?");
   blueToothSerial.println("Please enter load or pick up");
   //give customer 10s to operate before time out.
   delay(10000);
   //get input from customer
   content = blueToothSerial.readStringUntil('\n');
   //distinguish input and do correspnonding operation
   if(content == "load") {
    if(no_of_free_parking_lot == 0) { // if all parkling lots are occupied 
      blueToothSerial.println("No free parking lot, please wait.");
    }else {
      blueToothSerial.println("Welcome");
      //as mention above,  turn green leo on to let vehicle pass through and turn it off after 5 seconds
      digitalWrite(R_LED,LOW);
      digitalWrite(G_LED,HIGH);
      delay(5000);
      digitalWrite(G_LED,LOW);
      digitalWrite(R_LED,HIGH);
    }
    //if customer is picking up his vehicle
   }else if(content == "pick up") {
      blueToothSerial.println("Welcome");
      digitalWrite(R_LED,LOW);
      digitalWrite(G_LED,HIGH);
      delay(5000);
      digitalWrite(G_LED,LOW);
      digitalWrite(R_LED,HIGH);
      }else{ 
        //two situations: 1)time out, no operaiton is taken in 10s 2)invalid input both lead to this consequence
        blueToothSerial.println("Invalid operation or time out, try again.");
      }
  }
    //call parking monitor to update real time information;
    parkingMonitor();
 }//end of loop

//standard bluetooth set up
  void setupBlueToothConnection()
{
    blueToothSerial.begin(38400);                           // Set BluetoothBee BaudRate to default baud rate 38400
    blueToothSerial.print("\r\n+STWMOD=0\r\n");             // set the bluetooth work in slave mode
    blueToothSerial.print("\r\n+STNA=My Name is Carter \r\n");    // set the bluetooth name as "My Name is V"
    blueToothSerial.print("\r\n+STOAUT=1\r\n");             // Permit Paired device to connect me
    blueToothSerial.print("\r\n+STAUTO=0\r\n");             // Auto-connection should be forbidden here
    delay(2000);                                            // This delay is required.
    blueToothSerial.print("\r\n+INQ=1\r\n");                // make the slave bluetooth inquirable
    Serial.println("The slave bluetooth is inquirable!");
    delay(2000);                                            // This delay is required.
    blueToothSerial.flush();
}

//function to enable and initialize TC clk 
void squarewave(unsigned int freq) {
  unsigned int  tcclk = VARIANT_MCK / freq / 4;
  
  TC_Configure(XTC, XCHAN, TC_CMR_WAVE | TC_CMR_WAVSEL_UP_RC | TC_CMR_TCCLKS_TIMER_CLOCK1 | TC_CMR_ACPC_TOGGLE);
  TC_SetRC(XTC, XCHAN, tcclk);
  TC_Start(XTC, XCHAN);
}

// function of seven-segment display
void ssddecode(int a)
{
  int j;
  for(j = 0;j < SSDSIZE;j++) 
    digitalWrite(ssdpin(j), ssdtab[a][j] == 1 ? HIGH:LOW);
}

void parkingMonitor() {
  //Get data from three LED emmitor and receiver kits 
  int p1 = digitalRead(fir);
  //read data from ir receiver, which deliver 1 when it receive data and 0 when signal is blocked
  if(p1 == 0) {//vacant
    parking_lot[0] = 1;
  }else {
    parking_lot[0] = 0;
  } 
  
   int p2 = digitalRead(sec);
  if(p2 == 0) {//vacant
    parking_lot[1] = 1;
  }else {
    parking_lot[1] = 0;
  } 
  
   int p3 = digitalRead(thi);
  if(p3 == 0) {//vacant
    parking_lot[2] = 1;
  }else {
    parking_lot[2] = 0;
  } 
  //add outputs from threes kits to calculate number of free parking lot
  no_of_free_parking_lot =  parking_lot[0] +  parking_lot[1] +  parking_lot[2];
}

