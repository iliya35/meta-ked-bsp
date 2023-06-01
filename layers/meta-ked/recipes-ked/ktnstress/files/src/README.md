# Kontron BL Stresstest

Das Kontron Stress Testprogramm `ktnstress.sh`  wird für Belastungs- und 
Temperaturtests von Baugruppen eingesetzt. Je nach Kongfiguration wird die CPU
mit 50% oder 100% Rechenlast betrieben. Die Stresstests werden mit `stress-ng`
ausgeführt.

Bei Systemen mit mehreren CPU Cores werden beim 50% Test nur die Hälfte aller
Cores verwendet. Bei Systemen mit nur einer CPU wird der Test in einer Schleife
für 10 Sekunden gestartet und wieder für 10 Sekunden pausiert.

Die Tests starten 5 Minuten nach Start des `ktnstress` Testprogramms

Die Konfiguration der Tests ist über folgende Interfaces möglich

- Defaultkonfiguration über `/etc/ktnstress.conf`
- Testauswahl über DIP Switches
- Konfiguration über Kommandozeilenoptionen

Als weitere Konfiguration gibt es die Möglichkeit einen RAM Stresstest
mit `memtester` auszuführen. Die Ausgaben des Tests sind unter
`/home/root/memtest_<DATECODE>.log` zu finden.

Die Ausgaben des `ktnstressd`, der beim Start des Systems ausgeführt wird,
werden unter `/var/log/ktnstressd` gespeichert.

## Konfigurationsoptionen über DIP Switch

- DIP Switch 1: Start Stresstest mit 50% Last
- DIP Switch 2: Start Stresstest mit 100% Last
- DIP Switch 3: Start RAM Test

## Unterstützte Baugruppen

- BL imx6ul, imx6ull
- BL stm32mp
