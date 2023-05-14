# what
Audio-Klassifizier für die beiden Wörter "yes" und "no".

Das Projekt in im Zusammenhang eines drei wöchigen Projekts im Modul "Humanide Robotik" im gleichnamigen Studiengang (B.Eng.) an der Berliner Hochschule für Technik (BHT) entstanden.
Ziel war es ein kleines Audio-Projekt für eine Interaktion mit einem Roboter zu gestalten.


## Projektidee
Manchmal träumt man vor sich hin und hört seinem Gegenüber nicht richtig zu. Inspirtiert vom Träumen habe ich einen Interaktion geschrieben, bei dem ein Audiosignal (2 Sekdunden) vom Programm erkannt werden soll. Oder halt auch nicht, weil es gerade geträumt hat. Also fragt es noch einmal nach: What?!

## Umsetzung des Projekts im Code
1. In einem ersten Schritt wird das gesprochene Wort (maximale Länge von 2 Sekdunden) extrahierte.
2. Die extrahierte Audioaufnahme wird mit mittels der FFT transformierte.
3. Die Klassifikation in "yes", "no" und "unbestimmt" erfolgt über einen Parametervergleich mit einem bereits vorhandenen Datenset.
4. Das Datenset kann um die neue Aufnahme erweitert werden, um zukünftige Aufnahmen besser zu klassifizieren.


## Erläuterungen zur Datenstruktur
* WHAT: Interkation
* GUESS: Code zur Berechnung des erkannten Audiosignals
* YES/NO: Code für das Update des Datensets
* DATASET: Audioaufnahmen für das "Grunddatenset"


## Verwendung
Zum Abspielen der Interaktion ist ein Windows-PC notwendig, auf welchem der SkriptExecutor heruntergeladen werden muss. Der SkriptExetutor kann so bereits für das Programm "what" verwendet werden.


## Verwendete Software
Der SkriptExecutor wurde vom betreunden Professor Manfred Hild (BHT) geschrieben und uns für die Veröffentlichung unseres Projekt zur Verfügung gestellt.


## Autoren
Roya Elisa Eskandani
