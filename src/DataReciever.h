#pragma once
#include <string>
#include <vector>


using namespace std;


class DataReciever {
public:
    DataReciever(string ip, int port);
    ~DataReciever();

    vector<vector<double>> receiveData();

private:
    string ip;
    int client_fd; // Socket file descriptor for the client connection
    int server_fd; // Socket file descriptor for the server
    int port;

    ssize_t recvAll(int, void*, size_t);

};