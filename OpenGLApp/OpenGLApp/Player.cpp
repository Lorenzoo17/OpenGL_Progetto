#define _USE_MATH_DEFINES
#include "Player.h"
#include "utilities.h"
#include <cmath>
#include "time.h"


Player::Player(glm::vec3 pos, glm::vec3 rotation, glm::vec3 size, Model object_model, float speed, glm::vec3 color,
               glm::vec3 moveDirection , int MaxWater,    Game* game):
GameObject(pos, rotation,  size,  object_model, speed ,  color, moveDirection), MaxWaterLevel(MaxWater), WaterLevel(MaxWater), game(game), Level(game->Level),speedReached(0.0f), canMove(true), initialStreakTime(6), streakTime(6), streak(0){}



float waitTime = 0.6f;
bool speedMode = false;


void Player::Idle(float initialPos)
{
    float speed = 1.0f;
    float amplitude = 1/2;
    this->Position.z =  initialPos + (amplitude * sin(Time::currentTime  * speed/5 )) ;
    std::cout << this->Position.z << std::endl;
}

void Player::Move(glm::vec3 direction, float deltaTime)
{
    static float angle;
    static float counter = waitTime;

    
    if(canMove)
    {
        if (glm::length(direction) > 0.0f) {
            // Normalizzo la direzione
            direction = direction / sqrt(glm::dot(direction, direction));

            angle = atan2(direction.x, -direction.y); // si ottiene dove si sta dirigendo il player (angolo tra asse x e la direzione in cui si sta muovendo), in questo modo uso quest'angolo per ruotare attorno all'asse z
            angle = angle * 180.0 / M_PI; // Conversione da radianti a gradi

        }
        
        glm::vec3 desiredPosition = this->Position + direction * this->Speed * deltaTime;

        this->Position = desiredPosition;
        this->Rotation.y = angle;
        //this->SetRotation( glm::vec3(this->Rotation.x, this->Rotation.y, this->Rotation.z)); // PER MODELLI 3D SI FA ATTORNO AD ASSE Y! QUINDI ORA IN GAMEOBJECT QUESTE ROTAZIONI LE APPLICO AD ASSE Y

        Player::collision();
    }
    else
    {
        //disiabilita il movimento del player per un breve periodo per poi riabilitarlo
        counter -= Time::deltaTime;
        if (counter <= 0)
        {
            counter = waitTime;
            canMove = true;
        }
            
    }
}

void Player::collision() {

    for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
        Collision result = Utilities::CheckCollision((*this), wc.wcObject);
        
        if (std::get<0>(result)) {
            Direction dir = std::get<1>(result);
            glm::vec3 diff_vector = std::get<2>(result);

            // Calcola la penetrazione
            float penetrationX = wc.wcObject.Size.x / 2.0f + this->Size.x / 2.0f - std::abs(diff_vector.x);
            float penetrationY = wc.wcObject.Size.y / 2.0f + this->Size.y / 2.0f - std::abs(diff_vector.y);

            if (dir == DIR_LEFT && this->MoveDirection.x > 0)
                this->Position.x -= penetrationX; // Sposta il giocatore a sinistra
            else if (dir == DIR_RIGHT && this->MoveDirection.x < 0)
                this->Position.x += penetrationX; // Sposta il giocatore a destra

            if (dir == DIR_UP && this->MoveDirection.y < 0) {
                this->Position.y += penetrationY; // Sposta il giocatore in alto
            }
            else if (dir == DIR_DOWN && this->MoveDirection.y > 0) {
                this->Position.y -= penetrationY; // Sposta il giocatore in basso
            }
        
        }
    }
}

int Player::getWaterLevel() {
    return this->WaterLevel;
}

void Player::CleanWc(Wc* wc, bool interactPressed) {
    if (Utilities::CheckDistance(this->Position, wc->wcObject.Position, 1.2f)) {
        if (interactPressed) {
            if (wc->isDirty) {
                Utilities::PlaySound("wipe_fast");
                //game_score += 10.0f;
                this->game->game_score +=10.0f;
                this->WaterLevel -= 1;
                this->game->CustomerManager->customerToServe--;
                streak++;   //incrementa la streak
                streakTime += 3/4 * initialStreakTime; //da un piccolo vantaggio di tempo
            }
            wc->Clean();
            canMove = false;
        }
    }
}

