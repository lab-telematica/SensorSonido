#define TIMER1_RESOLUTION 65536UL

const int sensorPin = 3;
const long tiempo_ms = 1000;  //tiempo máximo q no detecta variaciones (resolución del sonido)

volatile unsigned long timeIni = 0, timeFin = 0;
volatile unsigned int timer1_ini = 0;
volatile bool flag = false;

unsigned int duracion=0,duracion_pasada=0;

/*FUNCIONES DE INTERRUPCIÓN*/
void sensorISR(void){
  if(digitalRead(sensorPin)){
    TCNT1 = timer1_ini;
    if(!flag){
      timeIni = millis();
      timeFin = timeIni;
      flag = true;
    }
  }
  else  timeFin = millis();
}
ISR(TIMER1_OVF_vect){
  flag = false;
}

/*CONFIGURACIÓN DEL TIMER1*/
void timer_init(unsigned long microseconds=1000000){
  const unsigned long cycles = (F_CPU / 1000000) * microseconds;
  unsigned int prescalador = 1;
  TCCR1B = 0;        // set mode as phase and frequency correct pwm, stop the timer
	TCCR1A = 0;
  if (cycles < TIMER1_RESOLUTION)              TCCR1B = _BV(CS10);
	else if (cycles < TIMER1_RESOLUTION * 8){    TCCR1B = _BV(CS11);prescalador=8;}
  else if (cycles < TIMER1_RESOLUTION * 64){   TCCR1B = _BV(CS11) | _BV(CS10);prescalador=64;}
	else if (cycles < TIMER1_RESOLUTION * 256){  TCCR1B = _BV(CS12);prescalador=256;}
	else if (cycles < TIMER1_RESOLUTION * 1024){ TCCR1B = _BV(CS12) | _BV(CS10);prescalador=1024;}
	else {                                       TCCR1B = _BV(CS12) | _BV(CS10);prescalador=1024;}
  timer1_ini = TIMER1_RESOLUTION-(microseconds*(F_CPU/1000000))/prescalador;
  TCNT1 = timer1_ini;
  TIMSK1 |= (1 << TOIE1);
}

/*CODIGO PRINCIPAL*/
void setup() {
  Serial.begin(9600);
  pinMode(sensorPin,INPUT_PULLUP);
  timer_init(tiempo_ms*1000);  //tiempo en microsegundos
  attachInterrupt(digitalPinToInterrupt(sensorPin), sensorISR, CHANGE);
}
void loop() {
  duracion = timeFin-timeIni;
  if(duracion != duracion_pasada){
    Serial.println(duracion);
    duracion_pasada = duracion;
  }
  delay(1000);
}
