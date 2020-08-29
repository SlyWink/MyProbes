#ifndef INIT

#ifdef LINKY
  Linky* g_probe ;
#elif defined GAZPAR
  Gazpar* g_probe ;
#elif defined METEO
  Meteo* g_probe ;
#endif


void setup() {

  Serial.begin(115200) ;
#ifdef LINKY
  g_probe = new Linky() ;
#elif defined GAZPAR
  g_probe = new Gazpar() ;
#elif defined METEO
  g_probe = new Meteo() ;
#endif

}

void loop() {

  g_probe->mesh.update() ;
}

#endif
