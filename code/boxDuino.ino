//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define pressionePin A0
#define valoreZero 400
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
  Serial.begin(115200);
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
boolean controlloTempo = false;
unsigned long tempo;
unsigned long controlloT;
boolean tempoInizializzo = true;
boolean primaVolta = true;
boolean prendoTempo = true;
boolean primaChiusura;

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

    // controllo sul tempo
    if (controlloTempo == true) {
      controlloT = millis() - tempo;
      if ((controlloT) > 10000) {
        animazione();
        erroreAutenticazione();
      }
    }
  }
}

// errore di autenticazone per tempo o per pin errato
void erroreAutenticazione() {
  ledRossoValore(true);
  delay(1500);
  ledRossoValore(false);
  ledBluValore(true);
  valorePre = 1023;
  indice = 0;
  controlloTempo = false;
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

  if (indice == 1) {
    tempo = millis();
    controlloTempo = true;
  }

}

void autenticazione() {
  animazione();
  // controllo correttezza codice
  if (acquisito[0] > valoreZero && acquisito[1] < valoreUno && acquisito[2] < valoreDue) {
    // codice corretto apro la scatola
    ledVerdeValore(true);
    controlloTempo = false;
    verificato = true;
    inizio = false;
  } else {
    // codice errato mi rimetto in ascolto
    erroreAutenticazione();
  }
}

// apro il coperchio
void aproCoperchio() {                           
  if (verificato == true) {

    // inizializzo il tempo
    if (tempoInizializzo == true) {
      tempo = millis();
      tempoInizializzo = false;
    }
    ledGialloValore(true);
    // leggo la distanza
    distance = ultra.read();
    if (distance < 20 && distance > 3) {
      // mi muovo in base alla distanza corretta
      myStepper.step(-prendoValore(distance));
    } else if (distance > 19) {
      // coperchio aperto, aspetto che l'utente lo voglia chiudere
      ledGialloValore(false);
      ledRossoValore(false);
      ledVerdeValore(true);
      tempoInizializzo = true;
      primaVolta = true;
      verificato = false;
    }

    // controllo il tempo
    controlloT = millis() - tempo;
    if (controlloT > 10000) {
      animazione();
      erroreApertura();
    }
  }
}

// errore di apertura di tempo, provo 2 tentativi altrimenti chiudo
void erroreApertura() {
  ledRossoValore(true);
  tempoInizializzo = true;
  if (primaVolta == false) {
    verificato = false;
    lavoroFinito = true;
    primaVolta = true;
  } else {
    primaVolta = false;
  }

  delay(2000);
  // disabilitare led rosso in chiudi coperchio
}

// prendo il valore lo mappo per esere letto dallo stepper motor
int prendoValore(int posizioneCoperchio) {
  mancano = 21 - posizioneCoperchio;
  mancano = map(mancano, 0, 16, 0, 100);
  mancano = map(mancano, 0, 360, 0, 2048);
  return mancano;
}

// chiudo il coperchio
void chiudiCoperchio() {                              
  if (lavoroFinito == true) {

    if (prendoTempo == true) {
      tempo = millis();
      prendoTempo = false;
    }


    ledGialloValore(true);
    // leggo la distanza
    distance = ultra.read();
    if (distance > 5) {
      // muovo lo stepper motor della distanza corretta
      myStepper.step(prendoValoreChiudo(distance));
    } else if (distance < 5) {
      // coperchio chiuso mi rimetto in ascolto
      lavoroFinito = false;
      prendoTempo = true;
      inizio = true;
      ledGialloValore(false);
      ledRossoValore(false);
      ledBluValore(true);
      ledVerdeValore(false);
    }

// errore di tempo
    controlloT = millis() - tempo;
    if ((controlloT) > 10000 ) {
      animazione();
      erroreChiusura();
    }
  }

  }

// continuo a provare a chiudere
  void erroreChiusura() {
      ledRossoValore(true);
      delay(8000);
      prendoTempo = true;
 
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
  void animazione(){
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
  void ledBluValore(boolean veroFalso){
    if (veroFalso == true) {
      digitalWrite(blu, HIGH);
    } else {
      digitalWrite(blu, LOW);
    }
  }

  // accendere o spegnere led giallo
  void ledGialloValore(boolean veroFalso){
    if (veroFalso == true) {
      digitalWrite(giallo, HIGH);
    } else {
      digitalWrite(giallo, LOW);
    }
  }

  // accendere o spegnere led verde
  void ledVerdeValore(boolean veroFalso){
    if (veroFalso == true) {
      digitalWrite(verde, HIGH);
    } else {
      digitalWrite(verde, LOW);
    }
  }

  // accendere o spegnere led rosso
  void ledRossoValore(boolean veroFalso){
    if (veroFalso == true) {
      digitalWrite(rosso, HIGH);
    } else {
      digitalWrite(rosso, LOW);
    }
  }


  void loop(){
    acquisizione();
    aproCoperchio();
    chiudiCoperchio();
  }
