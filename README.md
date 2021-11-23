# BoxDuino
BoxDuino è una scatola la quale è protetta da un meccanismo di protezione, per poter accedere ad essa è necessario comporre una combinazione su un sensore di pressione, se
la combinazione è esatta, uno stepper motor aprirà la scatola alla giusta angolazione regolando l'apertura tramite un sensore a ultrasuoni che se ostacolato cercherà comunque di 
portare a termine l'apertura o abortirà il tentativo. Una volta aperta la scatolà puo essere chiusa con un bottone in modo automatico.

<br>

<h2>MANUALE UTENTE</h2>

<h3>1. SETTAGIO SEQUENZA</h3>
Al primo avvio del BoxDuino bisognerà impostare un codice, formato da quattro pressioni indipendenti, le quali verrà usata come codice di accesso. 
Per salvare una pressione basta premere il sensore a piacimento regolandosi con il supporto visivo dato dai led, dopo di che al raggiungimento della pressione voluta contemporaneamente bisogna schiacciare bottone posto fuori dalla scatola per salvarla.
Ripete per le successive pressioni.

Una volta settato il codice inizierà una fase di allenamento, evidenziata da una animazione da parte dei led, nella quale il codice dovrà essere reinserito correttamente per due volte, se lo sbagli non preoccuparti !! il codice corretto verrà reso visibile sui led in modo che tu possa impararlo correttamente.


<h3>2. SCATOLA CHIUSA</h3>
Quando la scatola è chiusa sarà sempre in attesa di ricevere il codice, una volta inserito valuterà se è corretto (led Verde) o errato (led Rosso).


<h3>3. IN APERTURA</h3>
Una volta inserito il codice corretto la scatola si aprirà in modo automatico! Potrai vedere questo stato dal led giallo che indica che il motore è in funzione.
Finche il led giallo non si spegne non toccare la scatola!

Ricorda:
Lascia sempre libero il coperchio, non ostruirlo, in caso contrario se dopo diversi tentativi non riesce ad aprirsi, la scatola si richiuderà!

<h3>4. SCATOLA APERTA</h3>
Una volta che il led Giallo sarà spento la scatola è aperta e potrai svolgere le tue attività in maniera sicura !

Se desideri cambiare il codice di sblocco puoi farlo in questo momento, semplicemente premendo il bottone sinistro all'interno della scatola e ri-setterai il codice proprio come hai fatto alla prima accensione.

Per chiudere la scatola premi il bottone posto al di fuori della scatola.

<h3>5. IN CHIUSURA</h3>
Una volta premuto il bottone la scatola si chiuderà, il led giallo si illuminerà e da questo momento non toccare la scatola e non ostruirla in alcun modo !
Una volta chiusa correttamente la scatola si rimetterà in attesa del codice di sblocco.

<br>

 <h2>Hardware (indicativo):</h2>
 	<li>ELEGOO UNO R3</li>
	<li>Sensore di Pressione (Akozon DF9-40)</li>
	<li>Stepper Motor (28BYJ-48)</li>
	<li>Ultrasuoni (HC-SR04)</li>
	<li>Bottone </li>
	<li>Vari Led</li>
	<li>Varie Resistenze (2k ohm e 10k ohm)</li>
	
<br>	


<h2>Schema elettrico (in aggiornamento...)</h2>

![schema](https://user-images.githubusercontent.com/78487344/140295181-7ebf395d-3990-46d8-942a-38f863ec2417.png)
