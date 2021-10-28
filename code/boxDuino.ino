//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define pressionePin A0
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
  settaggioSequenza();
}

//definizioni variabili
int valorePre = 1023;
int valore;
int indice = 0;
int distance;
int mancano;
int posizione;
int volte = 0;
int indiceInizio = 0;
int acquisito[4];
int pin[8];
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
boolean miStoAllenando = true;


void settaggioSequenza() {
  while (indiceInizio < 7) {
    delay(100);
    valore = analogRead(pressionePin);
    if (verifica == true) {
      // salvo dato solo se rispetta i parametri di acquisizione
      if (valore > valorePre && valorePre < 1000) {
        led(1, false);
        salvoCombinazione(valorePre);
        verifica = false;
      }
    } else if (valore > 1000) {
      verifica = true;
    }
    valorePre = valore;
  }
  allenamento();
  miStoAllenando = false;
  led(1, true);
}

// Salvo i range che l'utente ha impostato
void salvoCombinazione(int dato) {
  animazione();
  pin[indiceInizio] = dato - 150;
  indiceInizio++;
  pin[indiceInizio] = dato + 150;
  indiceInizio++;
}

// Allenamento da parte dell'utente per impostare la combinazione
// Deve effettuarla correttamente 2 volte
// Se sbaglia i led gli mostrano che errore ha fatto e se ha impresso troppa o troppa poca forza
void allenamento() {
  animazione();

  while (volte < 2) {
    acquisizione();
    if (verificato == true) {
      delay(800);
      verificato = false;
      inizio = true;
      volte++;
      led(3, false);
      led(1, true);
    }
  }
  animazione();
  led(1, true);
}


// acquisizione valori letti dal sensore di pressione
void acquisizione() {
  if (inizio == true) {
    delay(100);
    valore = analogRead(pressionePin);
    Serial.println(valore);
    if (verifica == true) {
      // salvo dato solo se rispetta i parametri di acquisizione
      if (valore > valorePre && valorePre < 1000) {
        led(1, false);
        led(4, true);
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
      if ((controlloT) > 100000) {
        animazione();
        erroreAutenticazione();
      }
    }
  }
}

// errore di autenticazone per tempo o per pin errato
void erroreAutenticazione() {
  led(2, true);
  delay(1500);
  led(2, false);
  led(1, true);
  valorePre = 1023;
  indice = 0;
  controlloTempo = false;
}

// salvo il valore nella poszione corretta
void salvaValore(int posizione, int dato) {
  acquisito[posizione] = dato;
  if (indice == 3) {
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
  if (acquisito[0] > pin[0] && acquisito[0] < pin[1] && acquisito[1] > pin[2] && acquisito[1] < pin[3] && acquisito[2] > pin[4] && acquisito[2] < pin[5] && acquisito[3] > pin[6] && acquisito[3] < pin[7]) {
    // codice corretto apro la scatola
    led(3, true);
    controlloTempo = false;
    verificato = true;
    inizio = false;
  } else {
    // codice errato mi rimetto in ascolto
    if (miStoAllenando == true) {
      led(2, true);
      delay(1500);
      led(2, false);
      delay(1000);
      mostramiErrore();
    }
    delay(1000);
    erroreAutenticazione();
  }
}


// viene mostrato l'erorre all'utente tramite i led
void mostramiErrore() {
  boolean erroreUno = false;
  boolean erroreDue = false;
  boolean erroreTre = false;
  boolean erroreQuattro = false;
  boolean errorePrimo = false;
  boolean erroreSecondo = false;
  boolean erroreTerzo = false;
  boolean erroreQuarto = false;

  // ptima viene mostarto quali valori ha sbagliato, il primo led raprresenta il primo valore, il secondo led....
  if (acquisito[0] < pin[0] || acquisito[0] > pin[1]) {
    led(1, true);
    erroreUno = true;
    if (acquisito[0] < pin[0]) {
      errorePrimo = true;
    }
  }

  if (acquisito[1] < pin[2] || acquisito[1] > pin[3]) {
    led(2, true);
    erroreDue = true;
    if (acquisito[1] < pin[2]) {
      erroreSecondo = true;
    }
  }

  if (acquisito[2] < pin[4] || acquisito[2] > pin[5]) {
    led(3, true);
    erroreTre = true;
    if (acquisito[2] < pin[4]) {
      erroreTerzo = true;
    }
  }

  if (acquisito[3] < pin[6] || acquisito[3] > pin[7]) {
    led(4, true);
    erroreQuattro = true;
    if (acquisito[3] < pin[6]) {
      erroreQuarto = true;
    }
  }

  delay(1000);
  animazione();
  delay(1000);

  // verrà poi acceso il led che raprresenta il valore sbagliato e successivamente o led verde(troppo poco forte) o led rosso(troppo forte)
  sbagliato(erroreUno, 1, errorePrimo);
  sbagliato(erroreDue, 2, erroreSecondo);
  sbagliato(erroreTre, 3, erroreTerzo);
  sbagliato(erroreQuattro, 4, erroreQuarto);

}


void sbagliato(boolean errore,int colore, boolean sopraOSotto) {
  if (errore == true) {
    errore = false;
    led(colore, true);
    delay(1000);
    led(colore, false);
    delay(1000);

    if(sopraOSotto == true){
      led(2, true);
      sopraOSotto = false;
    }else{
      led(3, true);
    }
    
    delay(1000);
    animazione();
    delay(1000);
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
    led(4, true);
    // leggo la distanza
    distance = ultra.read();
    if (distance < 20 && distance > 3) {
      // mi muovo in base alla distanza corretta
      myStepper.step(-prendoValore(distance));
    } else if (distance > 19) {
      // coperchio aperto, aspetto che l'utente lo voglia chiudere
      led(4, false);
      led(2, false);
      led(3, true);
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
  led(2, true);
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
    led(4, true);
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
      led(4, false);
      led(2, false);
      led(1, true);
      led(3, false);
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
  led(2, true);
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
void led(int colore, boolean veroFalso) {
  switch (colore) {
    case 1:
      if (veroFalso == true) {
        digitalWrite(blu, HIGH);
      } else {
        digitalWrite(blu, LOW);
      }
      break;

    case 2:
      if (veroFalso == true) {
        digitalWrite(rosso, HIGH);
      } else {
        digitalWrite(rosso, LOW);
      }
      break;

    case 3:
      if (veroFalso == true) {
        digitalWrite(verde, HIGH);
      } else {
        digitalWrite(verde, LOW);
      }
      break;

    case 4:
      if (veroFalso == true) {
        digitalWrite(giallo, HIGH);
      } else {
        digitalWrite(giallo, LOW);
      }
      break;
  }
}


void loop() {
  acquisizione();
  aproCoperchio();
  chiudiCoperchio();
}
