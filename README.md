# BoxDuino
BoxDuino è una scatola automatizzata in apertura e chisura tramite uno stepper motor che viene comandato da un sensore ultrasuoni, per poter azionare il meccanismo di apertura è necessario conoscere un codice formato da quattro pressioni, che viene impostato all'accensione della macchina. Successivamente il codice puo essere sempre cambiato quando la scatola è aperta. 
Per dare un servizio di feedback all'utente vengono utilizzati quattro led, che in base al loro stato possono indicare diverse informazioni (esempio: codice errato(led rosso), codice corretto(led verde), ...), un'altro feedback importante dato dai led al momento delle acquisizioni delle pressioni è la loro illumnazione graduale all'aumentare della pressione esercitata sul sensore.  

<br>

<h2 color:blue>MANUALE UTENTE</h2>

<h3>1. AVVIO BOXDUINO</h3>
<p>Al primo avvio del BoxDuino bisognerà impostare un codice, formato da quattro pressioni indipendenti, le quali verranno usate come codice di accesso. 
Per salvare una pressione basta premere il sensore di pressione a piacimento regolandosi con il supporto visivo dato dai led, dopo di che al raggiungimento della pressione voluta contemporaneamente bisogna schiacciare bottone posto fuori dalla scatola per salvarla.</p>
<p>Ripete per le successive pressioni.</p>

Una volta settato il codice inizierà una fase di allenamento, evidenziata da una animazione da parte dei led, nella quale il codice dovrà essere reinserito correttamente per due volte, se viene sbagliato il codice corretto verrà reso visibile sui led in modo che si possa impararlo correttamente.


<h3>2. SCATOLA CHIUSA</h3>
Quando la scatola è chiusa sarà sempre in attesa di ricevere il codice, una volta inserito valuterà se è corretto (led Verde) o errato (led Rosso).


<h3>3. APERTURA SCATOLA</h3>
<p>Una volta inserito il codice corretto la scatola si aprirà in modo automatico, si potrà vedere questo stato dal led giallo che indica che il motore è in funzione.
Finche il led giallo non si spegne non bisogna toccare la scatola.</p>
<p>Ricorda:
Lascia sempre libero il coperchio, non ostruirlo, in caso contrario se dopo diversi tentativi non riesce ad aprirsi, la scatola si richiuderà.</p>

<h3>4. SCATOLA APERTA</h3>
Una volta che il led Giallo sarà spento la scatola è aperta e si potranno essere svolte le proprie attività in maniera sicura.

Se si desidera cambiare il codice di sblocco si puo fare in questo momento, semplicemente premendo il bottone sinistro all'interno della scatola, le azioni da eseguire saranno le stesse che sono state effettuate durante la prima impostazione del primo codice di sblocco.

Per chiudere la scatola basterà premere il bottone posto al di fuori di essa.

<h3>5. CHIUSURA SCATOLA</h3>
Una volta premuto il bottone la scatola si chiuderà, il led giallo si illuminerà e da questo momento non bisognerà toccare la scatola e non ostruirla in alcun modo.
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