void Player::clean( bool interactPressed)
{
    std::cout << WaterLevel << std::endl;
    for (Wc& wc : this->Level->toilets) { // per ogni wc nella scena
        if(wc.isDirty)
            if(WaterLevel > 0) // non si possono pulire i bagni senza acquas
                CleanWc(&wc, interactPressed);
    }
    //waterRefill();
}

void Player::CheckPoop() { // Eseguito in Update
    
    if (!poopMalus && !speedMode) {
        float interactPoopDistance = 1.0f;
        float stepOnPoopDistance = 0.3f;

        auto& poops = this->game->Level->poopList;
        int i = 0;

        while (i < poops.size()) {
            bool stepOn = Utilities::CheckDistance(poops[i].Position, this->Position, stepOnPoopDistance);
            bool interact = Utilities::CheckDistance(poops[i].Position, this->Position, interactPoopDistance);

            if (stepOn || (interact && this->game->Keys[GLFW_KEY_E])) { // se calpesto o mi avvicino molto premendo E
                poops[i].Destroyed = true;
                poops[i].Destroy();

                if (stepOn) {
                    poopMalus = true;
                    poopMalusCurrentTime = poopMalusTime;
                    this->Speed -= poopMalusModifier; // decremento velocita'
                }
                else {
                    this->game->game_score += 5;
                }
                // scambio elemento con ultimo e faccio per rimuoverlo
                if (i != poops.size() - 1) {
                    std::swap(poops[i], poops.back());
                }
                poops.pop_back();
            }
            else {
                // se sono a distanza per poter interagire coloro per avvisare del pericolo, se mi allontano torna a colore iniziale
                poops[i].Color = interact ? glm::vec3(1.0f, 0.0f, 0.0f) : glm::vec3(1.0f, 1.0f, 1.0f);
                i++;
            }
        }
    }

    // Se il malus di velocita' è attivo la velocita' viene ripristinata dopo poopMalusTime secondi
    if (poopMalus) {
        if (poopMalusCurrentTime <= 0) {
            poopMalus = false;
            this->Speed += poopMalusModifier;
            poopMalusCurrentTime = poopMalusTime;
        }
        else {
            poopMalusCurrentTime -= Time::deltaTime;
        }
    }
}

void Player::upadateStreak()
{
    
    static float initialSpeedModetime = 4;
    static float speedModetime = 4;
    
    
    static float oldspeed = this->Speed;
    
    // printf("Streak : %d\n", streak);
    
    if(!speedMode){
        if (streak >= 1) //lavora solo se il player ha pulito almeno un gabinetto
        {
            streakTime -= Time::deltaTime;
            if(streakTime <= 0)
            {
                streak = 0; //resetta la streak
                streakTime = initialStreakTime; //resetta il timer
            }else
            {   if (streak >= 4)
            {
                
                streak = 0; //resetta la streak
                speedMode = true;
                this->Color = glm::vec3(0.6f, 0.2f , 0.1f);
                speedModetime = initialSpeedModetime;
                oldspeed = this->Speed;
                this->Speed = 5;
                this->WaterLevel = (WaterLevel < MaxWaterLevel) ? WaterLevel + 2: WaterLevel;
                
            }
            }
        }
    }else{
        speedModetime -= Time::deltaTime;
        if(speedModetime <= 0)
        {
            this->Color = glm::vec3(1.0f); //ripristina la velocità del player
            speedModetime = initialSpeedModetime; //resetta il timer
            this->Speed = oldspeed;
            speedMode = false;
        }
    }
}

void Player::waterRefill()
{
    static float initialWaitTime = 0.5;
    static float waitTime = 0.5;
    
    waitTime -= Time::deltaTime;
    if (waitTime <= 0)
    {
        if(Utilities::CheckDistance(this->Position, Level->Mocio->Position, 2.6f))
        {
            this->WaterLevel = (WaterLevel < MaxWaterLevel) ? WaterLevel + 1: WaterLevel; //incrementa l'acqua solo se è minore del livello massimo
            waitTime = initialWaitTime;
        }
    }
}
