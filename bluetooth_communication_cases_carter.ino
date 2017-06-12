void setup() {
  // put your setup code here, to run once:

}

void parkingMonitor() {
  int p1 = digitalRead(13);//parking lot number 1 (not sure yet)
  int p2 = digitalRead(14);//parking lot number 2 (not sure yet)
  if(p1 == 1) {//empty 
    parking_lot[0] = 1;
  }else {
    parking_lot[0] = 0;
  }
  if(p2 == 1) {//empty    
    parking_lot[1] = 1;
  }else {
    parking_lot[1] = 0;
  }
  no_of_free_parking_lot = parking_lot[0] + parking_lot[1];  
}
void loop() {
  static unsigned i;
  String content = "";
  //initially The entries is closed. Red ON,Ge
  digitalWrite(R_LED,HIGH);
  digitalWrite(G_LED,LOW);
  parkingMonitor();
  ssddecode(no_of_free_parking_lot); // Display free spot on 7 segment display

}
