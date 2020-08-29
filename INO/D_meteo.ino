// GY BMP 280 : température + pression

class Meteo: public Probe {
  public :
    Meteo(void) ;
    void readSensor(void) ;
    
  protected :
    bool _fillData(JsonObject& p_obj) ;

  private :
    const byte _RX_PIN = D1 ;
    uint16_t _temperature, _pressure ;
    uint16_t _light ;
    bool _available ;
} ;


Meteo::Meteo(void) : Probe(F("METEO"),TASK_MINUTE * 6) {
   this->_available = false ;
}


void Meteo::readSensor(void) {
  // lecture BMP280
  // lecture capteur lumiere
  this->_available = true ;
} ;


bool Meteo::_fillData(JsonObject& p_obj) {
  if (!this->_available) return false ;
  p_obj[F("tmp")] = this->_temperature ;
  p_obj[F("prs")] = this->_pressure ;
  p_obj[F("lit")] = this->_light ;
  this->_available = false ;
  return true ;
} ;
