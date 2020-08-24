//#define INIT
#define LINKY
#define WANTDEBUG

#include <EEPROM.h>
#include <ArduinoOTA.h>
#include <painlessMesh.h>

/*
const uint32_t CHIP1_ID = 0x00000000 ;
const uint32_t CHIP2_ID = 0x00e67e71 ;
const uint32_t CHIP3_ID = 0x00000000 ;
const uint32_t CHIP4_ID = 0x00e3c664 ;
const uint32_t CHIP5_ID = 0x00e5e70d ;
const uint32_t CHIP6_ID = 0x00e5e76b ;

#define BRIDGE CHIP2_ID
#define LINKY  CHIP4_ID
#define GASPAR CHIP5_ID
#define METEO  CHIP6_ID
*/

const size_t PROBES_COUNT = 4 ;

/*
const char CHIP2_NAME[] PROGMEM = "BRIDGE" ;
const char CHIP4_NAME[] PROGMEM = "LINKY" ;
const char CHIP5_NAME[] PROGMEM = "GASPAR" ;
const char CHIP6_NAME[] PROGMEM = "METEO" ;
*/

const char DISCOVER_REQUEST_STRING[] PROGMEM = "dcr" ;
const char DISCOVER_ANSWER_STRING[] PROGMEM = "dca" ;
const char DATA_REQUEST_STRING[] PROGMEM = "dtr" ;
const char DATA_ANSWER_STRING[] PROGMEM = "dta" ;
const char STORE_REQUEST_STRING[] PROGMEM = "str" ;
const char OTA_REQUEST_STRING[] PROGMEM = "otr" ;
const char TEST_REQUEST_STRING[] PROGMEM="tsr" ;
const char MESSAGE_NUMBER_STRING[] PROGMEM = "num" ;
const char FROM_NAME_STRING[] PROGMEM = "frm" ;

enum class MsgType { DISCOVER_REQUEST, DISCOVER_ANSWER, DATA_REQUEST, DATA_ANSWER, STORE_REQUEST, OTA_REQUEST, TEST_REQUEST } ;

struct CONNECTION_T {
  String netName ;
  String netPassword ;
} ;

struct NETWORK_T {
  CONNECTION_T mesh ;
  CONNECTION_T wifi ;
} ;

struct MESSAGE_T {
  uint32_t chipId ;
  String message ;
} ;

 struct ITEM_T {
   uint32_t id ;
   String name ;
   uint8_t msgNum ;
 } ;


#ifdef WANTDEBUG
  #define debug(s) Serial.print(s)
  #define debugln(s) Serial.println(s)
#else
  #define debug(s) {}
  #define debugln(s) {}
 #endif
