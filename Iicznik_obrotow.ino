
#include <Nextion.h>

#define window_size 15  //rozmiar tablicy sredniej kroczacej
int window[window_size] = {0};

const int ignitionPin = 2;
const int ignitionInterrupt = 0;
const unsigned int pulsesPerRev = 1;

unsigned long lastPulseTime = 0;
unsigned long rpm = 0;
int rpm_int;
int rpm_to_disp;



NexButton b1 = NexButton(0, 3, "b1");  // dodaj przycisk
NexButton b2 = NexButton(0, 5, "b2");  // dosaj przycisk
NexButton b0 = NexButton(3, 4, "b0");  //przycisk do powrotu

NexPage page1 = NexPage(0, 0, "page1");  //dodaj strone
NexPage page3 = NexPage(3, 0, "page3");

NexTouch *nex_listen_list[] = 
{
  &b1,  // przycisk dodany
  &b2,  
  &b0,
  &page1,  //storna dodana
  &page3,
  NULL  // zakoncz łancuch 
};  


bool back = false;
void b0PopCallback(void *ptr)             //zbocze opadające przycisku "wroc" zmienia stan zmiennej back
{
    back = true;
    delay(2);
}  

bool f4 = false;
void b2PopCallback(void *ptr)             //zbocze opadajace przycisku czterosuw
{
    f4 = true;
    delay(2);
}  

bool t2 = false;
void b1PopCallback(void *ptr)             //zbocze opadajace przycisku dwusuw
{
    t2 = true;
    delay(2);
}  

void buttons(int val)          //obsługa przycisków wyświetlacza
{
    if(back == true)           //po wybraniu 'wroc' zresetuj poprzednie wartosci 
  { 
      f4 = false;
      t2 = false;
      back = false;
  }
  if(f4 == true)                      //jesli wybrany 'czterosuw'  
  {
      int val_4 = val + val;          //podwojenie wartosci val ze wzgledu na charakterystyke pracy czerosuwa
      Serial.print("n0.val=");        //nazwa zmiennej w pamieci wyswietlacza
      Serial.print(val_4);            //przesylana wartosc
      Serial.write(0xff);             //wyslij
      Serial.write(0xff);
      Serial.write(0xff);       
  }
 
    if(t2 == true)                       //jesli wybrany dwusuw
  {
      Serial.print("n0.val=");        //nazwa zmiennej w pamieci wyswietlacza     
      Serial.print(val);              //przesylana wartosc
      Serial.write(0xff);             //wyslij
      Serial.write(0xff);
      Serial.write(0xff);       
  }

}

void ignitionIsr()
{
  unsigned long now = micros();
  unsigned long interval = now - lastPulseTime;
  if (interval > 5000)
  {
     rpm = 60000000UL/(interval * pulsesPerRev);
     lastPulseTime = now;
     //rpm_int=int(rpm);
  }  
  
}
int voltage()
{
  int val = analogRead(A0);
  int B = (((val * 0.00465625) * 3.05264)*10)-2; //wejscie * wartosc napiecia po dzielniku na 1pkt * przekładnia
  return B;
}

void push(int val)   //funkcja wpisujaca dane wejsciowe do tablicy, dopisuje nowe dane w lewo
{
  int i = 0;
  for (i=1 ; i < window_size ; i++)
  {
    window[i-1] = window[i];
  }
  window[window_size - 1] = val;
}

int take_avg()      //average //oblicza srednia
{
  long sum = 0;
  int i = 0;
  for (i=0 ; i<window_size ; i++)
  {
    sum += window[i];
  }
  return sum/window_size;
}

void setup() 
{
  Serial.begin(9600);
  delay(500);
  Serial.print("baud=115200");  //zmiana predkosci komunikacji
  Serial.write(0xff);  
  Serial.write(0xff);
  Serial.write(0xff);
  Serial.end();  
  Serial.begin(115200);  
  
    b1.attachPop(b1PopCallback);  // Button relase
    b2.attachPop(b2PopCallback);  // Button relase
    b0.attachPop(b0PopCallback);  // Button relase
 

  pinMode(ignitionPin, INPUT);
  attachInterrupt(ignitionInterrupt, &ignitionIsr, RISING);       //ustawienie przerwan na pinie numer dwa
}



void loop() 
{
  noInterrupts();       //wyłacz przerwania
  rpm_to_disp=int(rpm);
  interrupts();        //włacz przerwania
 
   
int cur_sample = 0;

cur_sample = rpm_to_disp;
delay(200);
push(cur_sample);
buttons((take_avg()/10)*10);
//Serial.println((take_avg()/10)*10);

  
 Serial.print("x0.val=");    //wysłanie wartosci napiecia do wyswietlacza
 Serial.print(voltage());
 Serial.write(0xff);
 Serial.write(0xff);
 Serial.write(0xff);
  
 nexLoop(nex_listen_list);  // Sprawdzenie czy któryś przycisk został wyzwolony 


}
