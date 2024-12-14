#include "game.h"
#include "resource_manager.h"
#include "game_object.h"
#include "Camera.h"
#include "utilities.h"
#include "customer.h"


void CleanWc(Wc* wc, float cleanDistance);

// METTERE I MODELLI IN RESOURCE MANAGER!! V
// GESTIONE MODELLI CON CUSTOM MANAGER?? V
// PASSARE CAMERA E POSIZIONE LUCE NEL RENDERING DEI GAMEOBJECT?? (PER CALCOLARE UNIFORM LI, O CREARE STRUCT APPOSITA TIPO)
// LUCI IN GAMELEVEL V
// AGGIUNGERE COLLIDER
// COME SPORCARE I WC 3D ?
// LUCI COME GAMEOBJECTS ? DI BASE NON NECESSARIO IN QUANTO CONTA SOLO LA POSIZIONE E POSSO NON VOLERLE RENDERIZZARE, PER ORA LASCIO COSI

Camera* camera;
glm::vec3 INITIAL_CAMERA_POSITION(0.0f, 0.0f, 5.0f);

GameObject* Player;
const glm::vec3 INITIAL_PLAYER_POSITION(0.0f, 0.0f, 3.0f);
const float PLAYER_INITIAL_VELOCITY(3.5f);
float angle = 0.0f;

bool interactPressed; // booleano che va a true se si preme E (test)

float timeBtw = 2.0f;
float wcDirtyRate = 2.0f;

// rendering della mappa tramite classe apposita GameLevel
// rendering di scatole e interazione in process input : quasi, migliorare 

Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height) {

};

void Game::Init(){
	// Caricamento degli shader
	ResourceManager::LoadShader("assets/shaders/shader.vs", "assets/shaders/shader.fs", nullptr, "base");
	ResourceManager::LoadShader("assets/shaders/shader_3d.vs", "assets/shaders/shader_3d.fs", nullptr, "base3d");
	ResourceManager::LoadShader("assets/shaders/shader_3d_simple_light.vs", "assets/shaders/shader_3d_simple_light.fs", nullptr, "base3d_light");
	ResourceManager::LoadShader("assets/shaders/shader_3d_mult_light.vs", "assets/shaders/shader_3d_mult_light.fs", nullptr, "3d_mult_light");
	
	// Caricamento dei modelli 3D
	ResourceManager::LoadModel("assets/models/robot/robot_1.obj", "robot1");
	ResourceManager::LoadModel("assets/models/slime/slime.obj", "slime1");
	ResourceManager::LoadModel("assets/models/wc/Gabinetto.obj", "wc");
	ResourceManager::LoadModel("assets/models/floor/Piastrelle.obj", "floor");
	ResourceManager::LoadModel("assets/models/walls/wall.obj", "wall");

	// Caricamento delle texture
	ResourceManager::LoadTexture("assets/textures/container.jpg", false, "cassa");
	ResourceManager::LoadTexture("assets/textures/robot.png", true, "robot");
	ResourceManager::LoadTexture("assets/textures/floor.png", true, "pavimento");
	ResourceManager::LoadTexture("assets/textures/wall.png", true, "parete");
	ResourceManager::LoadTexture("assets/textures/wc.png", true, "wc");
	ResourceManager::LoadTexture("assets/textures/slime.png", true, "slime");

	// Si definisce la matrice di proiezione 
	glm::mat4 projection = glm::mat4(1.0f);
	float cameraDistance = 5.0f;
	float aspect = (float)this->Width / this->Height;
	projection = glm::ortho(-aspect * cameraDistance, aspect * cameraDistance, -1.0f * cameraDistance, 1.0f * cameraDistance, 0.1f, 100.0f);
	// projection = glm::perspective(11.0f, (float)this->Width / (float)this->Height, 0.1f, 100.0f);

	// Si assegna la matrice di proiezione agli shader
	ResourceManager::GetShader("base").Use();
	ResourceManager::GetShader("base").SetMatrix4("projection", projection);
	ResourceManager::GetShader("base").SetInteger("texture1", 0); //poi da modificare
	ResourceManager::GetShader("base").SetInteger("texture2", 1); //poi da modificare

	ResourceManager::GetShader("base3d").Use();
	ResourceManager::GetShader("base3d").SetMatrix4("projection", projection);

	ResourceManager::GetShader("base3d_light").Use();
	ResourceManager::GetShader("base3d_light").SetMatrix4("projection", projection);

	ResourceManager::GetShader("3d_mult_light").Use();
	ResourceManager::GetShader("3d_mult_light").SetMatrix4("projection", projection);

	// Si creano i gameobject e si assegna lo shader corretto 
	// Creazione del player con texture robot, velocita 2.5 e direzione vec3(0)
	// Model test_model("resources/robot/robot_1.obj");
	Player = new GameObject(INITIAL_PLAYER_POSITION, glm::vec3(1.0f), ResourceManager::GetModel("robot1"), PLAYER_INITIAL_VELOCITY, glm::vec3(1.0f), glm::vec3(0.0f));
	Player->SetShader(ResourceManager::GetShader("3d_mult_light")); // Si assegna anche uno shader al player

	// Si inizializza il livello
	this->Level = new GameLevel();

	// Settaggio camera
	// Per ora si assegna la matrice di vista staticamente
	camera = new Camera(INITIAL_CAMERA_POSITION);

	// settaggio customer_manager
	float customerSpawnRate = 2.0f; // per ora fisso
	this->CustomerManager = new CustomersManager(customerSpawnRate);

	// Generazione customers statica
	// creare customer_manager dove viene fatto da li
	// se ce n'e' destroyed si riabilitano se no se ne spawnano nuovi -> si raggiunge regime dove non se ne spawnano piu

	// settaggio luce effettuato in gameLevel.cpp
	// Assegno posizione e colori delle luci (fisse)
	// luci renderizzate in game_level.cpp
	for (int i = 0; i < this->Level->lights.size(); i++) {
		renderData.lightPositions.push_back(this->Level->lights[i].Position);
		renderData.lightColors.push_back(this->Level->lights[i].Color);
	}
}

