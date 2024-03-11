[![de](https://img.shields.io/badge/lang-de-green)](https://github.com/ToyKeeper/anduril/blob/trunk/docs/anduril-manual.de.md)
[![en](https://img.shields.io/badge/lang-en-red)](https://github.com/ToyKeeper/anduril/blob/trunk/docs/anduril-manual.md)

Anduril Benutzerhandbuch
------------------------

Anduril ist eine Open-Source Firmware für Taschenlampen, bereitgestellt unter
den Bedingungen der GPL v3.  Der Quelltext kann hier bezogen werden:

  - https://toykeeper.net/anduril

Die obige URL leitet auf die tatsächliche Projektwebseite um. Auch wenn es künftig erforderlich sein wird,
das Projekt auf ein anderes Hosting umzuziehen, sollte die Verknüpfung weiterhin funktionieren.
Stand Ende 2023 lautet das Umleitungsziel:

  - https://github.com/ToyKeeper/anduril


Schnelleinstieg
---------------

Nach dem Einsetzen des Akkus in die Taschenlampe und Festziehen aller Teile,
sollte die Taschenlampe einmal kurz blinken, um die Bereitschaft zu
signalisieren.  Anschließend ist die grundlegende Bedienung ganz einfach:

  - **Klicken** um die Lampe ein- und auszuschalten.
  - Die Taste **halten** um die Helligkeit zu ändern.
  - **Loslassen und erneut halten** um die Helligkeit in die andere Richtung zu ändern.

Für die grundlegende Bedienung ist dies alles, was der Nutzer wissen muss.
Jedoch gibt es viele weitere Modi und Funktionen für Nutzer, die mehr wollen.

Eine vollständige Übersicht der Befehlssequenzen ist am Ende dieser Anleitung
in der [UI Referenztabelle](#ui-referenztabelle) zu finden.


Tastenkombinationen
-------------------

Tastenkombinationen werden über folgende Notation abgekürzt:

  - `1C`: **Ein Klick.**  Die Taste drücken und gleich wieder loslassen.
  - `1H`: **Halten.**  Die Taste betätigen und gedrückt halten.
  - `2C`: **Zwei Klicks.**  Zwei mal schnell hintereinander drücken und loslassen.
  - `2H`: **Klicken, halten.**  Zwei mal drücken und beim zweiten Mal gedrückt halten.
  - `3C`: **Drei Klicks.**  Drei mal schnell hintereinander drücken und loslassen.
  - `3H`: **Klicken, klicken, halten.**: Drei mal drücken und beim dritten Mal gedrückt halten.

Das gleiche Schema ist für die übrigen Kombinationen anzuwenden.  
Beispielsweise bedeutet `10C` "zehn mal drücken"...  und `10H` bedeutet "zehn mal drücken und beim
letzten Mal gedrückt halten".

Die *Zahl* bezeichnet wie oft die Taste betätigt sein soll. Der *Buchstabe* deutet darauf hin, ob
der letzte Tastendruck losgelassen (C) oder gehalten (H) werden soll,


Werkseinstellungen
------------------

Bei zu viel Verwirrung oder falls der Temperatursensor automatisch kalibriert werden soll, kann
die Taschenlampe auf Werkseinstellungen zurückgesetzt werden.  Um den Reset durchzuführen:

  - **Lösen** der Endkappe
  - **Halten** der Taste
  - **Festziehen** der Endkappe
  - **Taste gedrückt halten** (für etwa 4s)

Alternativ:

  - `13H` beim "Aus"-Modus, für etwa 4s gedrückt halten

Das Licht sollte nun flackern und immer heller werden, schließlich hell
aufblitzen.  Um einen Reset durchzuführen, muss die Taste gehalten werden, bis
das Licht ausgeht.  Wird die Taste vorher losgelassen, wird der Reset
abgebrochen.

Bei manchen Taschenlampen, bei denen die Endkappen-Methode nicht funktioniert,
kann `13H` bei ausgeschalteter Lampe genutzt werden, um einen Reset
durchzuführen.  Falls dies Schwierigkeiten bereitet, hilft es vielleicht, nach
einem Lied zu zählen:

```
  1 2 3 4
  2 2 3 4
  3 2 3 4
  HALTEN
```

Simple UI wird nach jedem Reset aktiviert.


Simple UI
---------

Standardmäßig wird die Taschenlampe im Simple UI ausgeliefert.  Dieser Modus
ist praktisch, wenn die Taschenlampe verliehen werden soll oder man sich nicht
mit allen Details der übrigen Modi auseinandersetzen möchte.

Simple UI kommt mit allen grundlegenden Funktionen, die für eine Taschenlampe
nötig sind, jedoch wird die minimale und die maximale Helligkeit standardmäßig
beschränkt, um die Taschenlampe sicherer zu machen. Zudem sind komplizierte oder
erweiterte Funktionen gesperrt.

### Bedienung

Im Simple UI enthaltene Funktionen:

  - `1C`: An / aus
  - `1H`: Helligkeit erhöhen (oder verringern, wenn die Taste weniger als eine
          Sekunde losgelassen wurde)
  - `2H`: Wenn die Lampe an ist: Helligkeit verringern
          Wenn die Lampe aus ist: Momentan-Modus bei hoher Helligkeit
  - `2C`: Doppelklick um zu oder von der höchsten sicheren Helligkeit zu wechseln
  - `4C`: Tastensperre.

Zusätzlich sind einige weitere Modi und Funktionen verfügbar.  Wenn die
Taschenlampe aus ist, gibt es folgende Optionen:

  - `3C`: Prüfen der Akkuspannung.  (Zeigt die Spannung einmalig an und schaltet
          danach wieder ab)
  - `4C`: Tastensperre.
  - `10H`: Zum Advanced UI wechseln.
  - `15C` oder mehr: Version ausgeben.

Wenn die Taschenlampe im Simple UI gesperrt ist, stehen folgende Funktionen
zur Verfügung:

  - `1H`: Momentan Moon
  - `2H`: Momentan niedrige Helligkeit
  - `3C`: Entsperren und ausschalten
  - `4C`: Entsperren und einschalten
  - `4H`: Entsperren und in niedrigster Helligkeit einschalten
  - `5C`: Entsperren und in höchster Helligkeit einschalten

Um zwischen Simple UI und Advanced UI zu wechseln, muss die Taschenlampe
ausgeschaltet sein und anschließend folgendes gemacht werden:

  Im Simple UI:
  
  - `10H`: Zum Advanced UI wechseln.

  Im Advanced UI:
  
  - `10C`: Zum Simple UI wechseln.


### Erweitertes Simple UI

Für einige Taschenlampen werden zusätzliche Funktionen im Simple UI freigeschaltet.
Dies erfolgt auf Wunsch des Herstellers. Üblicherweise sind folgende Optionen enthalten:

  - `Ramping -> 3C`: Zwischen Smooth Ramping und Stepped Ramping wechseln.
  - `Ramping -> 5H`: Sunset Timer.
  - `Aus -> 3H`: Zugang zu den Strobe-/Stimmungs-Modi.
  - `Aus -> 7C/7H`: Modus / Farbe der Aux-LEDs ändern.
  - `Tastensperre -> 7C/7H`: Modus / Farbe der Aux-LEDs ändern.

Bietet eine Taschenlampe ein erweitertes Simple UI an, *sollte es
gut überlegt sein, die Lampe Kindern zu überlassen*. Beabsichtigt war es
nicht, die Strobe-/Stimmungs-Modi im Simple UI verfügbar zu machen. Daher kann
die volle Leistung ohne Temperaturregulierung erreicht werden.  

Es ist wahrscheinlich, dass künftig die Strobe-/Stimmungs-Modi aus Sicherheitsgründen
aus dem Simple UI entfernt werden oder schon entfernt worden sind. Ältere
Firmwareversionen werden dadurch nicht verändert. Demzufolge sollte vorsichtig
vorgegangen werden.


### Simple UI konfigurieren

Simple UI kann auf verschiedene Weisen konfiguriert werden, aber nicht solange
Simple UI aktiv ist.  Daher muss zuerst ins Advanced UI gewechselt,
Einstellungen vorgenommen und wieder zurück zum Simple UI gewechselt werden.

Beim "Aus"-Modus des Advanced UIs:

  - `10H`: Simple UI konfigurieren

Die Konfiguration beinhaltet:

  - niedrigste Stufe
  - höchste Stufe
  - Anzahl der Stufen (beim Stepped Ramping)
  - Art des Turbos

Andere Einstellungen werden vom Advanced UI übernommen:

  - Art des Rampings (smooth / stepped)
  - Geschwindigkeit des Smooth Rampings
  - Nach Moon mit dem Ramping starten
  - Speicher Einstellungen
  - Auto-Lock Einstellungen
  - Aux-LED Einstellungen
  - Spannungskalibrierung
  - Einstellungen der Temperaturregulierung
  - Hardwarespezifische "Sonstiges Menü" Einstellungen


Advanced UI
-----------

Die meisten der folgenden Informationen beziehen sich auf das Advanced UI.  
Alles, was zuvor nicht erwähnt wurde, ist im Simple UI nicht verfügbar.


Ramping / Stepped Ramping Modi
------------------------------

Je nach Geschmack kann bei Anduril zwischen Smooth (stufenlos) und Stepped
(gestuft) Ramping umgeschaltet werden.

Jede Rampe hat ihre eigene Konfiguration -- "Floor" (niedrigste Stufe),
"Ceiling" (höchste Stufe) und Anzahl der Stufen beim Stepped Ramping.

Zusätzlich hat das Simple UI eigene Einstellungen für Floor und Ceiling und
die Anzahl der Stufen.  Die Art des Rampings wird vom Advanced UI übernommen.

Es gibt vier Möglichkeiten, das Ramping zu erreichen, wenn die Taschenlampe
ausgeschaltet ist:

  - `1C`: In der gespeicherten Helligkeit einschalten.
          (Weitere Details, was mit "gespeichert" gemeint ist, weiter unten)

  - `1H`: In Floor einschalten.  Die Taste loslassen, sobald das Licht angeht,
          um auf der niedrigsten Stufe zu bleiben, oder weiter gedrückt halten,
          um die Helligkeit zu erhöhen.

  - `2C`: In Ceiling einschalten.

  - `2H`: Bei voller Leistung einschalten, ausschalten wenn die Taste
          losgelassen wird (momentaner Turbo)  
          (Im Simple UI führt dies zum Ceiling anstatt zum Turbo)

Wenn die Taschenlampe eingeschaltet ist, stehen folgende Funktionen zur Verfügung:

  - `1C`: Ausschalten.
  - `2C`: Zum Turbo oder von dort zur vorherigen Helligkeit zurück wechseln  
          (oder wieder zurück zum Turbo, wenn die Lampe die Helligkeit
          automatisch reduziert hat)  
          (Das Verhalten des Turbos ist konfigurierbar)
  - `1H`: Helligkeit erhöhen.  
          Wenn die Taste weniger als eine Sekunde
          losgelassen wurde oder sich die Lampe bereits in der höchsten Stufe
          befindet, wird die Helligkeit reduziert.
  - `2H`: Helligkeit reduzieren.

  - `3C`: Zum anderen Ramping-Stil wechseln. (Smooth / Stepped)  
          (oder zum nächsten Kanal wechseln, sollte mehr als ein Kanal aktiv sein)  
          (dann sollte stattdessen `6C` benutzt werden, um zum anderen Ramping-Stil zu wechseln)
  - `6C`: Zum anderen Ramping-Stil wechseln.  (sollte `3C` dem nächsten Kanal zugeordnet werden)

  - `3H`: Momentaner Turbo (bei Taschenlampen ohne Tintramping).
  - `3H`: Farbtemperatur ändern (bei Taschenlampen mit Tintramping).
  - `4H`: Momentaner Turbo, sollte `3H` zugeordnet werden.

  - `4C`: Taste sperren.

  - `5C`: Momentan-Modus aktivieren.
  - `5H`: Den Sunset-Timer aktiveren.
          Weitere Informationen im Abschnitt "Sunset Timer".

  - `7H`: Konfigurationsmenü für das Ramping
    - Option 1: Floor.
    - Option 2: Ceiling.
    - Option 3:  
      Stepped Ramping: Anzahl der Stufen.  Kann zwischen 1 und 150 liegen.  
      Smooth Ramping: Geschwindigkeit.  
        1 = Volle Geschwindigkeit, ~2.5s von einem zum anderen Ende.  
        2 = Halbe Geschwindigkeit, ~5s von einem zum anderen Ende.  
        3 = Drittel Geschwindigkeit, ~7.5s.  
        4 = Viertel Geschwindigkeit, ~10s.  

  - `10C`: Manuelles Speichern aktivieren und die aktuelle Helligkeit speichern.
  - `10H`: Menü für zusätzliche Ramping-Einstellungen:
    - Option 1: Manuelles Speichern deaktivieren und zum automatischen
                Speichern zurückkehren.  
                (es spielt keine Rolle, welcher Wert eingegeben wird)
    - Option 2: Den Timer für manuelles Speichern konfigurieren.  
                Setzt den Timer auf N Minuten, wobei N die Anzahl der Klicks
                ist.  Ein Wert von 0 (keine Klicks) deaktiviert den Timer.
    - Option 3: Konfigurieren, ob nach `Aus -> 1H` automatisch die Helligkeit erhöht werden soll.  
                0: Nach Moon Helligkeit erhöhen.  
                1: Helligkeit nicht erhöhen, bei Moon bleiben.

    - Option 4: Den Turbo-Stil des Advanced UI konfigurieren:  
                0: Kein Turbo, nur höchste Helligkeit (Ceiling).  
                1: Anduril 1 Stil.  `Ramping -> 2C` zu voller Leistung.  
                2: Anduril 2 Stil.  `Ramping -> 2C` zu konfigurierter höchster Helligkeit (Ceiling)
         oder zu voller Leistung, falls bereits zur höchsten Helligkeit gerampt wurde.
         Diese Einstellungen beeinflusst auch das Verhalten des momentanen Turbos.

    - Option 5: "Smooth steps" (gleitende Stufen) konfigurieren.  
                0: Gleitende Stufen deaktivieren.  
                1: Gleitende Stufen aktivieren.

Speichern definiert die Helligkeit, die verwendet werden soll, wenn die
Taschenlampe mit `1C` von Aus eingeschaltet wird.  Es kann zwischen drei Arten
von Speichern gewählt werden:

  - Automatisch: Immer die zuletzt genutzte Helligkeit der Rampe benutzen.
    (Speichert nicht die Helligkeit nach einem Shortcut, wie beispielsweise
    Turbo, `2C` für Ceiling oder `1H von Aus` für niedrigste Helligkeit.)

  - Manuell: Immer die vom Benutzer gespeicherte Helligkeit verwenden.

  - Hybrid: Verwendet die Helligkeit des automatischen Speicherns, falls die
    Lampe nur kurz ausgeschaltet war, oder setzt die Helligkeit auf den
    gespeicherten Wert zurück, wenn sie länger aus war.
    Der Timer für diesen Modus kann zwischen 0 und ~140 Minuten gewählt
    werden.

Auf andere Weise beschrieben: Es gibt drei Arten, die zuletzt genutzte
Helligkeit wiederherzustellen...

  - immer wiederherstellen          (automatisch)
  - für N Minuten wiederherstellen  (hybrid)
  - niemals wiederherstellen        (manuell)

Um die Art des Speicherns zu wählen, folgende Konfiguration vornehmen:

| Speichertyp  | Manuelles Speichern  | Timer für manuelles Speichern
| -----------  | -------------------  | -----------------------------
| automatisch  | aus                  | beliebig
| manuell      | ein                  | Null
| hybrid       | ein                  | nicht Null

Wenn "Smooth Steps" (gleitende Stufen) aktiviert ist, gibt es beim Stepped Ramping,
sowie beim Ein- und Ausschalten, einen gleitenden Übergang zwischen den Stufen.
Ohne "Smooth Steps" sind diese Übergänge zackig.


Sunset Timer
------------

Im Ramping-Modus oder im Kerzen-Modus ist es möglich, die Taschenlampe nach
einer eingestellten Zeit automatisch auszuschalten.

Um diesen Timer zu aktivieren, ist zuerst die gewünschte Helligkeit zu wählen
und dann eine `5H` Aktion auszuführen.  Die Taste gedrückt halten und die
Taschenlampe sollte im Sekundentakt blinken.  Jedes Blinken fügt dem Timer
fünf Minuten hinzu.

Im Ramping-Modus wird die Helligkeit allmählich reduziert, bis sie die
niedrigste Stufe erreicht und dann abschaltet.  Im Kerzen-Modus bleibt die
Helligkeit erhalten, bis sie kurz vor Ende plötzlich abnimmt und dann ausgeht.

Während der Timer aktiv ist, kann der Nutzer die Helligkeit ändern.  Falls
dies in den letzten Minuten vor Ablauf des Timers geschieht, wird der Timer
auf mindestens drei Minuten erhöht.  Falls das Licht also bereits ziemlich
dunkel geworden ist und etwas mehr Licht benötigt wird, `5H` ausführen um fünf
Minuten zum Timer hinzuzufügen oder einfach die Helligkeit erhöhen.


Andere Modi
-----------

Anduril hat diverse weitere Modi.  Um sie zu erreichen, muss der Taster mehr
als zwei Mal gedrückt werden, während die Taschenlampe aus ist:

  - `3C`: Blink / Werkzeug Modi, beginnend mit Spannungsprüfung.
  - `3H`: Strobe Modi, beginnend mit dem zuletzt genutzten Strobe.
  - `4C`: Tastensperre.
  - `5C`: Momentan Modus.
  - `6C`: Taktischer Modus.
  - `7C` / `7H`: Aux-LED Konfiguration.
  - `9H`: Menü für sonstige Einstellungen. (nur bei manchen Lampen)
  - `10H`: Simple UI Konfigurationsmenü.
  - `13H`: Werkseinstellungen (bei manchen Lampen).
  - `15C` oder mehr: Version ausgeben.


Tastensperre
------------

Bei ausgeschalteter Lampe vier Mal drücken um die Taste zu sperren.  Oder vier
Mal beim Ramping.  Dies erlaubt es, die Lampe sicher in einer Tasche zu
transportieren, wo der Taster versehentlich betätigt werden könnte.

Um die Tastensperre zu deaktivieren, erneut vier Mal drücken.  Das Licht
sollte kurz aufblinken und sich dann in der gespeicherten Stufe einschalten.
Oder den letzten Druck halten um die Lampe stattdessen im Floor einzuschalten:

  - `3C`: Entsperren und ausschalten

  - `4C`: Entsperren und in gespeicherte Helligkeit einschalten  
          (verwendet manuell gespeicherte Helligkeit, falls gesetzt)

  - `4H`: Entsperren und im Floor einschalten

  - `5C`: Entsperren und im Ceiling einschalten

Die Tastensperre kann auch für einen momentanen Moon-Modus genutzt werden,
sodass der Nutzer kurze Tätigkeiten erledigen kann, ohne die Taschenlampe
entsperren zu müssen.  Die Helligkeit während der Tastensperre hat zwei
Stufen:

  - `1H`: In der niedrigsten Floor-Stufe einschalten.

  - `2H`: In der höchsten Floor-Stufe einschalten.
          (oder manuell gespeicherte Helligkeit, falls gesetzt)

  - `3H`: Zum nächsten Kanal wechseln (sollte mehr als ein Kanal aktiv sein).

Außerdem ist es möglich, die Taschenlampe nach einer eingestellten Zeit
automatisch sperren zu lassen (Auto-Lock).  Um diese Funktion zu verwenden,
zuerst die Tastensperre aktivieren und dann `10H` nutzen, um in das
Konfigurationsmenü der automatischen Tastensperre zu gelangen.  Die Taste nach
dem ersten Blinken loslassen.  Dann bei der Eingabeaufforderung N mal drücken,
um den Timeout auf N Minuten zu stellen.

  - `10H`: Auto-Lock Konfigurationsmenü.
           N mal drücken um den Timeout auf N Minuten zu setzen.
           Ein Wert von Null deaktiviert die Auto-Lock Funktion.
           Um Auto-Lock zu deaktivieren, also überhaupt nicht drücken.

Bei Taschenlampen mit Aux-LEDs gibt es weitere Funktionen:

  - `7C` / `7H`: Die Aux-LEDs für die Tastensperre-Modus ändern.  Weitere Informationen
                 zur Konfiguration weiter unten im separaten Abschnitt dazu.


Blink / Werkzeug Modi
---------------------

Die Taste drei Mal von Aus drücken, um Andurils Blink / Werkzeug Modi zu
erreichen.  Dies startet immer mit der Spannungsprüfung, von wo aus der Nutzer
im Advanced UI zu weiteren Modi wechseln kann.  Die Abfolge ist:

  - Spannungsprüfung.
  - Temperaturprüfung (falls die Lampe einen Temperatursensor hat).
  - Signal Modus.
  - SOS Modus (falls aktiviert).

In all diesen Modi sind folgende Aktionen möglich:

  - Klicken: Ausschalten.
  - 2 Klicks: Zum nächsten Blink-Modus wechseln.

Zusätzlich gibt es bei der Spannungs- und Temperaturanzeige folgende Aktionen:

  - `7H`: Das Menü zur Spannungskalibrierung oder Temperaturkonfiguration
    aufrufen.

Detaillierte Informationen zu den einzelnen Blink / Werkzeug Modi:

### Spannungsprüfung:

Blinkt die Akkuspannung aus.  Voll geladen entspricht 4,2 V, leer ist bei
rund 3,0 V.  Zuerst wird die ganze Stelle ausgeblinkt und nach einer
kurzen Pause die Zehntel Volt.  Nach einer längeren Pause beginnt die
Ausgabe von vorne.
Für 4,2 V würde es "blink-blink-blink-blink .. blink-blink" sein.

Eine "Null" Stelle wird durch ein sehr kurzes Blinken dargestellt.

Bei Taschenlampen mit mehr als einem Satz LEDs, kann während der Spannungsprüfung
durch `3C` der Satz LEDs ausgewählt werden (d.h. welchen Kanal), der für das
Ausblinken der Spannung verwendet wird.

Das Menü zur Spannungskalibrierung hat einen Eintrag:

  1. Spannungskorrekturfaktor.  Dies kalibriert den Spannungssensor,
     indem eine Abweichung von bis zu 0,3 V in 0,05 V Schritten
     eingestellt werden kann.  N mal klicken um einen Wert einzustellen:

     `1C`:  -0,30 V  
     `2C`:  -0,25 V  
     `3C`:  -0,20 V  
     `4C`:  -0,15 V  
     `5C`:  -0,10 V  
     `6C`:  -0,05 V  
     `7C`:   0,00 V (Standard)  
     `8C`:  +0,05 V  
     `9C`:  +0,10 V  
     `10C`: +0,15 V  
     `11C`: +0,20 V  
     `12C`: +0,25 V  
     `13C`: +0,30 V

  2. Timeout für die Spannungsanzeige nach dem Ausschalten.
     (Lediglich für Taschenlampen mit RGB Aux-LEDs)  
     Diese Einstellung bestimmt, wie viele Sekunden die farbige Spannungsanzeige
     durch die RGB Aux-LEDs angezeigt wird, bevor die Lampe in den Ruhemodus versetzt wird.
     Pro Klick wird eine Sekunde eingestellt. Ein Wert von 0 (keine Klicks) deaktiviert
     diese Funktion.

### Temperaturprüfung:

Blinkt die aktuelle Temperatur in Grad Celsius aus.  Dieser Wert sollte
möglichst genau der tatsächlichen Temperatur entsprechen.  Falls nicht,
sollte der Temperatursensor über das Menü zur Temperaturkonfiguration
kalibriert werden.  Oder die Taschenlampe wird auf Raumtemperatur
gebracht und anschließend auf Werkseinstellungen zurückgesetzt, um den
Sensor automatisch zu kalibrieren.

Das Menü zur Temperaturkonfiguration beinhaltet zwei Einstellungen:

  - Aktuelle Temperatur.  Ein Mal pro Grad Celsius drücken, um den Sensor
    zu kalibrieren.  Beispiel: Falls die Umgebungstemperatur 21 °C ist,
    muss 21 Mal gedrückt werden.

  - Temperaturlimit.  Dies konfiguriert die maximale Temperatur, welche die
    Taschenlampe erreichen darf, bevor die Helligkeit reduziert wird, um
    eine Überhitzung zu vermeiden. Ein Mal pro Grad Celsius über 30
    drücken.  Beispiel: Um ein Limit von 50 °C zu setzen, muss 20 Mal
    gedrückt werden.  Der Standardwert ist 45 °C und der maximale Wert
    erlaubt 70 °C.

### Signal Modus:

Langsames Blinken.  Das Licht bleibt für 100 ms an und bleibt danach bis
zum nächsten Blinken aus.  Die Helligkeit und die Anzahl von Sekunden
zwischen den Blitzen kann konfiguriert werden:

  - Die Helligkeit ist die gespeicherte Helligkeit des Rampings.  Also
    zuerst im Ramping die gewünschte Helligkeit einstellen.  Arbeitet mit
    den gleichen Speicherregeln wie beim Ramping -- automatisch, manuell,
    hybrid.

  - Die Geschwindigkeit wird durch halten des Tasters konfiguriert.  Das
    Licht blinkt bei gehaltenem Taster ein Mal pro Sekunde.  Den Taster
    nach der gewünschten Zeit loslassen, um die Geschwindigkeit zu
    setzen.  
    Beispiel: Um ein zehnsekündiges Alpines Notsignal zu konfigurieren,
    den Taster für 10 Sekunden gedrückt halten.

### SOS Modus:

Blinkt ein Notsignal.  Drei kurz, drei lang, drei kurz.  Wird so lange
wiederholt, bis die Lampe abgeschaltet wird oder der Akku leer ist.

Die gespeicherte Helligkeit bestimmt die Helligkeit im SOS Modus.


Strobe / Stimmungs Modi
-----------------------

Anduril beinhaltet einige weitere Modi für verschiedene Zwecke:

  - Kerzenmodus
  - Fahrrad Blinklicht
  - Party Strobe
  - Tactical Strobe
  - Gewittermodus

Von Aus drei Mal klicken und den letzten Druck für eine Weile halten.  Klick,
klick, halten.  Der zuletzt genutzte Strobe-Modus wird gespeichert, sodass er
beim nächsten Mal direkt startet.

In allen Strobe-Modi sind folgende Aktionen verfügbar:

  - `1C`: Ausschalten.
  - `2C`: Nächster Strobe / Stimmungs Modus.
  - `1H`: Helligkeit oder Geschwindigkeit erhöhen.  (außer Gewittermodus)
  - `2H`: Helligkeit oder Geschwindigkeit reduzieren.  (außer Gewittermodus)
  - `4C`: Vorheriger Strobe / Stimmungs Modus.
  - `5C`: Zum Momentan-Modus wechseln, für Momentan-Strobe
          (dies ist nützlich für Lightpainting)

Im Kerzenmodus gibt es eine weitere Aktion:

  - `5H`: Sunset-Timer aktivieren und/oder fünf Minuten hinzufügen.

Detaillierte Informationen zu jedem Modus:

  - Kerzenmodus

    Die Helligkeit ändert sich unregelmäßig, um das Licht einer Kerzenflamme
    zu simulieren.  Falls der Sunset-Timer aktiviert ist, läuft der
    Kerzenmodus bis zum Ende der Zeit, wird dann für eine Minute dunkler und
    flackert am Ende kurz auf, bevor das Licht erlischt.  Ohne Timer bleibt
    der Kerzenmodus aktiv, bis er abgeschaltet wird.  Die Helligkeit ist
    einstellbar.

  - Fahrrad Blinklicht

    Leuchtet auf mittlerer Stufe, aber flackert einmal pro Sekunde hell.  
    Entworfen, um besser sichtbar zu sein als kontinuierliches Licht, arbeitet
    ansonsten aber genauso.  Die Helligkeit ist einstellbar.

  - Party Strobe

    Strobe um Bewegungen einzufrieren.  Kann genutzt werden, um laufende
    Lüfter oder fallende Wassertropfen einzufrieren.  Geschwindigkeit ist
    einstellbar.

  - Taktischer Strobe

    Taktischer Strobe zur Desorientierung.  Kann genutzt werden, um Personen
    zu verwirren.  Geschwindigkeit ist einstellbar, die Taktzeit ist immer
    33%.

    Falls dieser Strobe längere Zeit genutzt wird, muss die Temperatur
    überwacht werden.

  - Polizei Strobe (bei manchen Lampen)

    2-farbige Strobe im Stil der Polizei.  Funktioniert lediglich
    bei Taschenlampen mit 2 oder mehr Farben.

  - Gewittermodus

    Blitzt in zufälliger Helligkeit und zufälliger Geschwindigkeit um ein
    Gewitter zu simulieren.  Nicht direkt in die Lampe blicken, da sie
    jederzeit bei voller Helligkeit aufblitzen kann.


Momentan-Modus
--------------

Fünf Mal von Aus klicken, um den Momentan-Modus zu aktivieren.  Oder fünf Mal
aus dem Ramping oder fünf Mal aus einem Strobe-Modus.

Dies versetzt die Taschenlampe in einen Ein-Stufen-Modus, in welchem die LED
nur so lange leuchtet, wie der Taster gedrückt gehalten wird.  Dieser Modus
ist zum Geben von Morse-Code, zum Lightpainting oder für andere Aufgaben
gedacht, in denen das Licht nur kurzzeitig an sein soll, möglicherweise in
einem Muster.

Der Momentan-Modus leuchtet entweder in einer gleichbleibenden Helligkeit oder
in einem Strobe, abhängig davon, welcher Modus zuletzt genutzt wurde.  Um den
Modus zu wählen, zuerst den gewünschten Modus aktivieren und dann 5 Mal
drücken, um den Momentan-Modus zu aktivieren.

Im gleichbleibenden Modus entspricht die Helligkeit der gespeicherten
Helligkeit.  Die Helligkeit somit im Ramping konfigurieren, bevor der
Momentan-Modus aktiviert wird.

Im momentanen Strobe-Modus werden die Einstellungen vom zuletzt genutzten
Strobe-Modus übernommen, also vom Party Strobe, Tactical Strobe oder
Gewittermodus.

**Um diesen Modus zu verlassen, muss die Stromversorgung unterbrochen werden**,
indem die Endkappe oder das Akkurohr gelöst wird.


Taktischer Modus
----------------

Von Aus sechs Mal klicken, um den Taktischen Modus zu aktivieren.
Sollte die Taschenlampe sich in diesem Modus befinden, sechs Mal klicken um
den Modus zu verlassen und die Taschenlampe auszuschalten.

Der Taktischer Modus stellt einen sofortigen momentanen Zugang zum Floor, Ceiling
und Strobe-Modus her, wobei jeder davon konfiguriert werden kann. Es stehen folgende
Modi zur Verfügung:

  - `1H`: Ceiling
  - `2H`: Floor
  - `3H`: Strobe

Jeder davon bleibt nur so lange an, wie die Taste gedrückt wird.

Weitere Funktionen im Taktischen Modus sind:

  - `6C`: Beenden (Taschenlampe ausschalten)
  - `7H`: Einstellen des Taktischen Modus
    - 1. Blinken: Taktische Position 1 einstellen
    - 2. Blinken: Taktische Position 2 einstellen
    - 3. Blinken: Taktische Position 3 einstellen

Um eine taktische Position einzustellen, `7H` drücken und die Taste nach dem
1., 2. oder 3. Blinken loslassen. Danach eine Zahl eingeben. Ein Klick
fügt 1 hinzu, ein Halten fügt 10 hinzu. Die folgenden Möglichkeiten gibt es für
die eingegebene Zahl:

  - 1 bis 150: Helligkeitsstufe setzen
  - 0: Zuletzt genutzte Strobe-Modus
  - 151+: Direkt zu einem bestimmten Strobe-Modus wechseln  
    151 = Party Strobe  
    152 = Tactical Strobe  
    153+ = Andere Strobe-Modi, in der gleichen Reihenfolge wie bei `Aus -> 3H'

Obiges setzt voraus, dass die Taschenlampe 150 Ramping-Schritte hat. Strobe-Modi
beginnen 1 höher als die oberste Ramping-Stufe. Daher können die Einstellungen
abweichen, sollte die Taschenlampe eine andere Anzahl an Ramping-Schritten haben.


Konfigurationsmenüs
-------------------

Jedes Konfigurationsmenüs hat die gleiche Bedienung.  Es gibt eine oder
mehrere Einträge, welche der Nutzer konfigurieren kann.  Die Einträge werden
nacheinander durchlaufen.  Jeder Eintrag läuft nach dem gleichen Schema ab:

  - Kurzes Blinken, dann konstant bei geringer Helligkeit.  Den Taster
    gedrückt lassen, um den Menüeintrag zu überspringen, oder den Taster
    loslassen, um den Eintrag auszuwählen und einen neuen Wert einzugeben.

  - Wenn der Taster losgelassen wurde:

    - Schnelles Flackern für einige Sekunden.  Dies bedeutet, dass nun ein
      neuer Wert eingegeben werden kann, indem ein oder mehrmals die Taste
      gedrückt wird um eine Zahl einzugeben.  Das Licht flackert solange, bis
      keine Eingabe mehr erfolgt.  Es besteht also kein Grund zur Eile.

      Verfügbare Aktionen sind:  
      - Klick: 1 hinzufügen  
      - Halten: 10 hinzufügen  (Nur in Versionen seit 09/2021)  
      - Warten: Menü erlassen

Nach der Eingabe einer Zahl oder nachdem alle Einträge übersprungen wurden,
wartet die Lampe bis der Taster losgelassen wurde und verlässt das Menü.  
Danach kehrt die Lampe in den zuletzt genutzten Modus zurück.


Rampen-Konfigurationsmenü
-------------------------

Wenn die Taschenlampe im Ramping-Modus eingeschaltet ist, 7 Mal klicken (und
den letzten Klick halten), um das Konfigurationsmenü für die aktuelle Rampe zu
aktivieren.

Oder um das Rampen-Konfigurationsmenü für das Simple UI zu erreichen, zuerst
sicherstellen, dass das Simple UI nicht aktiv ist und dann `10H` von Aus
drücken.

Für Smooth Ramping gibt es drei Menüeinträge:

  1. Floor.  
     (Standard = 1/150)
  2. Ceiling.  
     (Standard = 120/150)
  3. Geschwindigkeit.  
     (Standard = 1, höchste Geschwindigkeit)

Für Stepped Ramping gibt es drei Menüeinträge:

  1. Floor.  
     (Standard = 20/150)
  2. Ceiling.  
     (Standard = 120/150)
  3. Anzahl der Stufen.  
     (Standard = 7)

Für das Simple UI gibt es vier Menüeinträge.  Die ersten drei entsprechen
denen des Stepped Rampings.

  1. Floor.  
     (Standard = 20/150)
  2. Ceiling.  
     (Standard = 120/150)
  3. Anzahl der Stufen.  
     (Standard = 5)
  4. Turbo-Stil.  
     (Standard = 0, kein Turbo)

**Standardwerte können sich für jede Taschenlampe unterscheiden.  Die
angegebenen Werte sind nur Beispiele.**

Um die Floor Stufe zu konfigurieren, den Taster entsprechend der Stufe (von
150) Mal drücken.  Um die kleinstmögliche Stufe zu wählen, ein Mal drücken.

Um die Ceiling Stufe zu konfigurieren, reduziert jeder Klick die Helligkeit um
eine Stufe.  Ein Klick setzt die höchstmögliche Helligkeit, zwei Klicks die
zweithöchste Helligkeit, drei Klicks die dritthöchste Helligkeit usw.  Um den
Standardwert von 120/150 zu setzen, muss demnach 31 Mal gedrückt werden.

Wenn die Anzahl der Stufen konfiguriert wird, kann der Wert alles zwischen 1
und 150 betragen.  Ein Wert von 1 ist ein spezieller Fall.  Es setzt die
Helligkeit mittig zwischen Floor und Ceiling.


Versionsprüfung
---------------

Dies erlaubt dem Nutzer herauszufinden, welche Firmware auf der Taschenlampe
installiert ist.  Das Format besteht (üblicherweise) aus einer Modellnummer
und einem Datum.
`MODEL.YYYY-MM-DD`

  - `MODEL`: Modellnummer  
    (üblicherweise `BBPP`, wobei `BB` die Hersteller ID und `PP` die
    Produkt ID bedeutet)
  - `YYYY`: Jahr
  - `MM`: Monat
  - `DD`: Tag

Das Format der Versionsprüfungsausgabe hat sich über die Zeit einige Male verändert.
Der Nutzer sollte die Versionsprüfungsausgabe aufschreiben und mit den
untengenannten Mustern vergleichen.

Die Modellnummer ist äußerst wichtig, wenn eine neue Firmware installiert wird.
Es sollte sichergestellt werden, dass die neue Firmware genau die Modellnummer der
alten Firmware besitzt. Weitere Details sind in [Welche HEX-Datei](which-hex-file.de.md)
dokumentiert. Die [MODELS](../MODELS)-Datei kann dafür verwendet werden,
die Modellnummer einem Namen einer .hex Datei zuzuordnen.

## Formate der Versionsprüfung

Die Versionsprüfung sollte durch Blinken eine Zahlenfolge in einem der
folgenden Formate anzeigen:

  - `MODEL-YYYY-MM-DD-SEIT-DIRTY`
    Anduril 2 ab 2023-12.  
    "SEIT" und "DIRTY" können weggelassen werden.
    Ein schnelles Flackern bedeutet ein Interpunktionszeichen zwischen Abschnitten.
    - `MODEL`: Modellnummer
    - `YYYY-MM-DD`: Jahr, Monat, Tag. Hier wird der neuesten Release-Tag von Git, nicht
      jedoch das Erstellungsdatum, verwendet
    - `SEIT`: Wie viele Commits seit dem letzten offiziellen Release-Tag?
    - `DIRTY`: Fügt eine "-1" am Ende hinzu, sollte das Repository ohne Commit
      lokal verändert worden sein.

  - `NNNN-YYYY-MM-DD`
    Anduril 2 ab 2023-05.  
    Eine Modellnummer sowie Erstellungsdatum, mit schnellem Flackern
    zwischen Abschnitten.
    - `NNNN`: Modellnummer
    - `YYYY`: Jahr
    - `MM`: Monat
    - `DD`: Tag

  - `YYYYMMDDNNNN`
    Anduril 2 vom 2023-05 oder früher.  
    Ein Erstellungsdatum sowie eine Modellnummer.

  - `YYYYMMDD`
     Diese ist eine alte Anduril 1 Version. Sie bezeichnet lediglich das Erstellungsdatum.  
     Wenn der Modellname nicht offensichtlich ist, kann er in der PRODUCTS-Datei
     nachgeschlagen werden.

  - `1969-07-20`
    Das Datum der ersten menschlichen Berührung des Mondes. Diese Zahl deutet darauf hin,
    dass der Ersteller der Firmware irgendeinen Fehler gemacht hat.

Sollte die Versionsprüfung keine Modellnummer ausgeben, kann in der PRODUCTS-Datei
nachgeschlagen werden, um herauszufinden welches Firmware-Modell vermutlich
verwendet wird:

  https://bazaar.launchpad.net/~toykeeper/flashlight-firmware/anduril2/view/head:/PRODUCTS


Schutzfunktionen
----------------

Anduril beinhaltet einen Schutz gegen Tiefentladung
(LVP - Low Voltage Protection) und eine Temperaturregulierung.

LVP sorgt dafür, dass die Helligkeit automatisch reduziert wird, wenn der Akku
einen niedrigen Ladestand erreicht.  Wenn der Ladestand kritisch wird,
schaltet sich die Lampe selbstständig ab.  Dies ist bei 2,8 V der Fall.  LVP
Anpassungen erfolgen plötzlich in großen Schritten.

Die Temperaturregulierung versucht, die Taschenlampe vor Überhitzung zu
schützen und dabei gleichzeitig die höchstmögliche, gewählte Helligkeit
beizubehalten, je nach gesetztem Temperaturlimit.  Helligkeitsanpassungen der
Temperaturregulierung erfolgen gleichmäßig in kleinen Schritten, die für einen
Menschen kaum sichtbar sind.


Aux-LEDs / Taster-LEDs
----------------------

Manche Taschenlampen haben Aux- oder Taster-LEDs.  Diese können für
verschiedene Zwecke konfiguriert werden, während die Haupt-LEDs aus sind.  Es
gibt einen Aux-LED Modus für den normalen "Aus"-Modus und einen weiteren
Aux-LED Modus für die Tastensperre.  Dies ermöglicht eine unterschiedliche
Konfiguration, um auf den ersten Blick zu erkennen, ob die Lampe gesperrt ist.

Aux-LED Modi beinhalten typischerweise:

  - Aus
  - Niedrig
  - Hoch
  - Blinkend

Um die Aux-LEDs zu konfigurieren, zuerst in die gewünschte Betriebsart
wechseln (Aus oder Tastensperre) und dann den Taster 7 Mal drücken.  Dies
sollte die Aux-LEDs in den nächsten unterstützen Modus wechseln.

  - `7C`: Nächster Aux-LED Modus.

Falls die Aux-LEDs ihre Farbe ändern können, gibt es weitere Aktionen um die
Farbe einzustellen.  Genau wie zuvor, nur dass der Taster beim letzten Klick
gehalten und beim Erreichen der gewünschten Farbe wieder losgelassen wird.

  - `7H`: Nächste Aux-LED Farbe.

Bei den meisten Taschenlampen ist die Sequenz der Farben:

  - Rot
  - Gelb (Rot+Grün)
  - Grün
  - Cyan (Grün+Blau)
  - Blau
  - Lila (Blau+Rot)
  - Weiß (Rot+Grün+Blau)
  - Disco (schnelle zufällige Farben)
  - Regenbogen (wechselt nacheinander durch alle Farben)
  - Spannung (nutzt die Farbe um die Akkuspannung anzuzeigen)

Im Spannungsmodus folgt die Farbe der gleichen Abfolge wie die Farben in einem
Regenbogen... wobei Rot einen leeren Akku anzeigt und Lila einen vollen Akku.

![Spannungsmodus Farben](battery-rainbow.png)

Für Taschenlampen mit einem hinterleuchteten Taster bleibt die
Taster-LED üblicherweise an, während die Haupt-LEDs eingeschaltet sind.  Die
Helligkeit folgt den Haupt-LEDs -- aus, niedrig oder hell.

Für Taschenlampen mit RGB-Taster-LED zeigt die Taster-LED bei der Benutzung
den Ladestand auf die gleiche Weise an, wie die Aux-LEDs im Spannungsmodus.

Für Taschenlampen mit Aux-LEDs auf der Vorderseite bleiben die Aux-LEDs
normalerweise aus, währen die Haupt-LEDs eingeschaltet sind oder die
Taschenlampe anderweitig aktiv ist.  Die Aux-LEDs sind bei den meisten Lampen
nur aktiv, wenn die Lampe im Bereitschaftsmodus ist.

Sollte eine Taschenlampe einfarbige Aux-LEDs statt RGB Aux-LEDs haben, blinken
die Aux-LEDs schnell im "Aus"-Modus sobald die Spannung niedrig ist.


Menü für sonstige Einstellungen
-------------------------------

Manche Modelle haben ein Menü für weitere Einstellungen, die in keines der
anderen Menüs passen.  Diese Einstellungen sind durch `Aus -> 9H` im Advanced UI
zu erreichen.

Diese Einstellungen sind nacheinander:

  - Tintramping-Stil:  (bei manchen Lampen)

    0 : gleichmäßig (beide Kanäle gleichmäßig mischen)  
    1 : nur mittlere Farbtemperatur  
    2 : nur äußere Farbtemperaturen (immer nur ein Kanal aktiv)  
    3+: Stepped Ramping mit 3+ Stufen

  - Starthilfe-Stärke:  (bei manchen Lampen)

    Manche Lampen sind dafür bekannt, in niedrigen Stufen nur langsam zu
    starten.  Daher haben sie eine Starthilfe-Option, welche die LED nach dem
    Einschalten für wenige Millisekunden auf einer höheren Helligkeit
    betreibt.  Diese Einstellung gibt an, wie hell dieser Impuls sein soll.

    Der Wert kann zwischen 1 und 150 betragen, liegt üblicherweise aber
    zwischen 20 und 50.

Diese Einstellungen sind abhängig von der Hardware und sind möglicherweise
nicht bei allen Taschenlampen vorhanden.  Die Anzahl der Einstellungen in
diesem Menü hängt von der Hardware und der Firmwareversion ab.


Kanal Modi (alias Tintramping oder Multi-Kanal Steuerung)
---------------------------------------------------------

Manche Taschenlampen haben mehr als einen Satz von LEDs, welche genutzt werden
können, um die Farbtemperatur des Lichts einzustellen.
Diese Taschenlampen haben Funktionen wie Tintramping oder Multi-Kanal Steuerung.

Bei diesen Modellen gibt es eine globale Tastenbelegung,
die immer funktioniert, sofern sie nicht von einem bestimmten Modus
überschrieben wurde:

  - `3C`: Zum nächsten Kanal-Modus
  - `3H`: Aktuellen Kanal-Modus einstellen (z.B. Farbtemperatur ändern)
  - `9H`: Einstellungsmenü für die Kanal-Modi

Genaue Kanal-Modi hängen von der genauen Art der Taschenlampe ab.
Sollte, zum Beispiel, eine Taschenlampe mit kühl-weißen LEDs, warm-weißen LEDs
sowie roten LEDs ausgestattet sein, könnte die Taschenlampe einige
Kanal-Modi besitzen:

  - Weißmischung (regelbare Farbtemperatur / Tintramping)
  - Nur Rot
  - Automatische Farbtemperatur

Für eine solche Taschenlampe, kann mit `3C` durch diese Kanal-Modi
gewechselt werden... Weiß, dann Rot, dann Automatik, dann wieder Weiß.

Außerdem kann im "Weißmischungs-Modus" mit `3H` das
Mischverhältnis der warm- und kühl-weißen Farbtemperaturen manuell
gesteuert werden.

Schließlich kann entschieden werden, lediglich einige und nicht sämtliche Modi
aktiviert zu lassen, und einige Modi auszuschalten. Diese Einstellungen sind
durch "Ein -> 9H" im Advanced UI zu erreichen. Solange der letzte Klick gehalten
wird, wechselt die Lampe, jeweils mit einem Blinken, durch die Kanal-Modi
nacheinander durch. Um den gewünschten Modus auszuschalten wird nach dem jeweiligen
Blinken die Taste losgelassen und danach während des schnellen Flackerns nicht
mehr gedrückt (0 Klick). Danach wird der ausgeschaltete Modus nicht mehr im Zyklus
der Kanal-Modi vorhanden sein. Um später den Kanal-Modus wieder zu aktivieren,
wird genauso vorgegangen, jedoch 1 Klick statt 0 Klick beim schnellen Flackern
eingegeben.

Eine Taschenlampe kann viele unterschiedliche Kanal-Modi besitzen. Es sollte nicht
davor gescheut werden, unbenutzte Modi auszuschalten. Dadurch werden alle anderen Modi
besser zu erreichen sein.

Sollte lediglich ein Kanal-Modus noch aktiv bleiben, dann fällt `Ramping -> 3C`
wieder zum 1-Kanal Verhalten zurück -- Wechsel zwischen Smooth Ramping und Stepped Ramping.
Sollte der Kanal-Modus sich nicht durch `3H` justieren lassen, fällt auch `Ramping -> 3H`
zum 1-Kanal Verhalten zurück -- momentaner Turbo.

Das Menü für sonstige Einstellungen (`Aus -> 9H`) beinhaltet möglicherweise
eine Einstellung für den Tintramping-Stil. Durch die Eingabe unterschiedlicher Zahlen
stehen einige Stile zur Verfügung:

  0 : Gleichmäßiges Ramping  
  1 : Nur mittlere Farbtemperatur  
  2 : Nur äußere Farbtemperaturen  
  3+: Stepped Ramping mit 3+ Stufen

Diese Einstellung gilt einzig für Modi, bei denen Ramping zwischen den Kanälen möglich ist,
sowie nur dann, wenn für den Modus das standardmäßige `3H` Verhalten gilt.
Bestimmte Kanal-Modi verhalten sich möglicherweise anders.


UI Referenztabelle
------------------

Diese Tabellen stellen alle Tastensequenzen für Anduril in
einem Abschnitt dar:

### "Aus" Modus

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Aus              | Alle   | `1C`      | Ein (Ramping, gespeicherte Helligkeit)
| Aus              | Alle   | `1H`      | Ein (Ramping, Floor)
| Aus              | Alle   | `2C`      | Ein (Ramping, Ceiling)
| Aus              | Simple | `2H`      | Ein (Momentane Ceiling Stufe)
| Aus              | Full   | `2H`      | Ein (Momentaner Turbo)
| Aus              | Alle   | `3C`      | Spannungsprüfung
| Aus              | Full   | `3H`      | Strobe Modus (zuletzt genutzter Strobe)
| Aus              | Alle   | `4C`      | Tastensperre
| Aus              | Full   | `5C`      | Momentan-Modus
| Aus              | Full   | `6C`      | Taktischer Modus
| Aus              | Full   | `7C`      | Aux-LEDs: Nächstes Muster
| Aus              | Full   | `7H`      | Aux-LEDs: Nächste Farbe
| Aus              | Full   | `9H`      | Menü für sonstige Einstellungen (je nach Lampe)
|                  |        |           | ?1: Tintramping Stil
|                  |        |           | ?2: Starthilfe-Stärke
| Aus              | Full   | `10C`     | Aktiviert Simple UI
| Aus              | Simple | `10H`     | Deaktiviert Simple UI
| Aus              | Full   | `10H`     | Simple UI Konfigurationsmenü:
|                  |        |           | 1: Floor
|                  |        |           | 2: Ceiling
|                  |        |           | 3: Stufen
|                  |        |           | 4: Turbo-Stil
| Aus              | Alle   | `13H`     | Werkseinstellungen (bei manchen Lampen)
| Aus              | Alle   | `15+C`    | Version ausgeben

### Ramping-Modus

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Ramping          | Alle   | `1C`      | Aus
| Ramping          | Alle   | `1H`      | Helligkeit erhöhen (automatischer Richtungswechsel)
| Ramping          | Alle   | `2H`      | Helligkeit reduzieren
| Ramping          | Alle   | `2C`      | Zu/von Ceiling/Turbo gehen (konfigurierbar)
| Ramping          | Full   | `3C`      | Ramping-Stil ändern (smooth / stepped)
| Ramping          | Full   | `6C`      | (Funktion wie oben, bei Lampen mit mehreren Kanälen)
| Ramping          | Full   | `3H`      | Momentaner Turbo (beim Modus ohne Tintramping)
| Ramping          | Full   | `4H`      | Momentaner Turbo (bei Lampen mit mehreren Kanälen)
| Ramping          | Alle   | `4C`      | Tastensperre
| Ramping          | Full   | `5C`      | Momentan-Modus
| Ramping          | Full   | `5H`      | Sunset-Timer aktivieren, 5 Minuten hinzufügen
| Ramping          | Full   | `7H`      | Ramping-Konfigurationsmenü (für aktuellen Ramping-Stil)
|                  |        |           | 1: Floor
|                  |        |           | 2: Ceiling
|                  |        |           | 3: Geschwindigkeit/Stufen
| Ramping          | Full   | `10C`     | Manuelles Speichern aktivieren und aktuelle Helligkeit speichern
| Ramping          | Full   | `10H`     | Menü für zusätzliche Ramping-Einstellungen:
|                  |        |           | 1: automatisches Speichern aktivieren
|                  |        |           | 2: Timer für manuelles Speichern setzen
|                  |        |           | 3: Helligkeit nach Moon erhöhen oder nicht
|                  |        |           | 4: Advanced UI Turbo-Stil
|                  |        |           | 5: Smooth Steps (gleitende Stufen)

### Multi-Kanal Steuerung

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Alle             | Alle   | `3C`      | Zum nächsten Kanal-Modus (z.B. nächsten Farbmodus)
| Alle             | Alle   | `3H`      | Farbtemperatur ändern (abhängig vom Modus)
| Alle             | Full   | `9H`      | Einstellungsmenü für die Kanal-Modi:
|                  |        |           | N: 1 Klick (0 Klick) um Modus #N zu aktivieren (deaktivieren)

### Tastensperre Mode

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Tastensperre     | Alle   | `1C`/`1H` | Momentaner Moon (niedrigstes Floor)
| Tastensperre     | Alle   | `2C`/`2H` | Momentaner Moon (höchstes Floor oder manuell gespeicherte Helligkeit)
| Tastensperre     | Alle   | `3C`      | Entsperren (zum "Aus" Modus gehen)
| Tastensperre     | Alle   | `3H`      | Zum nächsten Kanal-Modus (sollten mehrere aktiviert sein)
| Tastensperre     | Alle   | `4C`      | Ein (Ramping, gespeicherte Helligkeit)
| Tastensperre     | Alle   | `4H`      | Ein (Ramping, Floor)
| Tastensperre     | Alle   | `5C`      | Ein (Ramping, Ceiling)
| Tastensperre     | Full   | `7C`      | Aux-LEDs: Nächstes Muster
| Tastensperre     | Full   | `7H`      | Aux-LEDs: Nächste Farbe
| Tastensperre     | Full   | `10H`     | Auto-Lock Konfigurationsmenü
| Tastensperre     |        |           | 1: Timeout setzen (0 = kein Auto-Lock)

### Strobe / Stimmungs Modi

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Strobe (alle)    | Full   | `1C`      | Aus
| Strobe (alle)    | Full   | `2C`      | Nächster Strobe-Modus
| Strobe (alle)    | Full   | `3C`      | Zum nächsten Kanal-Modus (pro Strobe-Modus gespeichert)
| Strobe (alle)    | Full   | `4C`      | Vorheriger Strobe-Modus
| Strobe (alle)    | Full   | `5C`      | Momentan-Modus (mit aktuell genutztem Strobe)
| Party Strobe     | Full   | `1H`/`2H` | Schneller / langsamer
| Tactical Strobe  | Full   | `1H`/`2H` | Schneller / langsamer
| Polizei Strobe   | -      | -         | Keine (zuletzt benutzte Helligkeit beim Ramping wird verwendet)
| Gewitter         | Full   | `1H`      | Aktuellen Blitz unterbrechen oder neu starten
| Kerze            | Full   | `1H`/`2H` | Heller / dunkler
| Kerze            | Full   | `5H`      | Sunset-Timer aktivieren, 5 Minuten hinzufügen
| Fahrrad          | Full   | `1H`/`2H` | Heller / dunkler

### Blink / Werkzeug Modi

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Batt Prüfung     | Alle   | `1C`      | Aus
| Batt Prüfung     | Full   | `2C`      | Nächster Blink-Modus (Temperaturprüfung, Signal, SOS)
| Batt Prüfung     | Full   | `3C`      | Zum nächsten Kanal-Modus (nur für das Ausblinken von Zahlen)
| Batt Prüfung     | Full   | `7H`      | Spannungskalibrierungsmenü
|                  |        |           | 1: Spannungskorrekturfaktor
|                  |        |           |    ...
|                  |        |           |    5: -0.10V
|                  |        |           |    6: -0.05V
|                  |        |           |    7: keine Korrektur
|                  |        |           |    8: +0.05V
|                  |        |           |    9: +0.10V
|                  |        |           |    ...
|                  |        |           | 2: Timeout (Sekunden) für die Spannungsanzeige

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Temp Prüfung     | Full   | `1C`      | Aus
| Temp Prüfung     | Full   | `2C`      | Nächster Blink-Modus (Signal, SOS, Spannungsprüfung)
| Temp Prüfung     | Full   | `7H`      | Menü zur Temperaturkonfiguration

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Signal           | Full   | `1C`      | Aus
| Signal           | Full   | `1H`      | Signalzeit konfigurieren
| Signal           | Full   | `2C`      | Nächster Blink-Modus (SOS, Spannungsprüfung, Temperaturprüfung)

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| SOS              | Full   | `1C`      | Aus
| SOS              | Full   | `2C`      | Nächster Blink-Modus (Spannungsprüfung, Temperaturprüfung, Signal)

### Momentan-Modus

| Modus            | UI     | Taste                  | Aktion
| :----            | :--    | -----:                 | :-----
| Momentan         | Full   | Alle                   | Ein (bis Taster losgelassen wird)
| Momentan         | Full   | **Strom unterbrechen** | Momentan-Modus verlassen

### Taktischer Modus

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Taktischer Modus | Full   | `1H`      | Ceiling (taktische Position 1)
| Taktischer Modus | Full   | `2H`      | Floor (taktische Position 2)
| Taktischer Modus | Full   | `3H`      | Strobe (taktische Position 3)
| Taktischer Modus | Full   | `6C`      | Beenden (Taschenlampe ausschalten)
| Taktischer Modus | Full   | `7H`      | Einstellen der Taktischen Modi
|                  |        |           | 1: taktische Position 1
|                  |        |           | 2: taktische Position 2
|                  |        |           | 3: taktische Position 3

### Konfigurationsmenüs

| Modus            | UI     | Taste     | Aktion
| :----            | :--    | -----:    | :-----
| Konfigmenüs      | Full   | Halten    | Aktuellen Eintrag überspringen
| Konfigmenüs      | Full   | Loslassen | Aktuellen Eintrag konfigurieren
|                  |        |           | (zur Zahleingabe)
| Nummerneingabe   | Full   | Klicken   | Zum aktuellen Wert 1 hinzuzählen
| Nummerneingabe   | Full   | Halten    | Zum aktuellen Wert 10 hinzuzählen
