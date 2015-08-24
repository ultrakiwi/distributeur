/// \file Bouton.h
/// \author ultrakiwi
#ifndef BOUTON_H
#define BOUTON_H

#include <Arduino.h>

/// Classe très simple gérant un bouton poussoir connecté sur une entrée numérique.
class Bouton
{
public:
	Bouton(const int & pinBP, const bool & etatRepos = LOW);
	~Bouton(void) {};

	void listen(void)
	{
		m_etatPrecedent = m_etatActuel;
		m_etatActuel = digitalRead(m_pinBP);
	}

	bool pressed(void) const { return m_etatActuel == m_niveauAppui; }
	bool released(void) const { return !pressed(); }

	bool onPress(void) const { return (pressed() && (m_etatPrecedent != m_niveauAppui)); }
	bool onRelease(void) const { return (!pressed() && (m_etatPrecedent != m_niveauAppui)); }

private:
	const int m_pinBP;		///< broche sur laquelle est connecté le bouton
	bool m_niveauAppui;		///< état actif du bouton (ie quand il est enfoncé)
	bool m_etatActuel;		///< dernier état lu du bouton
	bool m_etatPrecedent;	///< état du bouton lors de la précédente lecture
};

Bouton::Bouton(const int & pinBP,
	const bool & niveauAppui)
	:
	m_pinBP(pinBP),
	m_niveauAppui(niveauAppui),
	m_etatActuel(!m_niveauAppui),
	m_etatPrecedent(m_etatActuel)
{
	pinMode(m_pinBP, INPUT);
}

#endif //BOUTON_H