void Game::Render() {

	UpdateRenderData(); // aggiorno i dati di rendering da passare ai gameobject per il rendering

	// Necessario passare la matrice di vista per assegnarla prima della model
	Level->Draw(renderData); // renderizzo componenti statici del livello, li renderizzo per primi in quanto li voglio sotto tutto il resto

	for (Customer& c : this->CustomerManager->customers_list) {
		if (!c.customerObject.Destroyed)
			c.customerObject.Draw(renderData);
	}

	// Va renderizzato dopo per il blending (vedi glEnable(GL_BLEND) nel main), inoltre la posizione in z è 3.0f per averlo sopra tutto ma comunque sotto la camera (5.0f)
	Player->Draw(renderData); // Rendering del player, si attiva in automatico lo shader giusto e viene assegnata la texture corretta
}

void Game::Update(float deltaTime) {
	camera->CameraFollow(Player->Position, deltaTime); // Per camera che segue il player

	// aggiorno la matrice di vista e la assegno agli shader (NECESSARIO FARLO PRIMA DELLA MODEL, QUI NON VA BENE)
	// ResourceManager::GetShader("base").SetMatrix4("view", camera->GetViewMatrix());
	// ResourceManager::GetShader("base2").SetMatrix4("view", camera->GetViewMatrix());

	// TEST CUSTOMERS -> customer che vanno verso i wc

	for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
		for (Customer& c : this->CustomerManager->customers_list) { // per ogni cliente
			if (!c.customerObject.Destroyed) { // se il cliente non è destroyed
				if (c.currentState == CUSTOMER_WAIT) { // se è in wait (non ha ancora un wc associato)
					if (wc.available && !wc.isDirty) { // se il wc è disponibile e non è sporco
						c.targetWc = &wc; // si assegna a quel cliente il suddetto wc
						c.SetPath(wc.wcObject.Position); // si setta il path impostando come obiettivo la posizione di targetWc
						wc.available = false; // si mette il wc come occupato
					}
				}

				c.CustomerBehaviour(deltaTime); // si esegue la FSM per ogni cliente
			}
		}

		// Pulizia dei wc del player (Per ora gestito direttamente in questo script)
		CleanWc(&wc, 0.2f);
	}

	// spawn dei customer (PER ORA NON CONTROLLATO)
	this->CustomerManager->SpawnCustomers(deltaTime);
	
	DoCollisions();
}

void Game::ProcessInput(float deltaTime) {
	Player->MoveDirection = glm::vec3(0.0f); // resetto per evitare che continui a muoversi senza il mio input

	if (this->Keys[GLFW_KEY_W]) {
		Player->MoveDirection += glm::vec3(0.0f, 1.0f, 0.0f);
	}
	if (this->Keys[GLFW_KEY_S]) {
		Player->MoveDirection += glm::vec3(0.0f, -1.0f, 0.0f);
	}
	if (this->Keys[GLFW_KEY_A]) {
		Player->MoveDirection += glm::vec3(-1.0f, 0.0f, 0.0f);
	}
	if (this->Keys[GLFW_KEY_D]) {
		Player->MoveDirection += glm::vec3(1.0f, 0.0f, 0.0f);
	}

	if (glm::length(Player->MoveDirection) > 0.0f) {
		// Normalizzo la direzione
		Player->MoveDirection = Player->MoveDirection / sqrt(glm::dot(Player->MoveDirection, Player->MoveDirection));

		angle = atan2(Player->MoveDirection.x, -Player->MoveDirection.y); // si ottiene dove si sta dirigendo il player (angolo tra asse x e la direzione in cui si sta muovendo), in questo modo uso quest'angolo per ruotare attorno all'asse z
	}

	Player->Position += Player->MoveDirection * Player->Speed * deltaTime;
	Player->Rotation = angle; // PER MODELLI 3D SI FA ATTORNO AD ASSE Y! QUINDI ORA IN GAMEOBJECT QUESTE ROTAZIONI LE APPLICO AD ASSE Y

	interactPressed = this->Keys[GLFW_KEY_E];
}

void CleanWc(Wc* wc, float cleanDistance) {
	if (Utilities::CheckDistance(Player->Position, wc->wcObject.Position, 0.3f)) {
		if (interactPressed) {
			wc->Clean();
		}
	}
}

void Game::UpdateRenderData() {
	renderData.cameraPosition = camera->Position;
	renderData.viewMatrix = camera->GetViewMatrix(); // non da problemi metterla qui e non nei singoli come prima?
}

void Game::DoCollisions() {
	for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
		bool result = Utilities::CheckCollision((*Player), wc.wcObject);
		if (result) {
			// do something
		}
	}
}
