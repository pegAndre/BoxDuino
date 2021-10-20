//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define pressionePin A0
#define valoreZero 600
#define valoreUno 900
#define valoreDue 900
#define giallo 7
#define verde 6
#define rosso 5
#define blu 4
#define bottone 3

Ultrasonic ultra(12, 13);
Stepper myStepper(2048, 11, 9, 10, 8);

//setup

void setup() {
  pinMode(pressionePin, INPUT);
  pinMode(giallo, OUTPUT);
  digitalWrite(giallo, LOW);
  pinMode(verde, OUTPUT);
  digitalWrite(verde, LOW);
  pinMode(rosso, OUTPUT);
  digitalWrite(rosso, LOW);
  pinMode(blu, OUTPUT);
  digitalWrite(blu, HIGH);
  myStepper.setSpeed(10);
  attachInterrupt(digitalPinToInterrupt(bottone), segnale, FALLING);
}

//definizioni variabili

int valorePre = 1023;
int valore;
int indice = 0;
int acquisito[3];
int distance;
int mancano;
int posizione;
boolean verifica = true;
boolean inizio = true;
boolean verificato = false;
boolean lavoroFinito = false;

// acquisizione valori letti dal sensore di pressione
void acquisizione() {
  if (inizio == true) {
    delay(200);
    valore = analogRead(pressionePin);
    if (verifica == true) {
      // salvo dato solo se rispetta i parametri di acquisizione
      if (valore > valorePre && valorePre < 1000) {
        ledBluValore(false);
        ledGialloValore(true);
        salvaValore(indice, valorePre);
        verifica = false;
      }
    } else if (valore > 1000) {
      verifica = true;
    }
    valorePre = valore;
  }
}

// salvo il valore nella poszione corretta
void salvaValore(int posizione, int dato) {
  acquisito[posizione] = dato;
  if (indice == 2) {
    indice = 0;
    autenticazione();
  } else {
    indice++;
  }
}

void autenticazione() {
  animazione();
  // controllo correttezza codice
  if (acquisito[0] > valoreZero && acquisito[1] < valoreUno && acquisito[2] < valoreDue) {
    // codice corretto apro la scatola
    ledVerdeValore(true);
    verificato = true;
    inizio = false;
  } else {
    // codice errato mi rimetto in ascolto
    ledRossoValore(true);
    delay(1200);
    ledRossoValore(false);
    ledBluValore(true);
  }
}

// apro il coperchio
void aproCoperchio() {                           // manca controllo sul tempo !!!!
  if (verificato == true) {
    ledGialloValore(true);
    // leggo la distanza
    distance = ultra.read();
    if (distance < 20 && distance > 3) {
      // mi muovo in base alla distanza corretta
      myStepper.step(-prendoValore(distance));
    } else if (distance > 19) {
      // coperchio aperto, aspetto che l'utente lo voglia chiudere
      ledGialloValore(false);
      verificato = false;
    }
  }
}

// prendo il valore lo mappo per esere letto dallo stepper motor
int prendoValore(int posizioneCoperchio) {
  mancano = 21 - posizioneCoperchio;
  mancano = map(mancano, 0, 16, 0, 100);
  mancano = map(mancano, 0, 360, 0, 2048);
  return mancano;
}

// chiudo il coperchio
void chiudiCoperchio() {                              // manca controllo sul tempo !!!!
  if (lavoroFinito == true) {
    ledGialloValore(true);
    // leggo la distanza
    distance = ultra.read();
    if (distance > 5) {
      // muovo lo stepper motor della distanza corretta
      myStepper.step(prendoValoreChiudo(distance));
    } else {
      // coperchio chiuso mi rimetto in ascolto
      lavoroFinito = false;
      inizio = true;
      ledGialloValore(false);
      ledBluValore(true);
      ledVerdeValore(false);
    }
  }
}

// prendo il valore lo mappo per esere letto dallo stepper motor
int prendoValoreChiudo(int posizioneCoperchioChiuso) {
  mancano = posizioneCoperchioChiuso - 5;
  mancano = map(mancano, 0, 16, 0, 115);
  mancano = map(mancano, 0, 360, 0, 2048);
  return mancano;
}

// interrupt, il coperchio si chiuderà
void segnale() {
  lavoroFinito = true;
  delay(1200);
}

// animazione led prima della fase di autenticazione
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

// accendere o spegnere led blu
void ledBluValore(boolean veroFalso) {
  if (veroFalso == true) {
    digitalWrite(blu, HIGH);
  } else {
    digitalWrite(blu, LOW);
  }
}

// accendere o spegnere led giallo
void ledGialloValore(boolean veroFalso) {
  if (veroFalso == true) {
    digitalWrite(giallo, HIGH);
  } else {
    digitalWrite(giallo, LOW);
  }
}

// accendere o spegnere led verde
void ledVerdeValore(boolean veroFalso) {
  if (veroFalso == true) {
    digitalWrite(verde, HIGH);
  } else {
    digitalWrite(verde, LOW);
  }
}

// accendere o spegnere led rosso
void ledRossoValore(boolean veroFalso) {
  if (veroFalso == true) {
    digitalWrite(rosso, HIGH);
  } else {
    digitalWrite(rosso, LOW);
  }
}

void loop() {
  acquisizione();
  aproCoperchio();
  chiudiCoperchio();
}
