/*
 * net_sd.cpp
 *
 * Created: 27.01.2016 19:37:25
 * Author : Lüdemann
 */ 

#include <avr/io.h>
#include <stdlib.h>



extern "C" {
	#include "mmc_config.h"
	#include "mmc.h"
	#include "file.h"
	#include "fat.h"
};

int main(void)
{
	
	// Dateinamen muessen in diesem Format sein
	// Man beachte die Größe des Arrays und die Großbuchstaben!
	unsigned char file_name[] = "test.txt";
	
	// String zum in die Datei schreiben.
	uint8_t str[] = "Hallo Datei!";
	
	// Wenn Datei nicht existiert
	if (false == ffileExsists(file_name))
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
	if (true == ffileExsists(file_name))
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

   
    while (1) 
    {
    }
}

