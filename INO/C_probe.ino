class Probe {
  public :
    Probe(const String p_name, const unsigned long p_loopSensor) ;
    bool newMessage(MsgType p_msgtype, uint32_t p_toChipId) ;
    void loopSend(void) ;
    void receiveCallback(uint32_t p_fromChipId, String& p_message) ;
    void loopReceived(void) ;
    virtual void readSensor(void) ;
    Scheduler sched ;
    painlessMesh mesh ;
     
  protected :
    static const int _CAPACITY = JSON_OBJECT_SIZE(4) + JSON_OBJECT_SIZE(3) + 30 ;
    const int _MESHPORT = 5555 ;
    String _name ;
    NETWORK_T _network ;
    bool testing ;
    Task* _taskLoopSend ;
    Task* _taskLoopReceived ;
    Task* _taskLoopSensor ;
    void readConfig(void) ;
    virtual bool _fillData(JsonObject& p_json) ;

  private :
    Fifo<MESSAGE_T,4> _sndBuffer ;
    Fifo<MESSAGE_T,4> _rcvBuffer ;
    Fifo<ITEM_T,PROBES_COUNT> _items ;
    ITEM_T* _itemById(const uint32_t p_id) ;
    ITEM_T* _itemByName(String p_name) ;
 } ;

// === ### ===

Probe::Probe(const String p_name, const unsigned long p_loopSensor) {
  EEPROM.begin(sizeof(NETWORK_T)) ;
  eemem::readBytes((uint8_t*)(&this->_network), sizeof(NETWORK_T)) ;
  this->_name = p_name ;
//  this->testing = false ;
  this->testing = true ;

  this->_taskLoopSend = new Task(TASK_SECOND*1, TASK_FOREVER, [this](void)->void { this->loopSend(); }) ;
  this->_taskLoopReceived = new Task(TASK_SECOND*1, TASK_FOREVER, [this](void)->void { this->loopReceived(); }) ;
  this->_taskLoopSensor = new Task(p_loopSensor, TASK_FOREVER, [this](void)->void { this->readSensor(); }) ;

  this->mesh.setDebugMsgTypes( ERROR | MESH_STATUS | CONNECTION | SYNC | COMMUNICATION | GENERAL | MSG_TYPES | REMOTE ) ;
  this->mesh.init(this->_network.mesh.netName, this->_network.mesh.netPassword, &(this->sched), this->_MESHPORT) ;
  this->mesh.onReceive([this](uint32_t frm,String& msg)->void { this->receiveCallback(frm,msg); }) ;

  this->sched.addTask(*(this->_taskLoopSend)) ;
  this->sched.addTask(*(this->_taskLoopReceived)) ;
  this->sched.addTask(*(this->_taskLoopSensor)) ;
  this->_taskLoopSend->enableDelayed(TASK_SECOND * 15) ; // wait for mesh to setup
  this->_taskLoopReceived->enableDelayed(TASK_SECOND * 15) ; // wait for mesh to setup
  this->_taskLoopSensor->enable() ;

  this->newMessage(MsgType::DISCOVER_REQUEST,0) ;
  debugln(F("DBG : Init OK")) ;
} ;


bool Probe::newMessage(MsgType p_msgtype, const uint32_t p_toChipId=0L) {
  static uint8_t l_msgNum = 0 ;
  StaticJsonDocument<Probe::_CAPACITY> l_json ;
  JsonObject l_obj ;
  MESSAGE_T l_msg ;

  debugln(F("DBG : New message")) ;
  l_msgNum++ ;
  l_json[FPSTR(FROM_NAME_STRING)] = this->_name ;
  l_json[FPSTR(MESSAGE_NUMBER_STRING)] = l_msgNum ;
  switch(p_msgtype) {
    case MsgType::DISCOVER_REQUEST :
      l_obj = l_json.createNestedObject(FPSTR(DISCOVER_REQUEST_STRING)) ;
      break ;
    case MsgType::DISCOVER_ANSWER :
      l_obj = l_json.createNestedObject(FPSTR(DISCOVER_ANSWER_STRING)) ;
      break ;
    case MsgType::DATA_REQUEST :
      l_obj = l_json.createNestedObject(FPSTR(DATA_REQUEST_STRING)) ;
      break ;
    case MsgType::DATA_ANSWER :
      l_obj = l_json.createNestedObject(FPSTR(DATA_ANSWER_STRING)) ;
      if (!this->_fillData(l_obj)) return false ;
      break ;
    case MsgType::STORE_REQUEST :
      l_obj = l_json.createNestedObject(FPSTR(STORE_REQUEST_STRING)) ;
      break ;
    case MsgType::OTA_REQUEST :
      l_obj = l_json.createNestedObject(FPSTR(OTA_REQUEST_STRING)) ;
      break ;
    case MsgType::TEST_REQUEST :
      l_obj = l_json.createNestedObject(FPSTR(TEST_REQUEST_STRING)) ;
      break ;
  }
  serializeJson(l_json, l_msg.message) ;
  debug(F("newMessage : ")) ; debugln(l_msg.message) ;
  l_msg.chipId = p_toChipId ;
  return this->_sndBuffer.push(l_msg) ;
} ;


