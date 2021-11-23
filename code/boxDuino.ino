//librerie
#include <Ultrasonic.h>
#include <Stepper.h>

#define blu 9
#define rosso 5
#define verde 6
#define giallo 7
#define pressionePin A0
#define bottoneDue 2
#define bottone 3
#define minimoRaggiunto 5
#define massimoRaggiunto 19

Ultrasonic ultra(12, 13);
Stepper myStepper(2048, 11, 4, 10, 8);

void setup() {
  Serial.begin(115200);
  pinMode(bottoneDue, INPUT);
  pinMode(pressionePin, INPUT);
  pinMode(rosso, OUTPUT);
  pinMode(blu, OUTPUT);
  pinMode(giallo, OUTPUT);
  pinMode(verde, OUTPUT);
  attachInterrupt(digitalPinToInterrupt(bottone), cambioSequenza, FALLING);
  myStepper.setSpeed(9);
  inizializzazioneCombinazione();
  
}
//VARIABILI                                           
const byte inChiusura = 4;
const byte scatolaChiusa = 1;
const byte scatolaAperta = 3;
const byte inApertura =  2;
const byte premuto =  1;
const int range = 150;
const byte minimaDistanza =  1;
const int massimaDistanza = 20;
const int salita = -1;
const int discesa = 1;

byte stato;
int combinazione[4];
int sequenza[4];
boolean settoTempo = true;
unsigned long tempo;

//FUNZIONI
void inizializzazioneCombinazione(){
  acquisizione(combinazione);
  allenamento();
  stato = scatolaChiusa;
}

// ACQUISISCE QUATTRO PRESSIONI
void acquisizione(int sequenza[]) {
  int indiceSequenza = 0;
  while (indiceSequenza < 4) {
    // Ogni 50 millisecondi leggo un valore dal sensore della pressione
    delay(50);
    int valoreLetto = analogRead(pressionePin);
    // illumino i led in base al valore premuto
    illuminaSequenza(valoreLetto);

    // se il bottone è premuto salvo il valore
    if (digitalRead(bottoneDue) == premuto) {
      sequenza[indiceSequenza] = valoreLetto;
      indiceSequenza++;
      delay(2000);
    }
  }

  spegniTuttiLed();
}

// VENGONO ILLUMINATI I LED IN BASE AL VALORE LETTO DAL SENSORE DI PRESSIONE
void illuminaSequenza(int valorePressione) {

  int luminosita;
  // La luminosità di ciascun led dipende dalla pressione letta dal sensore
  if (valorePressione > 600) {
    luminosita = map(valorePressione, 1023, 600, 0, 255);
    accendoLed(luminosita, 0, 0, 0);
  } else if (valorePressione > 400) {
    luminosita = map(valorePressione, 600, 400, 0, 255);
    accendoLed(255, luminosita, 0, 0);
  } else if (valorePressione > 200) {
    luminosita = map(valorePressione, 400, 200, 0, 255);
    accendoLed(255, 255, luminosita, 0);
  } else {
    luminosita = map(valorePressione, 200, 0, 0, 255);
    accendoLed(255, 255, 255, luminosita);
  }
}

// ACCENDE I LED IN MODO ANALOGICO
void accendoLed(int luminositaUno, int luminositaDue, int luminositaTre, int luminositaQuattro) {
  analogWrite(blu, luminositaUno);
  analogWrite(rosso, luminositaDue);
  analogWrite(verde, luminositaTre);
  analogWrite(giallo, luminositaQuattro);
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

void sequenzaCorretta(){
    digitalWrite(verde, HIGH);
    delay(2000);
    digitalWrite(verde, LOW);
}

void sequenzaErrata(){
    digitalWrite(rosso, HIGH);
    delay(2000);
    digitalWrite(rosso, LOW);
}

void allenamento() {
  int allenamentoCompiuto = 0;
  animazione();
  while (allenamentoCompiuto < 2) {  
    acquisizione(sequenza);
    if (verifica(sequenza) == true) {
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
    //accendo il led che mostro
    for (int valoreLuminosita = 1023; valoreLuminosita >= combinazione[indiceSequenza]; valoreLuminosita-- ) {
      illuminaSequenza(valoreLuminosita);
      delay(2);
    }
    delay(1500);
    spegniTuttiLed();
  }
}

void animazione(){
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

void accendoTuttiLed(){
  digitalWrite(blu, HIGH);
  digitalWrite(rosso, HIGH);
  digitalWrite(verde, HIGH);
  digitalWrite(giallo, HIGH);
}

//AUTORIZZA L'APERTURA DELLA SCATOLA O MENO A SECONDA DELLA COMBINAZIONE
void autenticazione() {
  //acquisizione della sequenza
  acquisizione(sequenza);
  //verifica della combinazione, se errata lo stato non cambia
  if (verifica(sequenza) == true) {
    digitalWrite(verde, HIGH);
    stato = inApertura;
  }
}

void movimentoCoperchio(int valoreFisso, int salitaODiscesa) {

  digitalWrite(giallo, HIGH);
  int distanza = ultra.read();

  if (distanza >= 4 && distanza <= 24) {

    int valoreMovimento = calcoloMovimentoMotore(distanza, valoreFisso);
    myStepper.step(valoreMovimento * salitaODiscesa);

    switch (salitaODiscesa) {
      case salita:
        if (ultra.read() > massimoRaggiunto) {
          digitalWrite(giallo, LOW);
          stato = scatolaAperta; 
          settoTempo = true;
        }
        break;

      case discesa:
        if (ultra.read() < minimoRaggiunto) {
          spegniTuttiLed();
          stato = scatolaChiusa;  
          settoTempo = true;
        }
        break;
    }

  } else {
    //ERRORE IL COPERCHIO NON è IN SEDE
    Serial.println("IL COPERCHIO NON E' IN SEDE");
    delay(200);
  }

  controlloTempo(15000); 
}

void controlloTempo(unsigned long prefissato) {

  unsigned long tempoDiff = 0;

  if (settoTempo == true) {
    tempo = millis();  
    settoTempo = false;
  }

  tempoDiff = millis() - tempo;

  if (tempoDiff > prefissato) {   
    settoTempo = true;
    stato = inChiusura;
  }
}

int calcoloMovimentoMotore(int posizioneCoperchio, int valoreFisso) {
  int mancano = posizioneCoperchio - valoreFisso;

  // togliere -- mettere funzione abs()
  if (mancano < 0) {
    mancano = mancano * -1; // togliere -- mettere funzione abs()
  }
  // togliere -- mettere funzione abs()
  mancano = map(mancano, 0, 16, 0, 100);
  mancano = map(mancano, 0, 360, 0, 2048);
  return mancano;
}


void cambioSequenza(){
  acquisizione(combinazione);
  allenamento();
}

void attesaChiusura(){

    if (digitalRead(bottoneDue) == premuto) {
     stato = inChiusura;
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
        attesaChiusura();
      break;

    case inChiusura:
        movimentoCoperchio(minimaDistanza, discesa);
      break;
  }
}
