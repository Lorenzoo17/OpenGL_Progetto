#include "game.h"
#include "resource_manager.h"
#include "game_object.h"
#include "Camera.h"
#include "utilities.h"
#include "customer.h"
#include "time.h"
#include "Player.h"

//#include "text_renderer.h"



void CleanWc(Wc* wc, float cleanDistance);

// METTERE I MODELLI IN RESOURCE MANAGER!! V
// GESTIONE MODELLI CON CUSTOM MANAGER?? V
// PASSARE CAMERA E POSIZIONE LUCE NEL RENDERING DEI GAMEOBJECT?? (PER CALCOLARE UNIFORM LI, O CREARE STRUCT APPOSITA TIPO)
// LUCI IN GAMELEVEL V
// AGGIUNGERE COLLIDER
// COME SPORCARE I WC 3D ?
// LUCI COME GAMEOBJECTS ? DI BASE NON NECESSARIO IN QUANTO CONTA SOLO LA POSIZIONE E POSSO NON VOLERLE RENDERIZZARE, PER ORA LASCIO COSI

Camera* camera;
glm::vec3 INITIAL_CAMERA_POSITION(0.0f, 0.0f, 8.0f);

Player* player;
GameObject* menuTexture; // gameObject contenente la texture del menu
GameObject* startButton;

//GameObject* Player;
const glm::vec3 INITIAL_PLAYER_POSITION(0.0f, 0.0f, 3.0f);
const float PLAYER_INITIAL_VELOCITY(3.5f);
float angle = 0.0f;

//TextRenderer* Text;

bool interactPressed; // booleano che va a true se si preme E (test)

float timeBtw = 2.0f;
float wcDirtyRate = 2.0f;

float game_score = 0.0f; // score momentaneo del gioco
glm::mat4 projection;

// rendering della mappa tramite classe apposita GameLevel
// rendering di scatole e interazione in process input : quasi, migliorare 

Game::Game(unsigned int width, unsigned int height) : Width(width), Height(height), isGameOver(false) {
    isInMenu = true; // si parte con il menu attivo
};

void Game::Init(){
    ResourceManager::LoadAssets();
    
	// Si definisce la matrice di proiezione
	projection = glm::mat4(1.0f);
	float cameraDistance = 5.0f;
	float aspect = (float)this->Width / this->Height;
	//projection = glm::ortho(-aspect * cameraDistance, aspect * cameraDistance, -1.0f * cameraDistance, 1.0f * cameraDistance, 0.1f, 100.0f);
	projection = glm::perspective(11.0f,  (float)this->Width / (float)this->Height, 0.1f, 100.0f);

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
    // Si inizializza il livello
    this->Level = new GameLevel();
    
    player = new Player(INITIAL_PLAYER_POSITION, glm::vec3(70.0f, 0.0f, 0.0f), glm::vec3(1.0f), ResourceManager::GetModel("robot1"), PLAYER_INITIAL_VELOCITY, glm::vec3(1.0f), glm::vec3(0.0f), 10, this);
	player->SetShader(ResourceManager::GetShader("3d_mult_light")); // Si assegna anche uno shader al player

    glm::vec3 menuTexturePosition = glm::vec3(0.0f, 1.0f, 7.0f);
    glm::vec3 menuTextureSize = glm::vec3(3.8f, -3.5f, 0.0f);
    menuTexture = new GameObject(menuTexturePosition, menuTextureSize, ResourceManager::GetTexture("menuTexture"));
    menuTexture->SetShader(ResourceManager::GetShader("base"));
    menuTexture->Rotation = glm::vec3(50.0f, 0.0f, 0.0f);
    menuTexture->Color = glm::vec3(0.7f, 0.7f, 0.7f);

    glm::vec3 startButtonPosition = glm::vec3(0.0f, 0.0f, 7.0f);
    glm::vec3 startButtonSize = glm::vec3(1.0f, 1.0f, 1.0f);
    startButton = new GameObject(startButtonPosition, startButtonSize, ResourceManager::GetTexture("circleButton"));
    startButton->SetShader(ResourceManager::GetShader("base"));
    startButton->Rotation = glm::vec3(50.0f, 0.0f, 0.0f);
    startButton->Color = glm::vec3(0.2f, 0.2f, 0.2f);

	// Settaggio camera
	// Per ora si assegna la matrice di vista staticamente
	camera = new Camera(INITIAL_CAMERA_POSITION);
    

	// settaggio customer_manager
	float customerSpawnRate = 2.0f; // per ora fisso
    this->CustomerManager = new CustomersManager(customerSpawnRate, this);
    

	// Generazione customers statica
	// creare customer_manager dove viene fatto da li
	// se ce n'e' destroyed si riabilitano se no se ne spawnano nuovi -> si raggiunge regime dove non se ne spawnano piu

	// settaggio luce effettuato in gameLevel.cpp
	// Assegno posizione e colori delle luci (fisse)a
	// luci renderizzate in game_level.cpp
	for (int i = 0; i < this->Level->lights.size(); i++) {
		renderData.lightPositions.push_back(this->Level->lights[i].Position);
		renderData.lightColors.push_back(this->Level->lights[i].Color);
	}

	// Inizializzazione testo
	//Text = new TextRenderer(this->Width, this->Height);
	//Text->Load("assets/fonts/Roboto/Roboto-Regular.ttf", 24);
}

