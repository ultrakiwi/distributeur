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

#include <StandardCplusplus.h>
#include "Macho.h"

#include <FiniteStateMachine.h>
#include <LiquidCrystal.h>
#include <Time.h>

#include "Bouton.h"    // gère un bouton poussoir
#include "InterfaceIO.h"
#include "AlarmeMgr.h" // gère l'alarme

// à ranger!
const bool etatAppui = HIGH;  // état d'un bouton quand on appuie dessus
int indAlarme(0); // à transformer en variable de l'état Reglage
Bouton BPReglage(PIN_BP_MENU, etatAppui);
Bouton BPHeure(PIN_BP_H, etatAppui);
Bouton BPMinute(PIN_BP_MIN, etatAppui);
Bouton BPPosMoteur(PIN_CAPTEUR_POS_MOTEUR, etatAppui);

InterfaceIO m_IO;
// initialisation du gestionnaire d'alarme
AlarmeMgr alarme;

namespace DistributeurNS
{
	TOPSTATE(Top)
	{
		STATE(Top)

			// Machine's event protocol
			virtual void eventBPMenu() {}
		virtual void eventBPHeure() {}
		virtual void eventBPMinute() {}
		virtual void eventUpdate() {}
		virtual void eventBPPosMoteurAppui() {}
		virtual void eventBPPosMoteurRelachement() {}

	private:
		void entry() {}
		void init();
	};

	// Déclaration de l'état StateAffichageHeureActuelle
	SUBSTATE(StateAffichageHeureActuelle, Top) {

		STATE(StateAffichageHeureActuelle)

			// Event handler
			void eventBPMenu();
		void eventUpdate();

private:
	void entry();
	void exit()
	{
		Serial.println(F("exit StateAffichageHeureActuelle"));
	}
	};

	// Déclaration de l'état StateReglage
	SUBSTATE(StateReglage, Top)
	{

		STATE(StateReglage)

			// Event handler

	private:
		void entry() { Serial.println(F("enter StateReglage")); }
		void exit() { Serial.println(F("exit StateReglage")); }
		void init();
	};

	// Déclaration de l'état StateReglageHeureActuelle
	SUBSTATE(StateReglageHeureActuelle, StateReglage)
	{

		STATE(StateReglageHeureActuelle)

			// Event handler
			void eventBPMenu();
		void eventBPHeure();
		void eventBPMinute();

	private:
		void entry() {
			Serial.println(F("StateReglageHeureActuelle"));
			m_IO.effacerEcran();

			tmElements_t heureDecomposee;
			breakTime(now(), heureDecomposee);
			m_IO.afficherMessage(F("reglage horloge"), 0);
			m_IO.afficherHeure(hour(), minute(), second(), 1);
		}
		void exit() {/* Serial.println(F("exit StateReglageHeureActuelle"));*/ }
	};

	// Déclaration de l'état StateReglageHeureAlarme
	SUBSTATE(StateReglageHeureAlarme, StateReglage)
	{

		STATE(StateReglageHeureAlarme)

			// Event handler
			void eventBPMenu();
		void eventBPHeure();
		void eventBPMinute();

	private:
		void entry()
		{
			Serial.println(F("StateReglageHeureAlarme"));
			m_IO.effacerEcran();
			m_IO.afficherMessage(String("alarme  ") + String(indAlarme + 1) + " :", 0);
			// on scinde le temps en champs exploitables
			tmElements_t heureDecomposee;
			breakTime(alarme.getHeureAlarme(indAlarme), heureDecomposee);
			m_IO.afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
		}
		void exit() { /*Serial.println(F("exit StateReglageHeureAlarme"));*/ }
	};

	// Déclaration de l'état StateNbAlarmes
	SUBSTATE(StateNbAlarmes, StateReglage)
	{

		STATE(StateNbAlarmes)

			// Event handler
			void eventBPMenu();
		virtual void eventBPHeure();
		virtual void eventBPMinute();

	private:
		void entry()
		{
			Serial.println(F("StateNbAlarmes"));
			m_IO.effacerEcran();
			m_IO.afficherMessage(String("Nb alarmes : ") + String(alarme.getNbAlarmes()), 0);
		}
		void exit() { /*Serial.println(F("exit StateNbAlarmes"));*/ }
	};

	// Déclaration de l'état Distribution
	SUBSTATE(Distribution, Top)
	{

		STATE(Distribution)

			// Event handler

	private:
		void entry()
		{
			Serial.println(F("Distribution"));
			m_IO.effacerEcran();
			m_IO.afficherMessage(F("Distribution"), 0);
			alarme.setAlarmeActive(true);
		}
		void exit() {}

		void eventBPPosMoteurAppui();
	};

	// Déclaration de l'état FinDistribution
	SUBSTATE(FinDistribution, Top)
	{

		STATE(FinDistribution)

			// Event handler

	private:
		void entry() { Serial.println(F("FinDistribution")); }
		void exit() 
		{
			//Serial.println(F("Coupure moteur"));
			alarme.setAlarmeActive(false);
		}
		void eventBPPosMoteurRelachement();
	};


	// définition des méthodes de Top
	void Top::init()
	{
		setState<StateAffichageHeureActuelle>();
	}


	// définition des méthodes de StateReglage
	void StateReglage::init()
	{
		setState<StateReglageHeureActuelle>();
	}

