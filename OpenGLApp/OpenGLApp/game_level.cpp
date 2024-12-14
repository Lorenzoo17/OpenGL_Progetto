#include "game_level.h"
#include "resource_manager.h"

int level_shape[13][8] = {
		{1, 1, 1, 1, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 1, 1, 1, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 0, 0, 0, 0, 0, 0, 1},
		{1, 1, 0, 0, 1, 1, 1, 1}
};

GameLevel::GameLevel() {
	init(); // Di default si esegue GameLevel::init, per una disposizione di base
}

void GameLevel::Load(std::vector<GameObject*> w) {
	this->walls = w;
}

void GameLevel::Draw(RenderData renderData) {
	if (this->walls.size() > 0) {
		for (GameObject*& w : this->walls) {
			w->Draw(renderData); // Si richiama direttamente il metodo draw dei gameobject
		}
	}

	if (this->floor.size() > 0) {
		for (GameObject*& tale : this->floor) {
			tale->Draw(renderData);
		}
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
}

void GameLevel::init() {
	float unit_width = 1.0f; // in modo da unificare posizione e size, se modifico size modifico anche la posizione di conseguenza
	float unit_height = 1.0f;

	glm::vec3 offset = glm::vec3(-3.0f, -12.0f, 0.0f);

	// Generazione pareti e pavimento

	for (int i = 12; i >= 0; i--) {
		for (int j = 7; j >= 0; j--) {
			// calcolo posizione e size per ogni blocco
			glm::vec3 position = glm::vec3(unit_width * j, unit_height * i, 0.0f) + offset;
			glm::vec3 size = glm::vec3(unit_width, unit_height, 1.0f);

			if (level_shape[i][j] == 1) { // parete
				GameObject *brick = new GameObject(position, size, ResourceManager::GetTexture("parete"));
				brick->Rotation = glm::radians(0.0f);
				brick->SetShader(ResourceManager::GetShader("base"));
				this->walls.push_back(brick);
			}
			else if (level_shape[i][j] == 0) { // pavimento
				GameObject *tale = new GameObject(position, size, ResourceManager::GetTexture("pavimento"));
				tale->Rotation = glm::radians(0.0f);
				tale->SetShader(ResourceManager::GetShader("base"));
				this->floor.push_back(tale);
			}
		}
	}

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
		GameObject wc(position, size, ResourceManager::GetTexture("wc")); // creo gameobject wc
		//wc 3d
		//GameObject wc(position, glm::vec3(1.0f), ResourceManager::GetModel("wc"));

		wc.Rotation = glm::radians(90.0f);
		wc.SetShader(ResourceManager::GetShader("base"));
		//wc.SetShader(ResourceManager::GetShader("3d_mult_light"));
		this->toilets.push_back(Wc(wc)); // creo istanza Wc e li assegno il gameobject e lo metto nella lista di wc
	}

	// Luci
	// Posizione fissa delle luci decisa direttamente in fase di inizializzazione
	// Vettori paralleli per posizione e colori delle luci
	std::vector<glm::vec3> lightsPositions = {
		glm::vec3(-2.0f, -1.0f, 5.0f),
		glm::vec3(-2.0f, -5.0f, 5.0f),
		glm::vec3(-2.0f, -9.0f, 5.0f)
	};
	std::vector<glm::vec3> lightsColors = {
		glm::vec3(1.0f),
		glm::vec3(1.0f),
		glm::vec3(1.0f)
	};
	
	int n_lights = static_cast<int>(lightsPositions.size()); // numero di luci nella scena

	for (int i = 0; i < n_lights; i++) {
		// Per ora metto come texture slime, volendo pero si possono anche non renderizzare, tanto conta solo la posizione
		GameObject newLight(lightsPositions[i], glm::vec3(1.0f), ResourceManager::GetTexture("slime"), 0.0f, lightsColors[i]);
		newLight.SetShader(ResourceManager::GetShader("base")); // shader base

		this->lights.push_back(newLight);
	}
}
