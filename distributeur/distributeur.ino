/// Réveil programmable.
/// Gère l'heure actuelle et l'heure programmable ; tout s'affiche sur un
/// écran LCD. A l'heure programmée, la sortie de l'alarme passe à l'état haut.
/// Navigation affichage de l'heure -> réglage heure actuelle -> réglage heure réveil
/// via un BP ; 2 autres BP pour ajuster les heures / les minutes.
/// Les boutons sont gérés avec une classe spécifique Bouton.
/// L'alarme dure une minute. Le déclenchement de l'alarme est activable / désactivable
/// via un appui sur le bouton Heures (quand on est en mode affichage de l'heure actuelle).
/// Lorsque l'alarme est activable, l'heure programmée est affichée en plus de l'heure actuelle.
/// L'ensemble est géré par une machine d'états.

#include <FiniteStateMachine.h>
#include <LiquidCrystal.h>
#include <Time.h>

#include "Bouton.h"    // gère un bouton poussoir
#include "InterfaceIO.h"
#include "AlarmeMgr.h" // gère l'alarme

const bool etatAppui = HIGH;  // état d'un bouton quand on appuie dessus

int indAlarme(0);

Bouton BPReglage(PIN_BP_MENU, etatAppui);
Bouton BPHeure(PIN_BP_H, etatAppui);
Bouton BPMinute(PIN_BP_MIN, etatAppui);
Bouton BPPosMoteur(PIN_CAPTEUR_POS_MOTEUR, etatAppui);

// énumération permettant d'identifier facilement les états du système
// avantage : on peut faire un switch pour gérer les transitions dans la focntion loop
enum IdEtats
{
	AFFICHAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_ACTUELLE,
	REGLAGE_HEURE_REVEIL,
	DISTRIBUTION,
	FIN_DISTRIBUTION,
	SET_NB_ALARMES
};

InterfaceIO m_IO;

// initialisation des états de l'automate
State etatAffichageHeureActuelle(entreeAffichageHeureActuelle, afficherHeureActuelle, NO_EXIT);
State etatReglageHeureActuelle(entreeReglageHeureActuelle, reglageHeureActuelle, NO_EXIT);
State etatReglageHeureReveil(entreeReglageHeureReveil, reglageHeureReveil, NO_EXIT);
State etatDistribution(entreeDistribution, distribuer, NO_EXIT);
State etatFinDistribution(entreeFinDistribution, finDistribuer, sortieFinDistribuer);
State etatNbAlarmes(entreeNbAlarmes, reglageNbAlarmes, NO_EXIT);

// initialisation de l'automate lui-même
FiniteStateMachine automate(etatAffichageHeureActuelle);
IdEtats etatActuel = AFFICHAGE_HEURE_ACTUELLE;

// initialisation du gestionnaire d'alarme
AlarmeMgr alarme;


/// Initialisation du module.
void setup()
{
	// initialisation de la liaison série (pour le debug)
	Serial.begin(9600);
	//delay(500);
	Serial.println("InterfaceIO : init OK");
}

/// Incrémente une heure en controlant la validité.
void incrementerHeure(uint8_t & heure)
{
	if (++heure >= 24)
		heure = 0;

	//Serial.println(heure);
}

/// Incrémente les minutes en controlant la validité.
void incrementerMinutes(uint8_t & minutes)
{
	if (++minutes >= 60)
		minutes = 0;

	//Serial.println(minutes);
}

void reglageHeureReveil(void)
{
	// on scinde le temps en champs exploitables
	tmElements_t heureDecomposee;
	breakTime(alarme.getHeureAlarme(indAlarme), heureDecomposee);

	// réglage de l'heure de réveil
	if (BPHeure.onPress())
	{
		incrementerHeure(heureDecomposee.Hour);
	}

	// réglage des minutes
	if (BPMinute.onPress())
	{
		incrementerMinutes(heureDecomposee.Minute);
	}

	alarme.setHeureAlarme(indAlarme, makeTime(heureDecomposee));

	m_IO.afficherMessage(String("reglage reveil ") + (indAlarme + 1), 0);
	m_IO.afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
}

void reglageHeureActuelle(void)
{
	// on scinde le temps en champs exploitables
	tmElements_t heureDecomposee;
	breakTime(now(), heureDecomposee);

	// réglage de l'heure actuelle
	if (BPHeure.onPress())
	{
		incrementerHeure(heureDecomposee.Hour);
	}

	// réglage des minutes
	if (BPMinute.onPress())
	{
		incrementerMinutes(heureDecomposee.Minute);
	}

	setTime(makeTime(heureDecomposee));
	m_IO.afficherMessage("reglage horloge", 0);
	m_IO.afficherHeure(hour(), minute(), second(), 1);
}