	// définition des méthodes de StateAffichageHeureActuelle
	void StateAffichageHeureActuelle::entry()
	{
		Serial.println(F("StateAffichageHeureActuelle"));
		m_IO.effacerEcran();
	}

	void StateAffichageHeureActuelle::eventBPMenu()
	{
		setState<StateReglage>();
	}

	/// Méthode appelée à chaque cycle quand on est dans l'état "afficher heure actuelle".
	void StateAffichageHeureActuelle::eventUpdate()
	{
		if (alarme.activerAlarme())// changement d'état
		{
			setState<Distribution>();
			return;
		}

		m_IO.afficherHeure(hour(), minute(), second(), 0);
	}

	// définition des méthodes de StateReglageHeureActuelle
	void StateReglageHeureActuelle::eventBPMenu()
	{
		setState<StateNbAlarmes>();
	}

	void StateReglageHeureActuelle::eventBPHeure()
	{
		// on scinde le temps en champs exploitables
		tmElements_t heureDecomposee;
		breakTime(now(), heureDecomposee);
		incrementerHeure(heureDecomposee.Hour);
		setTime(makeTime(heureDecomposee));
		m_IO.afficherMessage(F("reglage horloge"), 0);
		m_IO.afficherHeure(hour(), minute(), second(), 1);
	}

	void StateReglageHeureActuelle::eventBPMinute()
	{
		// on scinde le temps en champs exploitables
		tmElements_t heureDecomposee;
		breakTime(now(), heureDecomposee);
		incrementerMinutes(heureDecomposee.Minute);
		setTime(makeTime(heureDecomposee));
		m_IO.afficherMessage(F("reglage horloge"), 0);
		m_IO.afficherHeure(hour(), minute(), second(), 1);
	}

	// définition des méthodes de StateNbAlarmes
	void StateNbAlarmes::eventBPMenu()
	{
		indAlarme = 0;

		if (alarme.getNbAlarmes() != 0)
			setState<StateReglageHeureAlarme>();
		else // cas où aucune alarme n'est active
			setState<StateAffichageHeureActuelle>();
	}

	void StateNbAlarmes::eventBPHeure()
	{
		alarme.setNbAlarmes(alarme.getNbAlarmes() - 1);
		m_IO.afficherMessage(String(F("Nb alarmes : ")) + String(alarme.getNbAlarmes()), 0);
	}

	void StateNbAlarmes::eventBPMinute()
	{
		alarme.setNbAlarmes(alarme.getNbAlarmes() + 1);
		m_IO.afficherMessage(String(F("Nb alarmes : ")) + String(alarme.getNbAlarmes()), 0);
	}


	// Event handler
	void StateReglageHeureAlarme::eventBPMenu()
	{
		// on a parcouru toutes les alarmes
		if (++indAlarme >= alarme.getNbAlarmes())
			setState<StateAffichageHeureActuelle>();
		else // passage à l'alarme suivante
			setState<StateReglageHeureAlarme>();
	}
	void StateReglageHeureAlarme::eventBPHeure()
	{
		tmElements_t heureDecomposee;
		breakTime(alarme.getHeureAlarme(indAlarme), heureDecomposee);
		incrementerHeure(heureDecomposee.Hour);
		alarme.setHeureAlarme(indAlarme, makeTime(heureDecomposee));
		m_IO.afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
	}

	void StateReglageHeureAlarme::eventBPMinute()
	{
		tmElements_t heureDecomposee;
		breakTime(alarme.getHeureAlarme(indAlarme), heureDecomposee);
		incrementerMinutes(heureDecomposee.Minute);
		alarme.setHeureAlarme(indAlarme, makeTime(heureDecomposee));
		m_IO.afficherHeure(heureDecomposee.Hour, heureDecomposee.Minute, heureDecomposee.Second, 1);
	}

	void Distribution::eventBPPosMoteurAppui() { setState<FinDistribution>(); }

	void FinDistribution::eventBPPosMoteurRelachement() { setState<StateAffichageHeureActuelle>(); }
}

/// Initialisation du module.
void setup()
{
	// initialisation de la liaison série (pour le debug)
	Serial.begin(9600);
	//delay(500);
	Serial.println(F("InterfaceIO : init OK"));
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

void reveiller(void)
{
	m_IO.afficherHeure(hour(), minute(), second(), 0);
}


void couperReveil(void)
{
	alarme.setAlarmeActive(false);
}

void loop()
{
	// permet d'avoir une variable globale initialis�e APRES la liaison s�rie
	// sinon perturbe la liaison, et rien ne fonctionne
	static Macho::Machine<DistributeurNS::Top> m;

	// Rafraîchissement de l'état des boutons
	BPReglage.listen();
	BPHeure.listen();
	BPMinute.listen();
	BPPosMoteur.listen();

	// déclenchement des évènements
	if (BPReglage.onPress())
		m->eventBPMenu();

	if (BPHeure.onPress())
		m->eventBPHeure();

	if (BPMinute.onPress())
		m->eventBPMinute();

	if (BPPosMoteur.onPress())
		m->eventBPPosMoteurAppui();

	if (BPPosMoteur.onRelease())
		m->eventBPPosMoteurRelachement();

	// mise à jour de l'action de l'état courant
	m->eventUpdate();

	// on souffle un peu
	delay(50);
}
