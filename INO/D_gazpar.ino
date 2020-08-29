class Gazpar: public Probe {
  public :
    Gazpar(void) ;
    void readSensor(void) ;
    static void newPulse(void) ;
    static volatile int16_t pulseCount ;
    
  protected :
    bool _fillData(JsonObject& p_obj) ;

  private :
    const byte _RX_PIN = D1 ;
} ;


volatile int16_t Gazpar::pulseCount ;


Gazpar::Gazpar(void) : Probe(F("GAZPAR"), TASK_MINUTE * 5) {
  Gazpar::pulseCount = -1 ;
  pinMode(_RX_PIN,INPUT_PULLUP) ;
  attachInterrupt(digitalPinToInterrupt(_RX_PIN), Gazpar::newPulse, FALLING) ;
} ;


void Gazpar::newPulse(void) {
  if (Gazpar::pulseCount == -1) pulseCount = 0 ;
  Gazpar::pulseCount++ ;
} ;


bool Gazpar::_fillData(JsonObject& p_obj) {
  if (Gazpar::pulseCount == -1) return false ;
  p_obj[F("dm3")] = Gazpar::pulseCount ;
  Gazpar::pulseCount = 0 ;
  return true ;
}