void Probe::readSensor(void) { debugln(F("DBG : Probe::readSensor")) ; } ;


bool Probe::_fillData(JsonObject& p_obj) {} ;


void Probe::loopSend(void) {
  static const int8_t SEND_REPEAT = 5 ;
  static uint8_t l_repeat = 0 ;

  debugln(F("DBG : loopSend")) ;
  String dbgs = this->_sndBuffer.dump([](MESSAGE_T* p_msg)->String { return (String) p_msg->chipId + "/" + p_msg->message ; }) ;
  debugln(dbgs) ;
//  debugln((String) (this->_sndBuffer[0]->chipId + "/" + this->_sndBuffer[0]->message)) ;
  MESSAGE_T* l_msg = this->_sndBuffer.first() ;
  if (!l_msg) return ; // Buffer empty
  if (!l_repeat) l_repeat = SEND_REPEAT ;
  if (l_msg->chipId)
    this->mesh.sendSingle(l_msg->chipId, l_msg->message) ;
  else
    this->mesh.sendBroadcast(l_msg->message) ;
  l_repeat-- ;
  if (!l_repeat) this->_sndBuffer.pull(*l_msg) ; // No more repeat, drop message from buffer
} ;


ITEM_T* Probe::_itemById(const uint32_t p_id) {
  static uint32_t l_id ;

  l_id = p_id ;
  return this->_items.find([](ITEM_T* p_item)->bool {
    return (l_id && (p_item->id == l_id)) ;
  }) ;
} ;


/*
ITEM_T* Probe::_itemByName(String p_name) {
  static String l_name ;

  l_name = p_name ;
  return this->_items.find([](ITEM_T* p_item)->bool {
    return (p_item->name == l_name) ;
  }) ;
} ;
*/


void Probe::receiveCallback(uint32_t p_fromChipId, String& p_message) {
  debugln(F("DBG : receiveCallback")) ;
  this->_rcvBuffer.push(MESSAGE_T {p_fromChipId, p_message}) ;
}


void Probe::loopReceived(void) {
  MESSAGE_T l_msg ;
  StaticJsonDocument <Probe::_CAPACITY> l_json ;
  ITEM_T* l_item ;

  debugln(F("DBG : loopReceived")) ;
  if (!this->_rcvBuffer.pull(l_msg)) return ;
  deserializeJson(l_json, l_msg.message) ;

  // Check if probe is already known
  if (l_item = this->_itemById(l_msg.chipId)) {
    uint8_t l_msgNum = l_json[FPSTR(MESSAGE_NUMBER_STRING)] ;
    // Known and message already received -> ignore message
    if (l_msgNum == l_item->msgNum) return ;
    debugln(F("DBG : not dropped")) ;
    l_item->msgNum = l_msgNum ;
  } else {
    // Unknown -> store probe items
    ITEM_T l_new = {l_msg.chipId, l_json[FPSTR(FROM_NAME_STRING)], l_json[FPSTR(MESSAGE_NUMBER_STRING)]} ;
    // Ignore message if item buffer is already full
    if (!this->_items.push(l_new)) return ; 
  }
  
  // MANAGE MESSAGE
  if (l_json.containsKey(FPSTR(DISCOVER_REQUEST_STRING)))
    this->newMessage(MsgType::DISCOVER_ANSWER,l_msg.chipId) ;
  else if (l_json.containsKey(FPSTR(DATA_REQUEST_STRING)))
    this->newMessage(MsgType::DATA_ANSWER,l_msg.chipId) ;
  else if (l_json.containsKey(FPSTR(STORE_REQUEST_STRING))) ;
  else if (l_json.containsKey(FPSTR(OTA_REQUEST_STRING))) ;
  else if (l_json.containsKey(FPSTR(TEST_REQUEST_STRING)))
    this->testing = !this->testing ;
} ;
