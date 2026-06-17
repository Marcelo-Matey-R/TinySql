//
// Created by josueaperez on 6/14/26.
//

#ifndef SERVER_APISERVER_H
#define SERVER_APISERVER_H

#include "httplib.h"
#include "../../query_processor/include/QueryExecutor.h"
#include <string>
#include <unordered_map>
#include <memory>

class ApiServer {
private:
    httplib::Server server;
    std::unordered_map<std::string, std::shared_ptr<QueryExecutor> > sessions;

public:
    ApiServer();

    void run(int port);

private:
    void setupRoutes();

    void handleQuery(const httplib::Request &req, httplib::Response &res);

    std::shared_ptr<QueryExecutor> getOrCreateSession(const std::string &sessionId);
};


#endif //SERVER_APISERVER_H
