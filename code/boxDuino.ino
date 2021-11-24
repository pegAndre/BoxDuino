//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define rosso 5
#define verde 6
#define giallo 7
#define blu 9
#define sensorePressione A0
#define bottoneEsterno 2
#define cambioCodice 3

#define scatolaChiusa 1
#define inApertura 2
#define scatolaAperta 3
#define inChiusura 4

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
  myStepper.setSpeed(10);
  inizializzazioneCombinazione();
}

//VARIABILI
const int minimaDistanza =  1;
const int massimaDistanza = 20;
const int salita = -1;
const int discesa = 1;
const int range = 150;
int combinazione[4];
int sequenza[4];
byte stato = scatolaChiusa;
boolean settoTempo = true;
unsigned long tempo;

//FUNZIONI
//INIZIALIZZA LA COMBINAZIONE ALL'ACCENSIONE
void inizializzazioneCombinazione(){
  acquisizione(combinazione);
  allenamento();
}

//AUTORIZZA L'APERTURA DELLA SCATOLA O MENO A SECONDA DELLA COMBINAZIONE
void autenticazione() {
  //acquisizione della sequenza
  acquisizione(sequenza);
  //verifica della combinazione, se errata lo stato non cambia
  if (verifica(sequenza)) {
    digitalWrite(verde, HIGH);
    stato = inApertura;
  }
}

// ACQUISISCE QUATTRO PRESSIONI
void acquisizione(int sequenza[]) {
  int indiceSequenza = 0;
  while (indiceSequenza < 4) {
    // Ogni 50 millisecondi leggo un valore dal sensore della pressione
    delay(50);
    int valoreLetto = analogRead(sensorePressione);
    // illumino i led in base al valore premuto
    illuminaSequenza(valoreLetto);

    // se il bottone è premuto salvo il valore
    if (digitalRead(bottoneEsterno)) {
      sequenza[indiceSequenza] = valoreLetto;
      indiceSequenza++;
      delay(2000);
    }
  }
  spegniTuttiLed();
}

//VERIFICA LA COMBINAZIONE INSERITA
boolean verifica(int sequenza[]) {
  if (sequenza[0] > (combinazione[0] - range) && sequenza[0] < (combinazione[0] + range) && sequenza[1] > (combinazione[1] - range) && sequenza[1] < (combinazione[1] + range) && sequenza[2] > (combinazione[2] - range) && sequenza[2] < (combinazione[2] + range) && sequenza[3] > (combinazione[3] - range) && sequenza[3] < (combinazione[3] + range)) {
    //LOGGATTO
    sequenzaCorretta();
    return true;
  } else {
    sequenzaErrata();
    return false;
  }
}

// VENGONO ILLUMINATI I LED IN BASE AL VALORE LETTO DAL SENSORE DI PRESSIONE
void illuminaSequenza(int valorePressione) {
  int luminosita;
  // La luminosità di ciascun led dipende dalla pressione letta dal sensore
  if (valorePressione > 600) {
    luminosita = map(valorePressione, 1023, 600, 0, 255);
    accendoLedAnalogico(luminosita, 0, 0, 0);
  } else if (valorePressione > 400) {
    luminosita = map(valorePressione, 600, 400, 0, 255);
    accendoLedAnalogico(255, luminosita, 0, 0);
  } else if (valorePressione > 200) {
    luminosita = map(valorePressione, 400, 200, 0, 255);
    accendoLedAnalogico(255, 255, luminosita, 0);
  } else {
    luminosita = map(valorePressione, 200, 0, 0, 255);
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

void sequenzaCorretta() {
  digitalWrite(verde, HIGH);
  delay(2000);
  digitalWrite(verde, LOW);
}

void sequenzaErrata() {
  digitalWrite(rosso, HIGH);
  delay(2000);
  digitalWrite(rosso, LOW);
}

void allenamento() {
  int allenamentoCompiuto = 0;
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

//MOSTRA LA SEQUENZA IN MODO DINAMICO
void mostramiSequenza() {
  // mostro la pressione da esercitare tramite la luminosità dei led, scandendo tutti i valori
  for (int indiceSequenza = 0; indiceSequenza < 4; indiceSequenza++) {
 //accendo led giusto
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

void movimentoCoperchio(int valoreFisso, int salitaODiscesa) {
  const int minimoRaggiunto = 5;
  const int massimoRaggiunto = 19;

  digitalWrite(giallo, HIGH);
  int distanza = ultra.read();

  if (distanza >= 3 && distanza <= 24) {
    int valoreMovimento = calcoloMovimentoMotore(distanza, valoreFisso);
    myStepper.step(valoreMovimento * salitaODiscesa);

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
        }
        break;
    }
  } else {
    delay(400); //ERRORE IL COPERCHIO NON è IN SEDE, RIPROVO
  }
  controlloTempo(15000); // CONTROLLO IL TEMPO TRASCOSO
}

void controlloTempo(unsigned long tempoMassimo) {
  if (settoTempo == true) {
    tempo = millis();
    settoTempo = false;
  }

  unsigned long tempoDiff = millis() - tempo;
  if (tempoDiff > tempoMassimo) {
    settoTempo = true;
    stato = inChiusura;
  }
}

int calcoloMovimentoMotore(int posizioneCoperchio, int valoreFisso) {
  int mancano = abs(posizioneCoperchio - valoreFisso);
  mancano = map(mancano, 0, 16, 0, 100);
  mancano = map(mancano, 0, 360, 0, 2048);
  return mancano;
}

void aperta() {
  // CAMBIO STATO, CHIUSURA COPERCHIO
  if (digitalRead(bottoneEsterno)) {
    stato = inChiusura;
  }

  // CAMBIO SEQUENZA
  if (digitalRead(cambioCodice)) {
    inizializzazioneCombinazione();
  }
}

void loop() {
  switch (stato) {
    case scatolaChiusa:
      autenticazione();
      break;

    case inApertura:
      movimentoCoperchio(massimaDistanza, salita);
      break;

    case scatolaAperta:
      aperta();
      break;

    case inChiusura:
      movimentoCoperchio(minimaDistanza, discesa);
      break;
  }
}
