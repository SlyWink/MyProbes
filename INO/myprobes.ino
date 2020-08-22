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
const char CHIP2_NAME[] PROGMEM = "BRIDGE" ;
const char CHIP4_NAME[] PROGMEM = "LINKY" ;
const char CHIP5_NAME[] PROGMEM = "GASPAR" ;
const char CHIP6_NAME[] PROGMEM = "METEO" ;

struct MESSAGE_T {
  uint32_t chipId ;
  String message ;
} ;

enum class MsgType { DISCOVER_REQUEST, DISCOVER_ANSWER, DATA_REQUEST, DATA_ANSWER, STORE_REQUEST, OTA_REQUEST, TEST_REQUEST } ;
