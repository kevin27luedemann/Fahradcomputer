/*
 * SD_test_c.c
 *
 * Created: 21.01.2016 20:16:00
 * Author : Lüdemann
 */ 

#define F_CPU 8000000

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>

#include "mmc_config.h"
#include "file.h"
#include "fat.h"
#include "mmc.h"

int main(void)
{
    // Versuch Karte zu Initialisieren, bis es klappt.
	// Unbedingt so, weil die Initialisierung nicht immer
	// auf Anhieb klappt.
    while (FALSE == mmc_init()){}
	
	// Fat initialisieren. Nur wenn das klappt sind weitere
	// Aktionen sinnvoll, sonst endet das Programm.
	if (!fat_loadFatData())
		return -1;
	
	// Dateinamen muessen in diesem Format sein
	// Man beachte die Größe des Arrays und die Großbuchstaben!
	uint8_t file_name[] = "test.txt";
	
	// String zum in die Datei schreiben.
	uint8_t str[] = "Hallo Datei!";
	
	// Wenn Datei nicht existiert
	if (FALSE == ffileExsists(file_name))
	{
		ffopen(file_name, 'c'); //Datei existiert nicht, also anlegen
		// Schreibt String auf Karte.
		// Nur richtige Strings koennen mit ffwrites geschrieben werden.
		ffwrites (str);
		
		// Neue Zeile in der Datei.
		// Schreibt Zeilenumbruch in die Text Datei.
		ffwrite ('\n');
		// Für MS-Windows Terminal '\r' anhängen.
		ffwrite ('\r');
		
		// Schließt Datei.
		ffclose();
	}
	
	// Datei existiert, also anhaengen !
	if (TRUE == ffileExsists(file_name))
	{
		ffopen (file_name, 'c');
		// Spult bis zum Dateiende vor um anzuhaengen.
		// Geht auch ohne Option MMC_OVER_WRITE
		ffseek (file.length);
		
		// Schreibt String.
		ffwrites(str);
		
		// Neue Zeile in der Datei.
		// Schreibt Zeilenumbruch in die Textdatei.
		ffwrite ('\n');
		// Für MS-Windows Terminal '\r' anhängen.
		ffwrite ('\r');
		
		// Schließt Datei.
		ffclose();
	}
	
	//Lesen brauche ich noch nicht
	/*
	// Datei existiert, also lesen.
	// Gerade angelegt und beschrieben, oder war schon vorhanden
	// und es wurde was angehaengt?
	if (TRUE == ffileExsists(file_name))
	{
		ffopen (file_name, 'r');
		// Setzen einer Variable und dann runterzählen geht
		// am schnellsten
		unsigned long int seek = file.length;
		
		// Lesen eines chars und Ausgabe des chars.
		// Solange bis komplette Datei gelesen wurde.
		do {
			uputc (ffread());
		} while (--seek);
		
		ffclose();
	}*/
	
    while (1) 
    {
    }
}