void Game::Render() {

    if (!isInMenu && !isGameOver) { // Se non si è nel menu, renderizza il gioco
        UpdateRenderData(); // aggiorno i dati di rendering da passare ai gameobject per il rendering

        // Necessario passare la matrice di vista per assegnarla prima della model
        Level->Draw(renderData); // renderizzo componenti statici del livello, li renderizzo per primi in quanto li voglio sotto tutto il resto


        for (Customer& c : this->CustomerManager->customers_list) {
            if (!c.customerObject.Destroyed)
                c.customerObject.Draw(renderData);
        }


        // Va renderizzato dopo per il blending (vedi glEnable(GL_BLEND) nel main), inoltre la posizione in z è 3.0f per averlo sopra tutto ma comunque sotto la camera (5.0f)
        player->Draw(renderData); // Rendering del player, si attiva in automatico lo shader giusto e viene assegnata la texture corretta

        //std::stringstream ss; ss << this->CustomerManager->customers_list.size();
        //Text->RenderText("Customers:" + ss.str(), 5.0f, 1.0f, 1.7f);
        std::stringstream ss; ss << game_score;
        //Text->RenderText("Score:" + ss.str(), 5.0f, 2.0f, 1.7f);

        // Text rendere del water level
        std::stringstream swater;
        swater << player->getWaterLevel();
       // Text->RenderText("Water:" + swater.str(), 600.0f, 2.0f, 1.7f);
    }
    else if (!isGameOver) { // Se si e' nel menu
        UpdateRenderData(); // aggiorno i dati di rendering da passare ai gameobject per il rendering
        menuTexture->Draw(renderData);
        startButton->Draw(renderData);

        //Text->RenderText("WC CLEANER", 250.0f, 150.0f, 2.0f, glm::vec3(0.7f, 0.2f, 0.2f));

        //Text->RenderText("PREMI INVIO O IL BOTTONE PER GIOCARE", 150.0f, 350.0f, 1.0f);
    }
    else if (isGameOver) {
        int record = ResourceManager::saveHighScore(this->game_score);
        //Text->RenderText("GAME OVER", 250.0f, 200.0f, 2.0f, glm::vec3(0.7f, 0.2f, 0.2f));

        std::stringstream score; 
        score << game_score;
       // Text->RenderText("NEW SCORE: " + score.str(), 280.0f, 300.0f, 1.0f);

        std::stringstream ssrecord;
        ssrecord << record;
        //Text->RenderText("RECORD: " + ssrecord.str(), 280.0f, 350.0f, 1.0f);
    }
}

void Game::Update() {
    if (!isGameOver && !isInMenu){
        ProcessInput();
        camera->CameraFollow(player->Position); // Per camera che segue il player
        //player->Idle(INITIAL_PLAYER_POSITION.z);
        // aggiorno la matrice di vista e la assegno agli shader (NECESSARIO FARLO PRIMA DELLA MODEL, QUI NON VA BENE)
        // ResourceManager::GetShader("base").SetMatrix4("view", camera->GetViewMatrix());
        // ResourceManager::GetShader("base2").SetMatrix4("view", camera->GetViewMatrix());
        
        // spawn dei customer (PER ORA NON CONTROLLATO)
        this->CustomerManager->SpawnCustomers();
        this->CustomerManager->updateBehaviour();
        
        // Pulizia dei wc del player
        player->clean(interactPressed);
        player->upadateStreak();
        player->CheckPoop(); // si controlla interazione con poops
        // DoCollisions(); //togli

        
        //printf("%f\n" ,sqrt(pow((Level->Mocio->Position.x - player->Position.x), 2) + pow((Level->Mocio->Position.y - player->Position.y), 2)));
        // glm::vec2 posizioneSchermowc = Utilities::worldToScreen(this->Level->toilets[0].wcObject.Position, camera->GetViewMatrix(), projection, Width, Height);
        // if (Utilities::CheckDistance(glm::vec3(mousePosition.x, mousePosition.y, 0.0f), glm::vec3(posizioneSchermowc.x, posizioneSchermowc.y, 0.0f), 100.0f)) {
        // }
    }else
    {
        if (isInMenu) {
            if (this->Keys[GLFW_KEY_ENTER]) {
                isInMenu = false; // Esce dal menu
            }
            
            glm::vec3 button_offset = glm::vec3(startButton->Position.x, startButton->Position.y + 2.0f, 0.0f);
            if (Utilities::clickOverObject(button_offset, mousePosition, camera->GetViewMatrix(), projection, Width, Height, 100.0f)) {
                startButton->Color = glm::vec3(0.2f, 0.8f, 0.2f);
                if (mouseLeftClick) {
                    isInMenu = false;
                }
            }
            else {
                startButton->Color = glm::vec3(0.2f, 0.2f, 0.2f);
            }
        }
        else if (isGameOver) {
            // std::cout << "Game Over" << std::endl;
            //high score
        }
    }
}

