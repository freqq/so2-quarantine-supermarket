#include <ncurses.h>
#include <thread>
#include <unistd.h>
#include <vector>
#include <random>
#include "Resources.hpp"
#include "Customer.hpp"
#include "Room.hpp"

#define CUSTOMER 'o'  // Customer char representation
#define MAX_T 4       // Max time for new customer to show up
#define MIN_T 2       // Min time for new customer to show up

unsigned long seed = (unsigned long)(time(nullptr));
std::mt19937_64 rng(seed);
std::uniform_int_distribution<int> randInt(MIN_T, MAX_T);

std::vector<Customer *> clients;         // Clients vector
std::vector<std::thread *> thClients;    // Clients threads vector

point max_size;  // Window size
Room *shopEntrance = new Room(0, 0, 65, 7, "Shop entrance");
Room *alleys = new Room(0, 0, 65, 7, "Alleys");
Room *cashRegister = new Room(0, 0, 20, 7, "Cash Register");
Room *corridor;
bool end = false;     // Exit program flag
bool closing = true;  // End of thread flag

void init();           // Init method
void print();          // Terminal printing method
void exiting();        // Checking if ESCAPE button pressed
void newClient();      // Adding new customer
void exitedClients();  // Removing exiting customers
void moveBasketsToEntrance(Room *alleys,
                           Room *shopEntrance);  // Cleaning and moving baskets to shop entrance
void getNewBasket(Room *shopEntrance);           // Getting free baskets
void shopping(Room *alleys);                     // Shopping

void init() {
    initscr();
    noecho();
    curs_set(0);

    getmaxyx(stdscr, max_size.y, max_size.x);

    shopEntrance->x = max_size.x - 35;
    alleys->x = max_size.x - 35;
    alleys->y = max_size.y - alleys->verticalSize;
    cashRegister->y = max_size.y - cashRegister->verticalSize;
    shopEntrance->setBaskets(BASKETS);

    int corh = max_size.y - shopEntrance->verticalSize - alleys->verticalSize;
    int corw = max_size.x;
    corridor = new Room(0, shopEntrance->verticalSize, corw, corh, "Corridor");
}

void print() {
    while (!end) {
        WINDOW *kit =
                newwin(shopEntrance->verticalSize, shopEntrance->horizontalSize, shopEntrance->y,
                       shopEntrance->x - (shopEntrance->horizontalSize / 2));
        box(kit, 0, 0);
        touchwin(kit);
        mvwprintw(kit, 1, 1, shopEntrance->getName());
        mvwprintw(kit, 2, 1, "In queue: %d", shopEntrance->clients);
        mvwprintw(kit, 3, 1, "Baskets: %d/%d", shopEntrance->baskets, BASKETS);
        wrefresh(kit);

        WINDOW *din = newwin(alleys->verticalSize, alleys->horizontalSize,
                             alleys->y, alleys->x - (alleys->horizontalSize / 2));
        box(din, 0, 0);
        touchwin(din);
        mvwprintw(din, 1, 1, alleys->getName());
        mvwprintw(din, 2, 1, "Shopping: %d/%d", alleys->clients, PEOPLE_ALLOWED);
        mvwprintw(din, 3, 1, "In queue: %d", alleys->inQueue);
        wrefresh(din);

        WINDOW *dish =
                newwin(cashRegister->verticalSize, cashRegister->horizontalSize,
                       cashRegister->y, cashRegister->x);
        box(dish, 0, 0);
        touchwin(dish);
        mvwprintw(dish, 1, 1, cashRegister->getName());
        mvwprintw(dish, 2, 1, "Baskets: %d/%d", cashRegister->baskets, BASKETS);
        wrefresh(dish);

        WINDOW *cor = newwin(corridor->verticalSize, corridor->horizontalSize,
                             corridor->y, corridor->x);

        touchwin(cor);

        mvwprintw(cor, 1, 3, "Biedronka - 'CORONA EDITION'");

        for (auto &client : clients) {
            mvwaddch(cor, client->pos.y, client->pos.x, CUSTOMER);
        }

        wrefresh(cor);
        exitedClients();
        usleep(100000);
    }

    while (closing) {
        clear();
        mvprintw(max_size.y / 2, max_size.x / 2 - 7, "Closing .  ");
        refresh();
        usleep(500000);
        clear();
        mvprintw(max_size.y / 2, max_size.x / 2 - 7, "Closing .. ");
        refresh();
        usleep(500000);
        clear();
        mvprintw(max_size.y / 2, max_size.x / 2 - 7, "Closing ...");
        refresh();
        usleep(500000);
    }
}

void exiting() {
    while (!end) {
        if (getch() == 27) end = true;
    }
}

void newClient() {
    Customer *client = new Customer(corridor->horizontalSize - 2, 0, shopEntrance,
                                alleys, cashRegister);
    clients.push_back(client);
    std::thread *th = new std::thread(Customer::run, client);
    thClients.push_back(th);
    sleep(randInt(rng));
}

void exitedClients() {
    for (unsigned int i = 0; i < clients.size(); i++) {
        if (clients.at(i)->exited) {
            thClients.erase(thClients.begin() + i);
            clients.erase(clients.begin() + i);
        }
    }
}

void moveBasketsToEntrance(Room *alleys, Room *shopEntrance) {
    while (!end) {
        if (alleys->baskets > 0) {
            sleep(TRANSPORT_TIME);
            alleys->baskets--;
            shopEntrance->baskets++;
        } else
            sleep(1);
    }
}

void getNewBasket(Room *shopEntrance) {
    while (!end) {
        if (shopEntrance->clients > 0 && shopEntrance->baskets > 0) {
            sleep(2);
            shopEntrance->baskets--;
            shopEntrance->clients--;
            Resources::cvEntrance.notify_one();
        } else
            sleep(1);
    }
}

void shopping(Room *alleys) {
    while (!end) {
        if (alleys->inQueue > 0 && !alleys->isFull) {
            alleys->inQueue--;
            alleys->clients++;

            if (alleys->clients >= PEOPLE_ALLOWED) {
                alleys->isFull = true;
            }

            Resources::cvShopping.notify_one();
        } else
            sleep(1);
    }
}

int main(int argc, char **argv) {
    init();

    std::thread ending(exiting);
    std::thread screen(print);
    std::thread basketTransporter(moveBasketsToEntrance, cashRegister, shopEntrance);
    std::thread basketGiver(getNewBasket, shopEntrance);
    std::thread shopGiver(shopping, alleys);

    while (!end) newClient();

    ending.join();

    basketTransporter.join();
    basketGiver.join();
    shopGiver.join();

    for (auto &client : clients) {
        client->exited = true;
    }

    Resources::cvEntrance.notify_all();
    Resources::cvShopping.notify_all();

    exitedClients();

    closing = false;
    screen.join();

    endwin();
    return 0;
}