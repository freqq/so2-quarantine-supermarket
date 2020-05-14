//
// Created by Przemysław Wit on 2020-05-13.
//

#ifndef SO2_BOOKSTORE_CLIENT_HPP
#define SO2_BOOKSTORE_CLIENT_HPP

#include "Room.hpp"

#define SHOPPING_TIME 10  // Time needed to finish shopping
#define PEOPLE_ALLOWED 5  // Quantity of people allowed in shop

struct point {
    int x;
    int y;
};

class Customer {
public:
    int target = 0;       // Target room
    bool exited = false;  // Is client leaving

    Room *shopEntrance;
    Room *alleys;
    Room *cashRegister;

    point pos = point{0, 0};  // Customer position on 'map'

    static void run(Customer *client);
    static void checkRoom(Customer *client);
    static void getBasket(Customer *client);
    static void shop(Customer *client);

    Customer(int x, int y, Room *shopEntrance, Room *alleys, Room *cashRegister);
};

#endif  // SO2_BOOKSTORE_CLIENT_HPP
