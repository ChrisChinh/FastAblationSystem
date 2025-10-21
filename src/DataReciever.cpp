#include "DataReciever.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <iostream>
#include <stdexcept>
#include <vector>
#include <cstdint>
#include <cerrno>

    
    
    
DataReciever::DataReciever(string ip, int port) {
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);

    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 1);

    cout << "Waiting for a connection on port " << port << "..." << endl;
    client_fd = accept(server_fd, NULL, NULL);
    cout << "Client connected." << endl;

}


DataReciever::~DataReciever() {
    if (client_fd >= 0) close(client_fd);
    if (server_fd >= 0) close(server_fd);
}

ssize_t DataReciever::recvAll(int socket, void* buffer, size_t length) {
    size_t total = 0;
    while (total < length) {
        ssize_t bytes = recv(socket, (char*)buffer + total, length - total, 0);
        if (bytes < 0) {
            if (errno == EINTR) continue;
            return -1;
        }
        if (bytes == 0) {
            // Peer closed
            return 0;
        }
        total += (size_t)bytes;
    }
    return (ssize_t)total;
}


vector<vector<double>> DataReciever::receiveData() {
    uint32_t nrows = 0, ncols = 0;

    ssize_t r = recvAll(client_fd, &nrows, sizeof(nrows));
    if (r != sizeof(nrows)) {
        // connection closed or error
        return {};
    }
    r = recvAll(client_fd, &ncols, sizeof(ncols));
    if (r != sizeof(ncols)) {
        return {};
    }

    unsigned int rows = ntohl(nrows);
    unsigned int cols = ntohl(ncols);

    if (rows == 0 || cols == 0) {
        return {};
    }

    // Prevent overflow in rows * cols * sizeof(double)
    const size_t maxElems = SIZE_MAX / sizeof(double);
    if ((size_t)rows > maxElems / (size_t)cols) {
        throw std::runtime_error("Invalid dimensions from client (overflow)");
    }

    const size_t totalElements = (size_t)rows * (size_t)cols;
    const size_t totalBytes = totalElements * sizeof(double);

    std::vector<double> data(totalElements);
    r = recvAll(client_fd, data.data(), totalBytes);
    if (r != (ssize_t)totalBytes) {
        // partial/failed payload read
        return {};
    }

    vector<vector<double>> result(rows, vector<double>(cols));
    for (unsigned int i = 0; i < rows; ++i) {
        for (unsigned int j = 0; j < cols; ++j) {
            result[i][j] = data[i * cols + j];
        }
    }
    return result;
}