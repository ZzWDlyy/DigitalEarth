#pragma execution_character_set("utf-8")

#ifndef LOCATION_H
#define LOCATION_H

#include <string>
#include <vector>

class District {
public:
    std::string name;
    double longitude;
    double latitude;

    District(const std::string& name, double longitude, double latitude)
    : name(name), longitude(longitude), latitude(latitude) {}
};

class City {
public:
    std::string name;
    double longitude;
    double latitude;
    std::vector<District*> districts;

    City(const std::string& name, double longitude, double latitude)
    : name(name), longitude(longitude), latitude(latitude) {}
};

class Province {
public:
    std::string name;
    double longitude;
    double latitude;
    std::vector<City*> cities;

    Province(const std::string& name, double longitude, double latitude)
    : name(name), longitude(longitude), latitude(latitude) {}
};
class Country {
public:
    std::string name;
    double longitude;
    double latitude;
    std::vector<Province*> provinces;
    Country(const std::string& name, double longitude, double latitude)
        : name(name), longitude(longitude), latitude(latitude) {}
    
};

class Location {
public:
    std::vector<Province*>get_provinces();
    std::vector<Country*>get_countries();
};

#endif // LOCATION_H