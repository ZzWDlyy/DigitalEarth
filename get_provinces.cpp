#include <iostream>
#include <istream>
#include <streambuf>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <stdlib.h>
#include "Location.h"
using namespace std;

vector<Province*> Location::get_provinces() {
    //定义一个provinces的vector
    std::vector<Province*> provinces;
    std::ifstream csv_data("./data/ok_geo.csv", std::ios::in);
    std::string line;
    if (!csv_data.is_open())
    {
        std::cout << "Error: opening file fail" << std::endl;
        std::exit(1);
    }
    std::istringstream sin;         //将整行字符串line读入到字符串istringstream中
    std::vector<std::string> words; //声明一个字符串向量
    std::string word;
    Province* cur_province = nullptr;
    City* cur_city = nullptr;
    District* cur_district = nullptr;
    // 读取标题行
    std::getline(csv_data, line);
    // 读取数据
    while (std::getline(csv_data, line))
    {
        sin.clear();
        sin.str(line);
        words.clear();
        while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
        {
            words.push_back(word); //将每一格中的数据逐个push
            // std::cout << atol(word.c_str());
        }
        // 存储地理数据  
        if (words[2] == "0") {

        }
        // 创建省、市、区对象并添加到数据结构中
        std::string deep = words[2];
        std::string Name = words[3];
        std::string geoString = words[4];
        std::istringstream iss(geoString);
        double longitude, latitude;
        iss >> longitude >> latitude;
        if (deep == "1") {
            cur_province = new Province(Name, longitude, latitude);
            provinces.push_back(cur_province);
        }
        else if (deep == "2") {
            cur_city = new City(Name, longitude, latitude);
            cur_province->cities.push_back(cur_city);
        }
        else if (deep == "3") {
            cur_district = new District(Name, longitude, latitude);
            cur_city->districts.push_back(cur_district);
        }
    }
    csv_data.close();
    return provinces;

}

vector<Country*> Location::get_countries() {
    //定义一个provinces的vector
    std::vector<Country*> countries;
    std::ifstream csv_data("./data/ok_geo.csv", std::ios::in);
    std::string line;
    if (!csv_data.is_open())
    {
        std::cout << "Error: opening file fail" << std::endl;
        std::exit(1);
    }
    std::istringstream sin;         //将整行字符串line读入到字符串istringstream中
    std::vector<std::string> words; //声明一个字符串向量
    std::string word;
    Country* cur_country = nullptr;
    Province* cur_province = nullptr;
    City* cur_city = nullptr;
    District* cur_district = nullptr;
    // 读取标题行
    std::getline(csv_data, line);
    // 读取数据
    while (std::getline(csv_data, line))
    {
        sin.clear();
        sin.str(line);
        words.clear();
        while (std::getline(sin, word, ',')) //将字符串流sin中的字符读到field字符串中，以逗号为分隔符
        {
            words.push_back(word); //将每一格中的数据逐个push
        }
        // 创建省、市、区对象并添加到数据结构中
        std::string deep = words[2];
        std::string Name = words[3];
        std::string geoString = words[4];
        std::istringstream iss(geoString);
        double longitude, latitude;
        iss >> longitude >> latitude;
        if (deep == "0") {
            cur_country = new Country(Name, longitude, latitude);
            countries.push_back(cur_country);
        }
        else if (deep == "1") {
            cur_province = new Province(Name, longitude, latitude);
            cur_country->provinces.push_back(cur_province);
        }
        else if (deep == "2") {
            cur_city = new City(Name, longitude, latitude);
            cur_province->cities.push_back(cur_city);
        }
        else if (deep == "3") {
            cur_district = new District(Name, longitude, latitude);
            cur_city->districts.push_back(cur_district);
        }
    }
    csv_data.close();
    return countries;

}