#pragma once

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp> 

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include <optional>
#include <mutex>
#include <shared_mutex>
#include <iostream>

#include <type_traits>
#include "clientOperation.h"

class MongoStorage {
    mongocxx::instance m_dbInstance;
/* http://mongocxx.org/api/mongocxx-3.6.1/classmongocxx_1_1pool.html#detailsy
    A pool of client objects associated with a MongoDB.
 the min and max number of connections in the pool is configured
  using the 'minPoolSize' and 'maxPoolSize' connection string options.
 */
    mongocxx::pool m_pool;
    const std::string m_dbname;

    explicit MongoStorage();
public:
    static MongoStorage &instance();

    bool addVideo(const boost::json::value &video);
    std::vector<boost::json::value> getVideo(const boost::json::object &filter);
    std::optional<boost::json::value> getVideo(const boost::uuids::uuid &uuid);
    bool deleteVideo(const boost::uuids::uuid &uuid);

    std::optional<boost::json::value> getUser(const boost::json::value &uniqueFilter);
    bool addUser(const boost::json::value &user);
    std::optional<boost::json::value> getSubject(const boost::json::value &authData);
    void addSubject(const boost::json::value &fullAuthData);

    TransactionHandlePtr prepareTransaction();
    void prepareAddVideo(const boost::json::value &video, TransactionHandle &transaction) const;
    void prepareAddUser(const boost::json::value &user, TransactionHandle &transaction) const;
    void prepareAddSubject(const boost::json::value &fullAuthData, TransactionHandle &transaction) const;

    void prepareUpdateUser(const boost::json::object &userUpdate, const boost::json::object &filter, TransactionHandle &transaction) const;
    void prepareUpdateSubject(const boost::json::object &subjectUpdate, const boost::json::object &filter, TransactionHandle &transaction) const;

private:
    void deleteAll();
    std::mutex m_transactionMutex;
};
