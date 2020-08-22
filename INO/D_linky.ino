#include <SoftwareSerial.h>

class Linky: public Probe {
  public :
    Linky(void) ;
    void readSensor(void) ;
    
  protected :
    bool _fillData(JsonObject& p_data) ;
    
  private :
    const int _RX_PIN = D6 ;
    SoftwareSerial* _serial = NULL ;
    bool _available = false ;
    uint32_t _base, _papp ;
    uint16_t _iinst ;
    bool _readLine(String& p_label, String& p_value) ;
} ;


Linky::Linky(void) : Probe(F("LINKY")) {
  pinMode(_RX_PIN,INPUT) ;
  this->_serial = new SoftwareSerial(this->_RX_PIN,SW_SERIAL_UNUSED_PIN) ;
  this->_serial->begin(1200) ;
} ;


void Linky::readSensor(void) {
  static String l_label, l_value ;
  static uint32_t l_base ;
  static uint16_t l_iinst ;
  static uint32_t l_tstBase = 111111L ;
  static uint32_t l_tstPapp = 222222L ;
  static uint16_t l_tstIinst = 3333 ;

  if (this->testing) {
    this->_base = l_tstBase++ ;
    this->_iinst = l_tstIinst++ ;
    this->_papp = l_tstPapp++ ;
    this->_available = true ;
  } else {
    if (this->_readLine(l_label,l_value)) {
//Serial.println(l_label + "=" + l_value) ;
      if (l_label == F("BASE")) l_base = atoi(l_value.c_str()) ;
      else
        if (l_label == F("IINST")) l_iinst = atoi(l_value.c_str()) ;
        else
          if (l_label == "PAPP") {
            this->_base = l_base ;
            this->_iinst = l_iinst ;
            this->_papp = atoi(l_value.c_str()) ;
            this->_available = true ;
          }
    }
  }
} ;


bool Linky::_fillData(JsonObject& p_obj) {
  if (!this->_available) return false ;
  p_obj[F("idx")] = this->_base ;
  p_obj[F("iin")] = this->_iinst ;
  p_obj[F("pap")] = this->_papp ;
  this->_available = false ;
  return true ;
} ;


bool Linky::_readLine(String& p_label, String& p_value) {
  static const char ASCII_STX = 0x2 ;
  static const char ASCII_ETX = 0x3 ;
  static const char ASCII_CR  = 0xD ;
  static const char ASCII_LF  = 0xA ;
  static const uint8_t FRAME   = 1 ;
  static const uint8_t SPACE1  = 2 ;
  static const uint8_t SPACE2  = 3 ;
  static const uint8_t CHECKED = 4 ;
  static uint8_t l_status = 0 ;
  static char l_checksum ;

  if (_serial == NULL || !_serial->available()) return false ;
  char l_char = _serial->read() & 0x7F ; // Ignore parity bit
  switch(l_char) {
    case ASCII_STX :
      l_status = FRAME ;
      break ;
    case ASCII_ETX :
      l_status = 0 ;
      break ;
    case ASCII_LF :
      p_label = p_value = "" ;
      l_checksum = 0 ;
      if (l_status) l_status = FRAME ;
      break ;
    case ASCII_CR :
      if (l_status == CHECKED) return true ;
      break ;
    default :
      if ((l_status == SPACE2) && ((l_checksum & 0x3F) + 0x20 == l_char))
        l_status = CHECKED ;
      else {
        if (l_char == ' ') {
          if (l_status == FRAME) l_status = SPACE1 ;
          else if (l_status == SPACE1) l_status = SPACE2 ;
        } else {
          if (l_status) {
            if (l_status < SPACE1) p_label += l_char ;
            else p_value += l_char ;
          }
        }
        if (l_status < SPACE2) l_checksum += l_char ;
      }
  }
  return false ;
} ;


/*
bool Linky::_readLine(String& p_label, String& p_value) {
  static uint8_t l_cpt = 0 ;
  static uint32_t l_base = 100 ;
  static uint16_t l_iinst = 200 ;
  static uint32_t l_papp = 300 ;

  l_cpt++ ;
  switch(l_cpt) {
    case 5 :
      p_label = F("BASE") ; p_value = String(l_base) ;
      l_base += 20 ;
      return true ;
    case 105 :
      p_label = F("IINST") ; p_value = String(l_iinst) ;
      l_iinst += 10 ;
      return true ;
    case 255 :
      p_label = F("PAPP") ; p_value = String(l_papp) ;
      l_papp += 30 ;
      return true ;
    default :
      return false ;
  }
}
*/
