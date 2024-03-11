[![de](https://img.shields.io/badge/lang-de-green)](https://github.com/ToyKeeper/anduril/blob/trunk/README.de.md)
[![en](https://img.shields.io/badge/lang-en-red)](https://github.com/ToyKeeper/anduril/blob/trunk/README.md)

# Anduril Flashlight Firmware + FSM Flashlight UI Toolkit

Anduril ist eine Benutzerschnittstelle für Taschenlampen.  Sie wird
mit FSM programmiert, ein UI-Toolkit für Taschenlampen.

Was bedeutet FSM? Die "S" und "M" bedeuten "Spaghetti Monster", und der "F"
kann jedes beliebige Wort sein, so wie "Flashlight" oder was auch immer zur
Laune passt.

FSM bedeutet auch "Finite State Machine" (endlicher Zustandsautomat).
Eine FSM ist die Art der Abstraktion für die Beschreibung von
Benutzerschnittstellen. Sie ist wie ein in funktionsfähigen 
Code verwandeltes Ablaufdiagramm.


## Anduril Benutzen

Anduril hat viele Features. Die einzigen Merkmale, die *wirklich* für
die Verwendung benötigt werden, sind:

  - Klicken um ein- und auszuschalten
  - Halten um die Helligkeit zu ändern

Das [Benutzerhandbuch](docs/anduril-manual.de.md) erläutert alle Details,
die für eine umfassende Benutzung vonnöten sind.

Um etwas über die neuesten Veränderungen zu erfahren, kann das
[ChangeLog](ChangeLog.md) gelesen werden.

Um Information über spezifische Hardware-Modelle und ggf. deren
Gerät-spezifische Dokumentation zu finden, kann man in dem
[hw/](hw/)-Ordner herumstöbern. Beispielsweise können Informationen über
die zur Verfügung stehenden Kanal-Modi ausfindig gemacht werden. 


## Firmware-Flashen

Die neuesten Updates sind durch flashen der Firmware zu erhalten.

Um Firmware zu flashen, werden einige Dinge benötigt:

  - Eine Firmware ".hex"-Datei.  Hier liest man
    [wie man feststellt, welche .hex-Datei zu verwenden ist](docs/which-hex-file.de.md). 
  
  - Ein Programmieradapter. Welcher Typ hängt davon ab,
    welcher MCU-Chip in der Taschenlampe verwendet wird. Der Adapter
    könnte ein 3-polige UPDI Adapter (für Attiny1616 sowie neuere)
    oder ein 6-polige AVR ISP Adapter (für Attiny85, Attiny1634) sein.
    
    Ein **UPDI** Adapter benutzt üblicherweise "Pogo Pins" (gefederte
    Kontaktstiften), die an die Kupfer-Pads gehalten werden, welche sich
    auf der Treiber-Platine befinden.
    
    Ein **AVR ISP** Adapter besteht typischerweise aus zwei Teilen -- einem
    USB-Gerät (so wie einem USBASP) sowie einem Adapter (z.B. Pogo-Pin Adapter
    oder SOIC8-Klemme). SOIC8 ist am unpraktischsten und wird hauptsächlich
    für alte Modelle mit Attiny85-Chip verwendet.
  
  - Eine Anwendung zum Flashen. Passende Anwendungen sind zum Beispiel,
    `avrdude`, `pymcuprog`, oder `ZFlasher`.
  
  - Ein Rechner oder Smartphone. So gut wie jeder moderne Rechner oder 
    modernes Smartphone sollte für das Flashen geeignet sein, solange der
    Adapter eingesteckt und die Flashing-Anwendung ausgeführt werden kann.

Eine besonders praktische Anleitung ist hier: https://anduril.click/ .

Für mehr Informationen bzgl. dem Installieren von Flashing-Anwendungen,
siehe unten: [Flashing-Anwendungen](#flashing-anwendungen).


## Kompilieren

Die Firmware kann tiefgehend und individuell angepasst werden,
indem sie modifiziert und eine eigene Version kompiliert wird.
Hirzu wird explizit aufgefordert.

Um die Firmware zu kompilieren, ist die Nutzung eines Linux-Rechners
dringend empfohlen, worauf idealerweise Debian oder Ubuntu läuft
(obwohl jede Distribution funktionieren sollte). Virtuelle Maschinen sind
gut geeignet zum Beispiel solche, die durch WSL in Windows ausgeführt werden
können. Zusätzlich gibt es einen Docker-Container (TODO: hier verlinken), falls
dienlich.

### Voraussetzungen

- AVR Toolchain-Pakete:  
  `sudo apt install gcc-avr avr-libc binutils-avr`

- Sonstige Pakete:  
  `sudo apt install git wget unzip bash`

- Atmel DFPs (Device Family Packs).  Ein kleines Skript wird bereitgestellt,
  um diese herunterzuladen sowie zu installieren:  
  `./make dfp`

### Build-Prozesse

Das in diesem Repository vorhandene `make`-Skript sollte verwendet werden.
Eine Gebrauchsanleitung wird durch `./make --help` bereitgestellt. In den
meisten Fällen, um alle verfügbaren Build-Ziele zu kompilieren, sollte
lediglich `./make` ausreichen.  Alternativ kann ein Suchbegriff 
angegeben werden, um lediglich ausgewählte Build-Ziele zu kompilieren
-- beispielsweise führt der Befehl `./make hank boost` zum Kompilieren
der Firmware-Varianten für Hanklights mit den optionalen Boost-Treibern.

Die kompilierte Firmware wird im `hex/`-Ordner abgelegt und ist bereit für die
Verwendung durch eine Flashing-Anwendung.  


## Benutzerspezifische Einstellungen

Durch Festlegen benutzerspezifischer Einstellungen im `users/benutzername/`-Ordner
und durch die Angabe des Benutzernamens kann der Build-Prozess
[individuell angepasst werden](docs/per-user-config.de.md).
Hierdurch können Lieblingseinstellungen automatisch angewendet werden, ohne
die ursprünglichen Quelldateien anzupassen.


## Flashing-Anwendungen

### AVRdude

AVRdude wird üblicherweise durch einen Einzelbefehl installiert:

`sudo apt install avrdude`

### PyMCUprog

Sollte für das Firmware-Flashen `pymcuprog` verwendet werden, sind
einige Extras vonnöten:

```sh
sudo apt install python3 python3-pip python3-venv
python3 -m venv .venv
source .venv/bin/activate
pip install pymcuprog
```

Jedes Mal wenn eine neue Shell gestartet wird und `pymcuprog` verwendet
werden soll, muss `source .venv/bin/activate` ausgeführt werden.
Die Aktivierung dauert bis zum Beenden der Shell an oder bis `deactivate`
eingegeben wird.


## Mitwirken

Es gibt mehrere Möglichkeiten mitzuwirken...

  - ToyKeeper hat für Spenden eine [Patreon Seite](https://patreon.com/ToyKeeper)
    damit bildlich und wortwörtlich das Licht nie ausgeht.
  
  - Pull-Requests oder Korrekturen können eingereicht werden um das Projekt
    unmittelbar zu verbessern. Diese können Code, Dokumentation, Hilfsanwendungen sein
    oder alles was nützlich wäre.
  
  - Fehlerberichte für jegliche aufgefundenen Probleme.
  
  - Fragen in öffentlichen Foren beantworten. Kommen immer wieder die gleichen
    Fragen auf, kann eine Aufnahme in die Dokumentation in Erwägung gezogen werden.
  
  - Taschenlampenhersteller können aufgefordert werden, diese Firmware zu verwenden und
    deren Entwicklung zu unterstützen.

