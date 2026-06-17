//
// Created by josueaperez on 6/14/26.
//

#include "ApiServer.h"
#define JSON_USE_IMPLICIT_CONVERSIONS 0
#include "json.hpp"
#include <iostream>

using json = nlohmann::json;

// el constructor

ApiServer::ApiServer() {
    setupRoutes();
}

// el setup de las rutas

void ApiServer::setupRoutes() {
    // se habilita CORS para que React pueda hacer fetch
    server.set_pre_routing_handler(
        [](const httplib::Request &req, httplib::Response &res) {
            res.set_header("Access-Control-Allow-Origin", "*");
            res.set_header("Access-Control-Allow-Methods", "POST, GET, OPTIONS");
            res.set_header("Access-Control-Allow-Headers", "Content-Type");
            return httplib::Server::HandlerResponse::Unhandled;
        });

    // responder a OPTIONS (preflight de CORS)
    server.Options(".*", [](const httplib::Request &, httplib::Response &res) {
        res.status = 200;
    });

    server.Post("/query",
                [this](const httplib::Request &req, httplib::Response &res) {
                    handleQuery(req, res);
                });
}

// manejo de query

void ApiServer::handleQuery(const httplib::Request &req,
                            httplib::Response &res) {
    json response;

    try {
        json body = json::parse(req.body);
        std::string sql = body.at("sql").get<std::string>();
        std::string sessionId = req.remote_addr; // identificador simple de sesión

        auto executor = getOrCreateSession(sessionId);

        // se separa el script por punto y coma y se ejecuta cada query
        json statementsResults = json::array();
        std::stringstream ss(sql);
        std::string statement;

        while (std::getline(ss, statement, ';')) {
            // skipear statements vacíos (espacios o saltos de línea)
            bool isEmpty = true;
            for (char c: statement)
                if (!std::isspace(c)) {
                    isEmpty = false;
                    break;
                }
            if (isEmpty) continue;

            QueryResult result = executor->execute(statement);

            json stmtResult;
            stmtResult["success"] = result.success;
            stmtResult["timeMs"] = result.timeMs;

            if (result.success) {
                stmtResult["columns"] = result.columns;
                stmtResult["rows"] = result.rows;
            } else {
                stmtResult["error"] = result.error;
            }

            statementsResults.push_back(stmtResult);
        }

        response["results"] = statementsResults;
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception &e) {
        response["error"] = std::string("Server error: ") + e.what();
        res.status = 500;
        res.set_content(response.dump(), "application/json");
    }
}

// la parte de sesiones

std::shared_ptr<QueryExecutor> ApiServer::getOrCreateSession(
    const std::string &sessionId) {
    if (sessions.find(sessionId) == sessions.end())
        sessions[sessionId] = std::make_shared<QueryExecutor>();
    return sessions[sessionId];
}

// la parte para hacerle run

void ApiServer::run(int port) {
    std::cout << "TinySQLDb server running on port " << port << std::endl;
    server.listen("0.0.0.0", port);
}
