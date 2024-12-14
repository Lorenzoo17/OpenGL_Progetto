#include "wc.h"

Wc::Wc(GameObject o) {
	this->wcObject = o; // assegno gameobject associato
	this->available = true; // wc parte come disponibile
	this->isDirty = false;
}

// assegnare colore fisso di sporco magari o vari tipi
// poi creare metodo che richiama il player per pulire
void Wc::MakeDirty(glm::vec3 color = glm::vec3(0.5f, 0.4f, 0.3f)) { // richiamato in customer.cpp per sporcare il wc
	this->wcObject.Color = color;

	// set eventuale variabile isDirty 
	isDirty = true;
}

void Wc::Clean() {
	this->wcObject.Color = glm::vec3(1.0f);

	isDirty = false;
}