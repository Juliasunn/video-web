#include "documentStorage.h"

#include <mongocxx/client.hpp>
#include <bsoncxx/builder/stream/document.hpp>

#include <bsoncxx/builder/basic/document.hpp>
#include <bsoncxx/builder/basic/kvp.hpp>

#include <bsoncxx/json.hpp>
#include <mongocxx/uri.hpp>

#include <algorithm>
#include <iostream>
#include <vector>

#include <boost/lexical_cast.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <boost/bind.hpp>

/* Transactions in mongocxx
https://mongoc.org/libmongoc/current/mongoc_client_session_start_transaction.html 

*/
using namespace bsoncxx::builder::basic;

namespace {

//prepare operations -> session.start_transaction -> execute operations -> finish transaction

template <typename ClientOperation>
auto scopeExecute(ClientOperation &&operation, mongocxx::pool &pool) {
  mongocxx::pool::entry clientEntry = pool.acquire();
  return operation.executeWithRet(*clientEntry);
}

auto prepareInsertOne(const std::string &db, const std::string &collection, const boost::json::value &rawData) {
  auto doc = bsoncxx::from_json(boost::json::serialize(rawData));
  auto operation = [db, collection, doc](mongocxx::client &client) mutable  {
    return client[db][collection].insert_one(std::move(doc));
  };
  return ClientOperation{ operation };    
}

}

MongoStorage &MongoStorage::instance() {
    static MongoStorage documentStorage;
    return documentStorage;
}

MongoStorage::MongoStorage() : m_pool(mongocxx::uri("mongodb://localhost:27017")), m_dbname("webvideodb") {
  //If the database you request does not exist, MongoDB creates it when you first store data.
 // m_db = m_client["webvideodb"];
  //m_session = m_client.start_session();
 // m_db["video"].delete_many(bsoncxx::builder::basic::make_document(
  //  bsoncxx::builder::basic::kvp("description", "Architecture of twiter\r")));
  deleteAll();
  std::cout << "[MongoStorage] constructor!" << std::endl;
}

void MongoStorage::addVideo(const boost::json::value &video) {
  auto rv = scopeExecute(prepareInsertOne("webvideodb", "video", video), m_pool);
}

std::vector<boost::json::value> MongoStorage::getVideo() 
{
  auto operation = [](mongocxx::client &client) mutable  {
    return client["webvideodb"]["video"].find({});
  };
  auto cursor_all = scopeExecute(ClientOperation{operation }, m_pool);

  std::vector<boost::json::value> res;
  for (auto doc : cursor_all) {
    std::string jsonStr = bsoncxx::to_json(doc);
    res.push_back(boost::json::parse(jsonStr));
    // Do something with doc
    //assert(doc["_id"].type() == bsoncxx::type::k_oid);
  }
  return res;
}

std::optional<boost::json::value> MongoStorage::getVideo(const boost::uuids::uuid &uuid) 
{
  auto doc = make_document(kvp("uuid", boost::uuids::to_string(uuid)));

  auto operation = [doc](mongocxx::client &client) mutable  {
    return client["webvideodb"]["video"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{operation}, m_pool);
  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  return boost::json::parse(jsonStr);
}

void MongoStorage::addUser(const boost::json::value &user) {
  //If the collection you request does not exist, MongoDB creates it when you first store data.
  auto rv = scopeExecute(prepareInsertOne("webvideodb", "user", user), m_pool);
}

std::optional<boost::json::value> MongoStorage::getUser(const boost::json::value &uniqueFilter) 
{
  auto doc = bsoncxx::from_json(boost::json::serialize(uniqueFilter));
  auto operation = [doc](mongocxx::client &client) mutable  {
    return client["webvideodb"]["user"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{operation}, m_pool);
  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  return boost::json::parse(jsonStr);
}

std::optional<boost::uuids::uuid> MongoStorage::getSubject(const boost::json::value &providedData)
{
  auto doc = bsoncxx::from_json(boost::json::serialize(providedData));

  auto operation = [doc](mongocxx::client &client) mutable  {
    return client["webvideodb"]["subjectr"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{ operation }, m_pool);

  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  auto sub = boost::json::parse(jsonStr).as_object();
  return boost::lexical_cast<boost::uuids::uuid>(boost::json::value_to<std::string>(sub["uuid"]));
}

void MongoStorage::addSubject(const boost::json::value &subjectClaims) {
  auto rv = scopeExecute(prepareInsertOne("webvideodb", "subject", subjectClaims), m_pool);
}

void MongoStorage::deleteAll() { 
 // auto clientEntry = m_pool.acquire()
 // auto videoCollection = m_db["video"];
  //videoCollection.delete_many({});
 // auto collection = (*clientEntry)[m_dbname]["user"];
 // collection.delete_many({});
}

TransactionHandlePtr MongoStorage::prepareTransaction() { return std::make_unique<TransactionHandle>(m_pool); }

void MongoStorage::prepareAddVideo(const boost::json::value &video, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne("webvideodb", "video", video));
}

void MongoStorage::prepareAddUser(const boost::json::value &user, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne("webvideodb", "user", user));
}

void MongoStorage::prepareAddSubject(const boost::json::value &fullAuthData, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne("webvideodb", "subject", fullAuthData));
}