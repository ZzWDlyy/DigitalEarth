#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>

class CSVHandler {
private:
    std::string filename;

public:
    CSVHandler(const std::string& filename) : filename(filename) {}

    bool fileExists() {
        std::ifstream file(filename);
        return file.good();
    }

    void createFile() {
        std::ofstream file(filename);
        if (file) {
            file << "IsCustom,Name,Latitude,Longitude\n";
        }
        file.close();
    }

    std::vector<std::vector<std::string>> readCSV() {
        std::vector<std::vector<std::string>> data;

        std::ifstream file(filename);
        if (file) {
            std::string line;
            while (std::getline(file, line)) {
                std::vector<std::string> row;
                std::string cell;
                std::stringstream lineStream(line);

                while (std::getline(lineStream, cell, ',')) {
                    row.push_back(cell);
                }

                data.push_back(row);
            }
        }
        file.close();

        return data;
    }

    void writeCSV(const std::vector<std::vector<std::string>>& data) {
        std::ofstream file(filename, std::ios::app);
        if (file) {
            for (const auto& row : data) {
                for (size_t i = 0; i < row.size(); ++i) {
                    file << row[i];
                    if (i != row.size() - 1) {
                        file << ",";
                    }
                }
                file << "\n";
            }
        }
        file.close();
    }

    void deleteRowByName(const std::string& name) {
        std::vector<std::vector<std::string>> data = readCSV();
        std::vector<std::vector<std::string>> newData;

        for (const auto& row : data) {
            if (row.size() > 1 && row[1] != name) {
                newData.push_back(row);
            }
        }

        std::ofstream file(filename);
        if (file) {
            for (const auto& row : newData) {
                for (size_t i = 0; i < row.size(); ++i) {
                    file << row[i];
                    if (i != row.size() - 1) {
                        file << ",";
                    }
                }
                file << "\n";
            }
        }
        file.close();
    }
    void removeRowFromCSV(int row) {
        std::vector<std::vector<std::string>> data = readCSV();

        if (row >= 0 && row < data.size()) {
            data.erase(data.begin() + row);

            std::ofstream file(filename);
            if (file) {
                for (const auto& row : data) {
                    for (size_t i = 0; i < row.size(); ++i) {
                        file << row[i];
                        if (i != row.size() - 1) {
                            file << ",";
                        }
                    }
                    file << "\n";
                }
            }
            file.close();
        }
    }
};