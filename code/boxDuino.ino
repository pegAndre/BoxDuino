//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define rosso 5
#define verde 6
#define giallo 3
#define blu 9
#define sensorePressione A0
#define bottoneEsterno 2
#define cambioCodice 7

#define accensioneMacchina 0
#define scatolaChiusa 1
#define inApertura 2
#define scatolaAperta 3
#define cambioSequenza 4
#define inChiusura 5

Ultrasonic ultra(12, 13);
Stepper myStepper(2048, 11, 4, 10, 8);

void setup() {
  Serial.begin(115200);
  pinMode(bottoneEsterno, INPUT);
  pinMode(cambioCodice, INPUT);
  pinMode(sensorePressione, INPUT);
  pinMode(rosso, OUTPUT);
  pinMode(verde, OUTPUT);
  pinMode(giallo, OUTPUT);
  pinMode(blu, OUTPUT);
}

//VARIABILI
const int salita = -1;
const int discesa = 1;
const int range = 150;
int combinazione[4];
int sequenza[4];
byte stato = accensioneMacchina;
boolean settoTempo = true;
unsigned long tempo;

//FUNZIONI
void acquisizione(int sequenza[]) {   //Acquisisce 4 pressioni sull'arry indicato
  int indiceSequenza = 0;
  int valoreLetto;

  while (indiceSequenza < 4) {
    delay(50);
    valoreLetto = analogRead(sensorePressione);
    illuminaSequenza(valoreLetto);  // feedback visivo utente della pressione
    
    if (digitalRead(bottoneEsterno)) {  // se il bottone è premuto salvo il valore
      sequenza[indiceSequenza] = valoreLetto;
      indiceSequenza++;
      delay(2000); // mostro all'utente la pressione con i led
    }
  }
  spegniTuttiLed();
}

void autenticazione() {
  acquisizione(sequenza);     //acquisizione della sequenza
  if (verifica(sequenza)) {   //verifica della combinazione, se errata lo stato non cambia
    stato = inApertura;
  }
}

//VERIFICA LA COMBINAZIONE INSERITA
boolean verifica(int sequenza[]) {
  animazioneVerifica();
  animazioneVerifica();
  if (sequenza[0] > (combinazione[0] - range) && sequenza[0] < (combinazione[0] + range) && sequenza[1] > (combinazione[1] - range) && sequenza[1] < (combinazione[1] + range) && sequenza[2] > (combinazione[2] - range) && sequenza[2] < (combinazione[2] + range) && sequenza[3] > (combinazione[3] - range) && sequenza[3] < (combinazione[3] + range)) {
    digitalWrite(verde, HIGH);  //LOGGATTO
    delay(600);
    return true;
  } else {
    sequenzaErrata();           //SEQUENZA ERRATA
    return false;
  }
}

void illuminaSequenza(int valorePressione) {
  int luminosita; // La luminosità di ciascun led dipende dalla pressione letta dal sensore
  if (valorePressione > 600) {
    luminosita = map(valorePressione, 1023, 700, 0, 255);
    accendoLedAnalogico(luminosita, 0, 0, 0);
  } else if (valorePressione > 400) {
    luminosita = map(valorePressione, 680, 480, 0, 255);
    accendoLedAnalogico(255, luminosita, 0, 0);
  } else if (valorePressione > 200) {
    luminosita = map(valorePressione, 480, 350, 0, 255);
    accendoLedAnalogico(255, 255, luminosita, 0);
  } else {
    luminosita = map(valorePressione, 350, 0, 0, 255);
    accendoLedAnalogico(255, 255, 255, luminosita);
  }
}

// ACCENDE I LED IN MODO ANALOGICO
void accendoLedAnalogico(int luminositaUno, int luminositaDue, int luminositaTre, int luminositaQuattro) {
  analogWrite(blu, luminositaUno);
  analogWrite(rosso, luminositaDue);
  analogWrite(verde, luminositaTre);
  analogWrite(giallo, luminositaQuattro);
}

void sequenzaErrata() {
  digitalWrite(rosso, HIGH);
  delay(2000);
  digitalWrite(rosso, LOW);
}

void allenamento() {
  int allenamentoCompiuto = 0;
  delay(600);
  animazione();
  while (allenamentoCompiuto < 2) {
    acquisizione(sequenza);
    if (verifica(sequenza)) {
      allenamentoCompiuto++;
    } else {
      mostramiSequenza();
    }
  }
  animazione();
}