void Game::ProcessInput() {
    double deltaTime = Time::deltaTime;
    player->MoveDirection = glm::vec3(0.0f); // resetto per evitare che continui a muoversi senza il mio input
    glm::vec2 dir = glm::vec2(0.0f);
    
    if (this->Keys[GLFW_KEY_W]) {
        if(player->Position.y <= this->Level->map_limit_up)
            dir.y = 1.0f;//player->Move(glm::vec3(0.0f, 1.0f, 0.0f),deltaTime);
        
    }
    if (this->Keys[GLFW_KEY_S]) {
        // if(Player->Position.y > -2.0f) // es. definizione limiti mappa
        if (player->Position.y >= this->Level->map_limit_down)
            dir.y = -1.0f; //player->Move(glm::vec3(0.0f, -1.0f, 0.0f),deltaTime);
    }
    if (this->Keys[GLFW_KEY_A]) {
        if (player->Position.x >= this->Level->map_limit_sx)
            dir.x = -1.0f;//player->Move(glm::vec3(-1.0f, 0.0f, 0.0f),deltaTime);
    }
    if (this->Keys[GLFW_KEY_D]) {
        if (player->Position.x <= this->Level->map_limit_dx)
            dir.x = 1.0f;//player->Move(glm::vec3(1.0f, 0.0f, 0.0f),deltaTime);
    }
    player->Move(glm::vec3(dir.x, dir.y, 0.0f),deltaTime);
    interactPressed = this->Keys[GLFW_KEY_E];

     
    if (Utilities::clickOverObject(this->Level->Mocio->Position, mousePosition, camera->GetViewMatrix(), projection, this->Width, this->Height, 50.0f)) {
        if (mouseLeftClick) {
            player->waterRefill();
        }
        this->Level->Mocio->Color = glm::vec3(0.2f, 1.0f, 0.2f);
    }
    else {
        this->Level->Mocio->Color = glm::vec3(1.0f, 1.0f, 1.0f);
    }
}

void Game::DoCollisions() {
    
    //if (glm::length(player->MoveDirection) == 0) // calcolo le collisioni solo se il player si sta muovendo
      //  return;

    for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
        Collision result = Utilities::CheckCollision((*player), wc.wcObject);

        if (std::get<0>(result)) {
            Direction dir = std::get<1>(result);
            glm::vec3 diff_vector = std::get<2>(result);

            // Calcola la penetrazione
            float penetrationX = wc.wcObject.Size.x / 2.0f + player->Size.x / 2.0f - std::abs(diff_vector.x);
            float penetrationY = wc.wcObject.Size.y / 2.0f + player->Size.y / 2.0f - std::abs(diff_vector.y);

            if (dir == DIR_LEFT && player->MoveDirection.x > 0)
                player->Position.x -= penetrationX; // Sposta il giocatore a sinistra
            else if (dir == DIR_RIGHT && player->MoveDirection.x < 0)
                player->Position.x += penetrationX; // Sposta il giocatore a destra

            if (dir == DIR_UP && player->MoveDirection.y < 0) {
                player->Position.y += penetrationY; // Sposta il giocatore in alto
            }
            else if (dir == DIR_DOWN && player->MoveDirection.y > 0) {
                player->Position.y -= penetrationY; // Sposta il giocatore in basso
            }
        
        }
    }
}

void Game::UpdateRenderData() {
	renderData.cameraPosition = camera->Position;
	renderData.viewMatrix = camera->GetViewMatrix(); // non da problemi metterla qui e non nei singoli come prima?
}

void Game::updateProjection(float width, float height)
{
    projection = glm::mat4(1.0f);
    float cameraDistance = 5.0f;
    float aspect = (float)this->Width / this->Height;
    //projection = glm::ortho(-aspect * cameraDistance, aspect * cameraDistance, -1.0f * cameraDistance, 1.0f * cameraDistance, 0.1f, 100.0f);
    projection = glm::perspective(11.0f,  (float)this->Width / (float)this->Height, 0.1f, 100.0f);

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
    
}
