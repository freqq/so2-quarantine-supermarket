//
// Created by Przemysław Wit on 2020-05-13.
//

#include "Resources.hpp"

std::mutex Resources::mtxEntrance;
std::condition_variable Resources::cvEntrance;
std::mutex Resources::mtxShopping;
std::condition_variable Resources::cvShopping;