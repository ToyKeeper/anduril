[![de](https://img.shields.io/badge/lang-de-green)](https://github.com/ToyKeeper/anduril/blob/trunk/docs/which-hex-file.de.md)
[![en](https://img.shields.io/badge/lang-en-red)](https://github.com/ToyKeeper/anduril/blob/trunk/docs/which-hex-file.md)

# Wie man feststellt, welche .hex Datei zu verwenden ist

Die falsche Firmware zu verwenden kann dazu führen,
dass eine Taschenlampe nicht mehr funktioniert.
Beim Flashen der Firmware ist daher sicherzustellen, dass
**die Modellnummer korrekt ist**!

Der Produktname reicht *nicht* aus, um die richtige .hex Datei
ausfindig zu machen.  Stattdessen fragt man die Taschenlampe,
welche Firmware benötigt wird:

1. Die Versionsprüfung-Funktion nutzen (15 Mal oder mehr die Taste
   drücken während die Taschenlampe aus ist) um heraus zu finden,
   welche Variante der Firmware installiert ist.  
   **DIE VERSIONSPRÜFUNGSAUSGABE AUFSCHREIBEN**

2. In der [MODELS-Datei](../MODELS) nachschlagen, um Name
   und MCU-Typ ausfindig zu machen

3. Die neuste Version der Firmware für dieses Modell
   finden (oder erstellen).  
   https://github.com/ToyKeeper/anduril/releases

Das Format der Versionsprüfungsausgabe hat sich über die Zeit einige
Male verändert. Im
[geeigneten Abschnitt der Anleitung](anduril-manual.de.md#formate-der-versionspr%C3%BCfung)
wird über diese Formate und deren Verwendung aufgeklärt. In den
meisten Fällen, sollte es eine Modellnummer und ein Datum geben...
**die Modellnummer wird benötigt** um die richtige .hex Datei
zu finden.

