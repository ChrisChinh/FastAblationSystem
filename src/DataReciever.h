#pragma once
#include <string>
#include <vector>


using namespace std;


class DataReciever {
public:
    DataReciever(string ip, int port);
    ~DataReciever();

    bool connect();
    vector<vector<double>> receiveData();

private:
    string ip;
    int port;

};