void reveiller(void)
{
	m_IO.afficherHeure(hour(), minute(), second(), 0);
	m_IO.afficherMessage("Debout!", 1);
}

void reglageNbAlarmes(void)
{
	// 
	if (BPHeure.onPress())
	{
		alarme.setNbAlarmes(alarme.getNbAlarmes() - 1);
		m_IO.afficherMessage(String("Nb alarmes : ") + String(alarme.getNbAlarmes()), 0);
	}

	//
	if (BPMinute.onPress())
	{
		alarme.setNbAlarmes(alarme.getNbAlarmes() + 1);
		m_IO.afficherMessage(String("Nb alarmes : ") + String(alarme.getNbAlarmes()), 0);
	}
}

void couperReveil(void)
{
	alarme.setAlarmeActive(false);
}

void distribuer(void)
{
	// rien
}

void finDistribuer(void)
{
	// rien
}

/// Méthode appelée lorsqu'on entre dans l'état "afficher heure actuelle".
void entreeAffichageHeureActuelle(void)
{
	Serial.println("Passage a l'etat AFFICHAGE_HEURE_ACTUELLE");
	m_IO.effacerEcran();
	etatActuel = AFFICHAGE_HEURE_ACTUELLE;
}

/// Méthode appelée à chaque cycle quand on est dans l'état "afficher heure actuelle".
void afficherHeureActuelle(void)
{
	m_IO.afficherHeure(hour(), minute(), second(), 0);
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure actuelle".
void entreeReglageHeureActuelle(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_ACTUELLE");
	m_IO.effacerEcran();
	etatActuel = REGLAGE_HEURE_ACTUELLE;
}

/// Méthode appelée lorsqu'on entre dans l'état "réglage heure réveil".
void entreeReglageHeureReveil(void)
{
	Serial.println("Passage a l'etat REGLAGE_HEURE_REVEIL");
	m_IO.effacerEcran();
	etatActuel = REGLAGE_HEURE_REVEIL;
}

/// Méthode appelée lorsqu'on entre dans l'état "distribution".
void entreeDistribution(void)
{
	Serial.println("Passage a l'etat DISTRIBUTION");
	m_IO.effacerEcran();
	m_IO.afficherMessage("Distribution", 0);
	etatActuel = DISTRIBUTION;
	alarme.setAlarmeActive(true);
}

void entreeFinDistribution(void)
{
	Serial.println("Passage a l'etat FIN_DISTRIBUTION");
	etatActuel = FIN_DISTRIBUTION;
}

void entreeNbAlarmes(void)
{
	Serial.println("Passage a l'etat set Nb alarmes");
	m_IO.effacerEcran();
	m_IO.afficherMessage(String("Nb alarmes : ") + String(alarme.getNbAlarmes()), 0);
	etatActuel = SET_NB_ALARMES;
}

void sortieFinDistribuer(void)
{
	Serial.println("Coupure moteur");
	alarme.setAlarmeActive(false);
}

void loop()
{
	// Rafraîchissement de l'état des boutons
	BPReglage.listen();
	BPHeure.listen();
	BPMinute.listen();
	BPPosMoteur.listen();

	switch (etatActuel)
	{
	case AFFICHAGE_HEURE_ACTUELLE:
		if (alarme.activerAlarme())// changement d'état
		{
			automate.transitionTo(etatDistribution);
			break;
		}
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatReglageHeureActuelle);

		break;

	case REGLAGE_HEURE_ACTUELLE:
		if (BPReglage.onPress())// changement d'état
			automate.transitionTo(etatNbAlarmes);

		break;

	case SET_NB_ALARMES:
		if (BPReglage.onPress())// changement d'état
		{
			// on change d'état si on a parcouru toutes les alarmes
			indAlarme = 0;
			if (alarme.getNbAlarmes() != 0)
				automate.transitionTo(etatReglageHeureReveil);
			else // cas où aucune alarme n'est active
				automate.transitionTo(etatAffichageHeureActuelle);
		}
		break;

	case REGLAGE_HEURE_REVEIL:
		if (BPReglage.onPress())// changement d'état
		{
			// on change d'état si on a parcouru toutes les alarmes
			if (++indAlarme >= alarme.getNbAlarmes())
				automate.transitionTo(etatAffichageHeureActuelle);

		}
		break;

	case DISTRIBUTION:
		if (BPPosMoteur.onPress())	//  on arrive à la position finale
		{
			automate.transitionTo(etatFinDistribution);
		}
		break;

	case FIN_DISTRIBUTION:
		if (BPPosMoteur.released())	//  position finale atteinte
		{
			automate.transitionTo(etatAffichageHeureActuelle);
		}
		break;

	default:
		Serial.println("Erreur dans la machine d'etats : etat inconnu!");
		break;
	}

	// traitement de l'état actuel
	automate.update();
	
	delay(50);
}

