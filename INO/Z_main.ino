void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200) ;
  Serial.println(ESP.getChipId()) ;
}

void loop() {
  static Linky lk ;

  lk.newMessage(MsgType::DATA_REQUEST) ;
  lk.loopSend() ;
  delay(200) ;

}
