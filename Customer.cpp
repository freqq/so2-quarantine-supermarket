#include <unistd.h>
#include "Customer.hpp"
#include "Resources.hpp"

Customer::Customer(int x, int y, Room *shopEntrance, Room *alleys,
               Room *cashRegister) {
    this->pos.x = x;
    this->pos.y = y;
    this->shopEntrance = shopEntrance;
    this->alleys = alleys;
    this->cashRegister = cashRegister;
}

void Customer::run(Customer *client) {
    while (!client->exited) {
        if ((client->pos.x > 40 && client->pos.y == 0) ||
            (client->pos.x >= 7 && client->pos.y >= 9)) {
            client->pos.x--;
        } else if (client->pos.x <= 40 && client->pos.y < 9) {
            client->pos.y++;
        } else {
            client->exited = true;
        }
        usleep(100000);
        checkRoom(client);
    }
}

void Customer::checkRoom(Customer *client) {
    if (!client->exited) {
        if (client->target != 1 && client->pos.x <= 40 && client->pos.y < 9) {
            Customer::getBasket(client);
            client->target = 1;
        } else if (client->target != 2 && client->pos.x > 7 && client->pos.y >= 9) {
            Customer::shop(client);
            client->target = 2;
        } else if (client->target != 3 && client->pos.x == 7 &&
                   client->pos.y == 9) {
            client->cashRegister->baskets++;
            client->target = 3;
        }
    }
}

void Customer::getBasket(Customer *client) {
    if (!client->exited) {
        client->shopEntrance->clients++;
        std::unique_lock<std::mutex> lck(Resources::mtxEntrance);
        Resources::cvEntrance.wait(lck);
    }
}

void Customer::shop(Customer *client) {
    if (!client->exited) {
        if (!client->alleys->isFull) {
            client->alleys->clients++;
            if (client->alleys->clients >= PEOPLE_ALLOWED)
                client->alleys->isFull = true;
            sleep(SHOPPING_TIME);
            client->alleys->clients--;
            if (client->alleys->clients < PEOPLE_ALLOWED)
                client->alleys->isFull = false;
        } else {
            client->alleys->inQueue++;
            std::unique_lock<std::mutex> lck(Resources::mtxShopping);
            Resources::cvShopping.wait(lck);
            sleep(SHOPPING_TIME);
            client->alleys->clients--;
            if (client->alleys->clients < PEOPLE_ALLOWED)
                client->alleys->isFull = false;
        }
    }
}