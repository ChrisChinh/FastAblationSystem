#include "DataReciever.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>

    
    
    
DataReciever::DataReciever(string ip, int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);
    this->port = port;

    cout << "Waiting for a connection on port " << port << "..." << endl;
    client_fd = accept(server_fd, NULL, NULL);
    cout << "Client connected." << endl;

}


DataReciever::~DataReciever() {

}

int DataReciever::reconnect() {
    close(client_fd);
    cout << "Waiting for a new connection on port " << port << "..." << endl;
    client_fd = accept(server_fd, NULL, NULL);
    cout << "Client reconnected." << endl;
    return 0;
}

ssize_t DataReciever::recvAll(int socket, void* buffer, size_t length) {
    size_t total = 0;
    while (total < length) {
        ssize_t bytes = recv(socket, (char*)buffer + total, length - total, 0);
        if (bytes <= 0) return bytes;
        total += bytes;
    }
    return total;
}


vector<vector<double>> DataReciever::receiveData() {
    // Implementation of receiveData method
    unsigned int rows, cols;
    ssize_t r = recvAll(client_fd, &rows, sizeof(rows));
    if (r <= 0) return vector<vector<double>>(0, vector<double>()); // Return empty on error
    recvAll(client_fd, &cols, sizeof(cols));

    size_t totalElements = (size_t) rows * cols;
    double *data = new double[totalElements];
    if (!data) {
        throw std::runtime_error("Out of memory");
    }

    recvAll(client_fd, data, totalElements * sizeof(double));
    vector<vector<double>> result(rows, vector<double>(cols));
    for (unsigned int i = 0; i < rows; ++i) {
        for (unsigned int j = 0; j < cols; ++j) {
            result[i][j] = data[i * cols + j];
        }
    }

    delete[] data;
    return result;
}

void DataReciever::sendDouble(double value) {
    send(client_fd, &value, sizeof(value), 0);
}

int DataReciever::receiveInt() {
    int value;
    ssize_t r = recvAll(client_fd, &value, sizeof(value));
    if (r <= 0) {
        std::cerr << "Socket disconnected while receiving int, attempting to reconnect..." << std::endl;
        if (reconnect() != 0) {
            throw std::runtime_error("Reconnect failed");
        }
        r = recvAll(client_fd, &value, sizeof(value));
        if (r <= 0) {
            throw std::runtime_error("Failed to receive int after reconnect");
        }
    }
    return value;
}

double DataReciever::receiveDouble() {
    double value;
    recvAll(client_fd, &value, sizeof(value));
    return value;
}