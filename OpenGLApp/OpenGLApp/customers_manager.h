#ifndef CUSTOMERS_MANAGER_H
#define CUSTOMERS_MANAGER_H

#include <vector>
#include <list>
#include "customer.h"
#include <memory>

class CustomersManager {
public:
	std::list<Customer> customers_list = std::list<Customer>(); // Array di customer presenti nel bagno
	float spawnRateTime;
	float timeBtwSpawn;

	CustomersManager(float spawnRate); // costruttore vuoto
	void SpawnCustomers(float deltaTime); // spawna customer ogni spawnRateTime

private:
	void Spawn();
	void DespawnCustomer();
	// void Spawn(int customerToEnableIndex);
	// int GetDisabledNumber();
	// int GetDisabledCustomerIndex();
};

#endif // !CUSTOMERS_MANAGER_H

