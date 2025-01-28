#ifndef CUSTOMERS_MANAGER_H
#define CUSTOMERS_MANAGER_H

#include <vector>
#include <list>
#include "wc.h"
#include "game_level.h"
#include <memory>
#include "customer.h"

class Game;

class CustomersManager {
public:
    std::list<Customer> customers_list = std::list<Customer>(); // Array di customer presenti nel bagno
    std::list<Wc> toiletsAvailable = std::list<Wc>();
    GameLevel* Level;
    Game* game;
    
    

    //da modificare ora che i customer hanno il riferimento al menager
    int queueLenght;
    int total_spawned;
    
    int customersWaves[10] = {1, 1, 2, 3 , 5, 7, 8, 8, 9};
    int customerToServe;
    
    float spawnRateTime;
    float timeBtwSpawn;

    CustomersManager(float spawnRate, Game* game); // costruttore vuoto
    //CustomersManager(float spawnRate ); // costruttore vuoto
    
    void SpawnCustomers(); // spawna customer ogni spawnRateTime
    void updateBehaviour();

private:
    void Spawn(float customerSpeed);
    void DespawnCustomer();
    // void Spawn(int customerToEnableIndex);
    // int GetDisabledNumber();
    // int GetDisabledCustomerIndex();
};

#endif // !CUSTOMERS_MANAGER_H


