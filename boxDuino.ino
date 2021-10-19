#include <Stepper.h>                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             #include <Stepper.h>
#include <Ultrasonic.h>

#define pressionePin A0
#define giallo 7
#define verde 6
#define rosso 5
#define blu 4
#define valoreZero 300
#define valoreUno 700
#define valoreDue 700
#define bottone 3

Stepper myStepper(2048, 11, 9, 10, 8);
Ultrasonic ultra(12, 13);

void setup() {
  Serial.begin(115200);
  
  pinMode(pressionePin, INPUT);
  pinMode(giallo, OUTPUT);
  digitalWrite(giallo, LOW);
  pinMode(verde, OUTPUT);
  digitalWrite(verde, LOW);
  pinMode(rosso, OUTPUT);
  digitalWrite(rosso, LOW);
  pinMode(blu, OUTPUT);
  digitalWrite(blu, LOW);
  
  attachInterrupt(digitalPinToInterrupt(bottone), segnale, FALLING);
  
  myStepper.setSpeed(10);
}

int valorePre = 1023;
int i = 0;
int acquisito[3];
int distance;
int mancano;
int posizione;
boolean verifica = true;
boolean ledBlu = true;
boolean verificato = false;
boolean lavoroFinito = false;
boolean inizio = true;

void acquisizione() {       // manca controllo sul tempo !!!!
  if (inizio == true) {      
    // acquisisco valore dal sensore di pressione
    int valore = analogRead(pressionePin);
    Serial.println(valore);
    // controllo che non ci siano errori di acquisizione
    if (verifica == true) {
      // se il valore rispetta i requisiti ovvero cambia la sua crescita e non è un errore della resistenza salvo il dato
      if (valore > valorePre && valorePre < 1000) {
        // led giallo identifica iniziata la rilevazione
        digitalWrite(giallo, HIGH);
        // spengo il led blu
        digitalWrite(blu, LOW);
        ledBlu = false;
        // salvo il valore passando come parametri il valore e a che posizione inserirlo
        salvaValore(i, valorePre);
        // resetto il controllo
        verifica = false;
      }
      // riprendo a salvare il valore solo quando la resistenza è tornata al livello iniziale
    } else if (valore > 1000) {
      verifica = true;
    }

    valorePre = valore;
  }
}

void salvaValore(int posizione, int dato) {
  // salvao il dato nella posizione corretta
  acquisito[posizione] = dato;

  // se ho raggiunto le 3 acquisizioni le controllo
  if (i == 2) {
    // azzero il contatore
    i = 0;
    // fine acquisizione
    digitalWrite(giallo, LOW);
    autenticazione();
  } else {
    // aumento la posizione
    i++;
  }
}

void autenticazione() {
  animazione();
  if (acquisito[0] < valoreZero && acquisito[1] < valoreUno && acquisito[2] < valoreDue) {
    // se corretta led verde che identifica l'accesso
    digitalWrite(verde, HIGH);
    verificato = true;
    inizio = false;
  } else {
    // codice errato, riprendo ad aspettare la sequnza, led rosso
    digitalWrite(rosso, HIGH);
    delay(1200);
    digitalWrite(rosso, LOW);
    digitalWrite(blu, HIGH);
    ledBlu = true;
  }
}

void animazione() {
  digitalWrite(blu, HIGH);
  digitalWrite(rosso, HIGH);
  digitalWrite(verde, HIGH);
  digitalWrite(giallo, HIGH);
  delay(1000);
  digitalWrite(blu, LOW);
  digitalWrite(verde, LOW);
  digitalWrite(rosso, LOW);
  digitalWrite(giallo, LOW);
}

void aproCoperchio() {  ///////////////////////////////////////////////////////////metto il whilww con timeout
  digitalWrite(giallo, HIGH);
  if (verificato == true) {
    // acquisiisco la distanza di quanto devo spostarmi
    posizione = prendoValore();
    // mi muovo di posizione
    myStepper.step(-posizione);
    //controllo se sono in posizione corretta
    distance = ultra.read();
    if (distance > 20) {
      digitalWrite(giallo, LOW);
      verificato = false;
    }
  }

}

int prendoValore() {
  // leggo dal sensore ultraSuoni
  distance = ultra.read();


  if (distance < 21) {
    // se la distanza è nel range corretto, calcolo quanto mi manca da spostarmi
    mancano = 21 - distance;
    // mappo il valore da cm in gradi dove 115 gradi e il mio range di spostamento massimo
    mancano = map(mancano, 0, 17, 0, 115);
    // mappo il valore, ne numeri letti dallo steppr
    mancano = map(mancano, 0, 360, 0, 2048);
  } else {
    // non devo spostarmi
    mancano = 0;
  }
  return mancano;
}

void chiudoCoperchio() {
  if (lavoroFinito == true) {
    digitalWrite(giallo, HIGH);
    distance = ultra.read();
    if (distance > 5) {
      mancano = distance - 5;
      mancano = map(mancano, 0, 17, 0, 115);
      mancano = map(mancano, 0, 360, 0, 2048);
      myStepper.step(mancano);
    } else {
      lavoroFinito = false;
      digitalWrite(verde, LOW);
      digitalWrite(giallo, LOW);
      inizio = true;
      ledBlu = true;
    }
  }
}

void segnale() {
  lavoroFinito = true;
}


void loop() {
  //led blu identifica che è pronto ad acquisire
  if (ledBlu == true) {
    digitalWrite(blu, HIGH);
  }
  // avvio acquisizione
  acquisizione();
  aproCoperchio();
  chiudoCoperchio();
  delay(100);
}
