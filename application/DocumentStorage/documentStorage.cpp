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

auto prepareUpdateOne(const std::string &db, const std::string &collection, const boost::json::object &update,
   const boost::json::object &filter ) 
{
  auto updateDoc = bsoncxx::from_json(boost::json::serialize(update));
  auto updateExpression = bsoncxx::builder::stream::document{} << "$set" << updateDoc << bsoncxx::builder::stream::finalize;
  std::cout << bsoncxx::to_json(updateExpression) << std::endl;
  auto filterDoc = bsoncxx::from_json(boost::json::serialize(filter));

  auto operation = [db, collection, filterDoc, updateExpression](mongocxx::client &client) mutable  {
    return client[db][collection].update_one(std::move(filterDoc), std::move(updateExpression));
  };
  return ClientOperation{ operation };    
}

}

MongoStorage &MongoStorage::instance() {
    static MongoStorage documentStorage;
    return documentStorage;
}

MongoStorage::MongoStorage() : m_pool(mongocxx::uri("mongodb://localhost:27017")), m_dbname("webvideodb") {
  //deleteAll();
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
  std::cout << "[MongoStorage::getUser]" << uniqueFilter << std::endl;

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

std::optional<boost::json::value> MongoStorage::getSubject(const boost::json::value &providedData)
{
  std::cout << "[Get subject] " << boost::json::serialize(providedData) << std::endl;
  auto doc = bsoncxx::from_json(boost::json::serialize(providedData));

  auto operation = [doc](mongocxx::client &client) mutable  {
    return client["webvideodb"]["subject"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{ operation }, m_pool);

  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  return boost::json::parse(jsonStr);
  //return boost::lexical_cast<boost::uuids::uuid>(boost::json::value_to<std::string>(sub["uuid"]));
}

void MongoStorage::addSubject(const boost::json::value &subjectClaims) {
  auto rv = scopeExecute(prepareInsertOne("webvideodb", "subject", subjectClaims), m_pool);
}

void MongoStorage::deleteAll() { 
  auto clientEntry = m_pool.acquire();
 // auto collection = (*clientEntry)[m_dbname]["user"];
 auto collection = (*clientEntry)[m_dbname]["video"];
  collection.delete_many({});
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

void MongoStorage::prepareUpdateUser(const boost::json::object &userUpdate, 
    const boost::json::object &filter, TransactionHandle &transaction) const 
{
  transaction.addOperation(prepareUpdateOne("webvideodb", "user", userUpdate, filter));
}

void MongoStorage::prepareUpdateSubject(const boost::json::object &subjectUpdate,
     const boost::json::object &filter, TransactionHandle &transaction) const
{
  transaction.addOperation(prepareUpdateOne("webvideodb", "subject", subjectUpdate, filter));
}