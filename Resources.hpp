//
// Created by Przemysław Wit on 2020-05-13.
//

#ifndef SO2_BOOKSTORE_RESOURCES_HPP
#define SO2_BOOKSTORE_RESOURCES_HPP

#include <condition_variable>

class Resources {
public:
    static std::mutex mtxEntrance;
    static std::condition_variable cvEntrance;

    static std::mutex mtxShopping;
    static std::condition_variable cvShopping;
};

#endif //SO2_BOOKSTORE_RESOURCES_HPP
