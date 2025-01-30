#include "game_level.h"
#include "resource_manager.h"

GameLevel::GameLevel() {
	init(); // Di default si esegue GameLevel::init, per una disposizione di base
}

void GameLevel::Load() {
	//
}



void GameLevel::init() {
	//float unit_width = 1.0f; // in modo da unificare posizione e size, se modifico size modifico anche la posizione di conseguenza
	//float unit_height = 1.0f;

	// Definizione dei limiti della mappa
	map_limit_sx = -1.8f;
	map_limit_dx = 2.5f;
	map_limit_up = 2.0f;
	map_limit_down = -12.5f;

	glm::vec3 offset = glm::vec3(-3.0f, -12.0f, 0.0f);

	// Stanza
    Room = new GameObject(glm::vec3(2.3f, -8.0f, -2.0f), glm::vec3(0.0f,90.0f, 0.0f) ,glm::vec3(1.6f, 1.0f, 1.3f), ResourceManager::GetModel("room"));
	Room->SetShader(ResourceManager::GetShader("3d_mult_light"));
	
    // pavimento 3d
	Floor_3d = new GameObject(glm::vec3(2.0f, -8.0f, 1.0f), glm::vec3(90.0f,90.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.5f), ResourceManager::GetModel("floor"));
	Floor_3d->SetShader(ResourceManager::GetShader("3d_mult_light"));

	
	Wall_3d = new GameObject(glm::vec3(2.0f, -8.0f, 1.0f), glm::vec3(90.0f,90.0f, 0.0f), glm::vec3(2.0f, 1.0f, 1.5f), ResourceManager::GetModel("wall"));
	Wall_3d->SetShader(ResourceManager::GetShader("3d_mult_light"));


	// Generazione wc
	float wc_unit_width = 2.0f;
	float wc_unit_height = 2.0f;
	float z_offset = 1.0f; 

	// tutti gli oggetti si basano comunque su l'offset di base di pareti e pavimento
	glm::vec3 wc_offset = offset + glm::vec3(0.0f, 1.5f, z_offset); // pareti e pavimento sono a z = 0, questo deve essere sopra pavimento

	for (int i = 0; i < 4; i++) {
		glm::vec3 position = glm::vec3(wc_unit_width * 2.8f, wc_unit_height * i * 1.5f, 0.0f) + wc_offset;
		glm::vec3 size = glm::vec3(wc_unit_width, wc_unit_height, 1.0f);

		// wc 2d
		//GameObject wc(position, size, ResourceManager::GetTexture("wc")); // creo gameobject wc
		//wc.Rotation = glm::radians(90.0f);
		//wc.SetShader(ResourceManager::GetShader("base"));
		
		// wc 3d
		glm::vec3 wc_3d_pos(position.x - 1.0f, position.y, 2.0f);
		GameObject wc(wc_3d_pos, glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(1.0f), ResourceManager::GetModel("wc"));
		//wc.Rotation.z = glm::radians(0.0f);
        
		wc.SetShader(ResourceManager::GetShader("3d_mult_light"));

		this->toilets.push_back(Wc(wc)); // creo istanza Wc e li assegno il gameobject e lo metto nella lista di wc
	}

	// Luci
	// Posizione fissa delle luci decisa direttamente in fase di inizializzazione
	// Vettori paralleli per posizione e colori delle luci

	// PER CAMBIARE NUMERO DI LUCI MODIFICARE PARAMETRO PRESENTE NELLO SHADER!
	// MODIFICARLO POI PER SETTARLO CON UNO UNIFORM!!
	std::vector<glm::vec3> lightsPositions = {
		glm::vec3(-2.5f, -1.0f, 3.5f),
		glm::vec3(-2.5f, -5.0f, 3.5f),
		glm::vec3(-2.5f, -9.0f, 3.5f),
		glm::vec3(0.0f, -13.2f, 3.5f)
	};
	std::vector<glm::vec3> lightsColors = { // Colore per ora usato solo per dare un colore differente all'oggetto che rappresenta la luce, non cambia il colore della luce in se
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f)
	};
	
	int n_lights = static_cast<int>(lightsPositions.size()); // numero di luci nella scena

	for (int i = 0; i < n_lights; i++) {
		float z_rotation = -90.0f;
		if (i == n_lights - 1) {
			z_rotation = 0.0f;
		}
		GameObject newLight(lightsPositions[i], glm::vec3(-0.3f, 0.0f, z_rotation), glm::vec3(0.2f), ResourceManager::GetModel("lamp"), 0.0f, lightsColors[i]);
		newLight.SetShader(ResourceManager::GetShader("3d_mult_light")); // shader base
		this->lights.push_back(newLight);
	}

	// Mocio, per ricarica acqua
	glm::vec3 mocioPosition = glm::vec3(-1.5f, -12.0f, 2.0f);
	this->Mocio = new GameObject(mocioPosition, glm::vec3(90.0f, 0.0f, 0.0f), glm::vec3(0.4f), ResourceManager::GetModel("mocio"));

	this->Mocio->SetShader(ResourceManager::GetShader("3d_mult_light"));
}

void GameLevel::Draw(RenderData renderData) {

    if (Room != nullptr) {
        //Room->Draw(renderData);
    }

    if (Floor_3d != nullptr && Wall_3d != nullptr) {
        Floor_3d->Draw(renderData);
        Wall_3d->Draw(renderData);
    }

    if (this->toilets.size() > 0) {
        for (Wc& wc : this->toilets) {
            wc.wcObject.Draw(renderData);
        }
    }

    if (this->lights.size() > 0) {
        for (GameObject& l : lights) {
            l.Draw(renderData);
        }
    }

	for (GameObject& p : this->poopList) {
		if (!p.Destroyed) {
			p.Draw(renderData);
		}
	}

	this->Mocio->Draw(renderData);
}
