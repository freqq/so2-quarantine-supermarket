//
// Created by Przemysław Wit on 2020-05-13.
//

#ifndef SO2_BOOKSTORE_ROOM_HPP
#define SO2_BOOKSTORE_ROOM_HPP

#define BASKETS 10        // Number of available baskets
#define TRANSPORT_TIME 4  // Time needed to transport baskets to main entrance

#include <string>

class Room {
public:
    int x;
    int y;
    int horizontalSize;  // Height of room box
    int verticalSize;    // Width of room box
    int clients = 0;     // Number of customers inside a room
    int baskets = 0;     // Number of baskets inside a room
    int inQueue = 0;     // Number of customers in queue

    bool isFull = false;  // Is room full

    std::string name;  // Room name

    char const* getName();
    void setBaskets(int number);

    Room(int x, int y, int horizontalSize, int verticalSize, std::string name);
};

#endif  // SO2_BOOKSTORE_ROOM_HPP