void mostramiSequenza() { //Mostra la sequenza in modo dinamico
  spegniTuttiLed();
  delay(600);
  for (int indiceSequenza = 0; indiceSequenza < 4; indiceSequenza++) {
    for (int valoreLuminosita = 1023; valoreLuminosita >= combinazione[indiceSequenza]; valoreLuminosita-- ) {
      illuminaSequenza(valoreLuminosita);
      delay(3);
    }
    delay(1500);
    spegniTuttiLed();
    delay(800);
  }
}

void animazione() {
  accendoTuttiLed();
  delay(800);
  digitalWrite(blu, LOW);
  digitalWrite(giallo, LOW);
  delay(800);
  spegniTuttiLed();
}

void animazioneVerifica() {
  digitalWrite(blu, HIGH);
  delay(180);
  digitalWrite(rosso, HIGH);
  delay(180);
  digitalWrite(verde, HIGH);
  delay(180);
  digitalWrite(giallo, HIGH);
  digitalWrite(blu, LOW);
  delay(200);
  digitalWrite(rosso, LOW);
  delay(200);
  digitalWrite(verde, LOW);
  delay(200);
  digitalWrite(giallo, LOW);
}

void spegniTuttiLed() {
  digitalWrite(blu, LOW);
  digitalWrite(rosso, LOW);
  digitalWrite(verde, LOW);
  digitalWrite(giallo, LOW);
}

void accendoTuttiLed() {
  digitalWrite(blu, HIGH);
  digitalWrite(rosso, HIGH);
  digitalWrite(verde, HIGH);
  digitalWrite(giallo, HIGH);
}

void movimentoCoperchio(int salitaODiscesa) {
  const int minimoRaggiunto = 5;
  const int massimoRaggiunto = 19;
  const byte miMuovoDiUno = 1;

  digitalWrite(giallo, HIGH);
  int distanza = ultra.read();
  if (distanza >= 3 && distanza <= 24) {

    regolazioneVelocita(distanza);
    myStepper.step(miMuovoDiUno * salitaODiscesa);

    switch (salitaODiscesa) {
      case salita:
        if (ultra.read() > massimoRaggiunto) {
          digitalWrite(giallo, LOW);
          settoTempo = true;
          stato = scatolaAperta;
        }
        break;

      case discesa:
        if (ultra.read() < minimoRaggiunto) {
          spegniTuttiLed();
          settoTempo = true;
          stato = scatolaChiusa;
          myStepper.step(200);
        }
        break;
    }
  } else {
    delay(400); //ERRORE IL COPERCHIO NON è IN SEDE, RIPROVO
  }
  if (controlloTempo(15000)) { // CONTROLLO IL TEMPO TRASCOSO
    stato = inChiusura;
  }
}

boolean controlloTempo(unsigned long tempoMassimo) {   // Controllo Tempo
  if (settoTempo == true) {
    tempo = millis();
    settoTempo = false;
  }
  unsigned long tempoDiff = millis() - tempo;
  if (tempoDiff > tempoMassimo) {
    settoTempo = true;
    return true;
  }
  return false;
}

void regolazioneVelocita(int distanza) {    // regolazione velocità motore
  int posizione = map(distanza, 1, 20, 1, 6);
  switch (posizione) {
    case 1: case 5: case 6:
      myStepper.setSpeed(1);
      break;

    case 2: case 4:
      myStepper.setSpeed(3);
      break;

    case 3:
      myStepper.setSpeed(5);
      break;
  }
}

void aperta() {
  // CAMBIO STATO, CHIUSURA COPERCHIO
  if (digitalRead(bottoneEsterno)) {
    stato = inChiusura;
  }

  // CAMBIO SEQUENZA
  if (digitalRead(cambioCodice)) {
    stato = cambioSequenza;
  }
}

void loop() {
  switch (stato) {

    case accensioneMacchina:
      acquisizione(combinazione);
      allenamento();
      stato = scatolaChiusa;
      break;

    case scatolaChiusa:
      autenticazione();
      break;

    case inApertura:
      movimentoCoperchio(salita);
      break;

    case scatolaAperta:
      aperta();
      break;

    case cambioSequenza:
      acquisizione(combinazione);
      allenamento();
      stato = scatolaAperta;
      break;

    case inChiusura:
      movimentoCoperchio(discesa);
      break;
  }
}
