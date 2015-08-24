#ifndef INTERFACEIO_H
#define INTERFACEIO_H

#include "Arduino.h"
#include <LiquidCrystal.h> // inclure également dans le .ino
//#include "Bouton.h"    // gère un bouton poussoir



// Affectation des entrées / sorties
enum ArduinoPinout
{
	PIN_RS					= 2,	// broche de l'autorisation de lecture du LCD
	PIN_E					= 3,	// broche E (activation du registre) du LCD
	PIN_D4					= 4,	// broche D4 du LCD
	PIN_D5					= 5,	// broche D5 du LCD
	PIN_D6					= 6,	// broche D6 du LCD
	PIN_D7					= 7,	// broche D7 du LCD

	PIN_BP_MENU				= 8,	// pour commander le réglage
	PIN_BP_H				= 9,	// pour le réglage de l'heure
	PIN_BP_MIN				= 10,	// pour le réglage des minutes

	PIN_CMD_MOTEUR			= 11,	// commande du moteur
	PIN_CAPTEUR_POS_MOTEUR	= 12,	// capteur de position du moteur
	PIN_ENABLE_MOTEUR		= 13	// autorise le moteur à tourner
};

static const int VITESSE_MOTEUR = 128; // [0;255]

class InterfaceIO
{
public:
	InterfaceIO();

	void afficherMessage(const String & msg, int ligne)
	{
		m_lcd.setCursor(0, ligne);
		m_lcd.print(msg);
	}

	void afficherHeure(int heures, int minutes, int secondes, int ligne);
	void effacerEcran(void);

	//const Bouton & getBPHeure(void) const { return BPHeure; }

	//void listen(void)
	//{
	//	BPHeure.listen();
	//}

private:
	LiquidCrystal	m_lcd;	// LCD
	//Bouton BPHeure;
};

#endif //INTERFACEIO_H