template <typename T, size_t S>
class Fifo {
  public:
    Fifo(void) ;
    ~Fifo(void) ;
    bool isEmpty(void) ;
    bool isFull(void) ;
    bool push(T p_value, bool p_dropOldest=true) ;
    bool pull(T& p_value) ;
    T* operator[](size_t p_rank) ;
    T* first(void) ;
    T* last(void) ;
    String dump(String (* p_callback)(T*)) ;
    T* find(bool (*p_callback)(T*)) ;

  private:
    T _fifo[S] ;
    byte _first ;
    byte _last ;
    byte _count ;
} ;


// === ### ===


template <typename T, size_t S> Fifo<T,S>::Fifo(void) {
  this->_first = this->_last = this->_count = 0 ;
} ;


template <typename T, size_t S> Fifo<T,S>::~Fifo(void) {
  T p_value ;
  while (this->pull(p_value)) ;
} ;


template <typename T, size_t S> bool Fifo<T,S>::isEmpty(void) {
  return (this->_count == 0) ;
} ;


template <typename T, size_t S> bool Fifo<T,S>::isFull(void) {
  return (this->_count >= S) ;
} ;


template <typename T, size_t S> bool Fifo<T,S>::push(T p_value, bool p_dropOldest) {
  if (this->isFull()) {
    if (p_dropOldest)
      this->_first = (this->_first + 1) % S ; // Buffer full, drop first value
    else
      return false ;
  } else
    this->_count++ ;
debug(F("DBG : Fifo::push -> ")) ; debugln(sizeof(p_value)) ;
  this->_fifo[this->_last] = p_value ;
  this->_last = (this->_last + 1) % S ;
  return true ;
} ;


template <typename T, size_t S> bool Fifo<T,S>::pull(T& p_value) {
  if (this->isEmpty()) return false ;
  p_value = this->_fifo[this->_first] ;
  this->_first = (this->_first + 1) % S ;
  this->_count-- ;
  return true ;
} ;


template <typename T, size_t S> T* Fifo<T,S>::operator[](size_t p_rank) {
  if (p_rank >= this->_count) return NULL ;
  return &(this->_fifo[(uint8_t) (((int) this->_first + (int) p_rank) % S)]) ;
} ;


template <typename T, size_t S> T* Fifo<T,S>::first(void) {
  return this->operator[](0) ;
}


template <typename T, size_t S> T* Fifo<T,S>::last(void) {
  return (this->_count) ? this->operator[](this->_count-1) : NULL ;
}


template <typename T, size_t S> String Fifo<T,S>::dump(String (*p_callback)(T*)) {
  String l_dumpstr = "" ;

  for (byte l_rank=0 ; l_rank < this->_count ; l_rank++)
    l_dumpstr += "[" + (p_callback(this->operator[](l_rank))) + "] " ;
  if (l_dumpstr == "") l_dumpstr = F("*") ;
  return l_dumpstr ;
} ;


template <typename T, size_t S> T* Fifo<T,S>::find(bool (*p_callback)(T*)) {
  for (byte l_rank=0 ; l_rank < this->_count ; l_rank++) {
    T* l_item = this->operator[](l_rank) ;
    if (p_callback(l_item)) return l_item ;
  }
  return NULL ;
} ;
