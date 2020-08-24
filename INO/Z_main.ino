#ifndef INIT

#ifdef LINKY
  Linky* g_probe ;
#endif


void setup() {

  Serial.begin(115200) ;
#ifdef LINKY
  g_probe = new Linky() ;
#endif

}

void loop() {

  g_probe->mesh.update() ;
}

#endif
