#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>

#include <optional>

#include "video.h"

class MongoStorage {

    mongocxx::client m_client;
    mongocxx::instance m_dbInstance;
    mongocxx::database m_db;
    explicit MongoStorage();
public:
    static MongoStorage &instance() {
        static MongoStorage documentStorage;
        return documentStorage;
    }
    
    std::string addVideo(const boost::json::value &video);

    std::vector<boost::json::value> getVideo() const;
    std::optional<boost::json::value> getVideo(const boost::uuids::uuid &uuid) const;

    std::optional<boost::json::value> getUser(const boost::uuids::uuid &uuid) const;
    std::string addUser(const boost::json::value &user);

  //  std::optional<boost::json::value> getUser(const boost::uuids::uuid &uuid) const; 
 
   // std::optional<boost::json::value> getSubject(const boost::uuids::uuid &uuid) const;
   // login: "Emily white" password: "983439fdjHHsbb78"
   // mail: "emilywhile@gmail.com" password: "983439fdjHHsbb78"
   // phone: "87654232212" password: "983439fdjHHsbb78"
    std::optional<boost::uuids::uuid> getSubject(const boost::json::value &authData) const;
    std::string addSubject(const boost::json::value &fullAuthData) const;
private:
    void deleteAll();
};


