namespace eemem {
  void readBytes(byte* p_to, int p_count, int p_start) ;
  void writeBytes(const byte* p_from, int p_count, int p_start) ;
}


// === ### ===

namespace eemem {

  void readBytes(byte* p_to, int p_count, int p_start=0) {
    for (int l_i=0 ; l_i<p_count ; l_i++)
      p_to[l_i] = EEPROM.read(l_i+p_start) ;
  }


  void writeBytes(const byte* p_from, int p_count, int p_start=0) {
    for (int l_i=0 ; l_i<p_count ; l_i++)
      EEPROM.write(l_i+p_start,p_from[l_i]) ;
    EEPROM.commit() ;
  }

}
