#include "documentStorage.h"

#include <boost/json/object.hpp>
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

auto prepareInsertOne(const std::string &db, const std::string &collection, bsoncxx::v_noabi::document::value &&rdoc) {
 // auto &&rdoc = bsoncxx::from_json(boost::json::serialize(rawData));
  auto operation = [db, collection, doc = rdoc](mongocxx::client &client) mutable  {
    return client[db][collection].insert_one(std::move(doc));
  };
  return ClientOperation{ operation };    
}

auto prepareDeleteOne(const std::string &db, const std::string &collection, const bsoncxx::v_noabi::document::value &&uniqueFilter) {
  auto operation = [db, collection, doc = std::move(uniqueFilter)](mongocxx::client &client) mutable  {
    return client[db][collection].delete_one(std::move(doc));
  };
  return ClientOperation{ operation };    
}

auto prepareUpdateOne(const std::string &db, const std::string &collection, const bsoncxx::v_noabi::document::value &&update,
   const bsoncxx::v_noabi::document::value &&uniqueFilter ) 
{
  auto &&rupdateExpression = bsoncxx::builder::stream::document{} << "$set" << update << bsoncxx::builder::stream::finalize;
  std::cout << bsoncxx::to_json(rupdateExpression) << std::endl;
  auto operation = [db, collection, filterDoc = std::move(uniqueFilter), updateExpression = rupdateExpression](mongocxx::client &client) mutable  {
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
}

bool MongoStorage::addVideo(const Video &video) {
  return scopeExecute(prepareInsertOne(m_dbname, "video",
   DocumentConvertor<Video>::toDocument(video)),
       m_pool).has_value();
}

std::vector<Video> MongoStorage::getVideo(const VideoFilter &filter) 
{
  auto filterDoc = DocumentConvertor<VideoFilter>::toDocument(filter);

  auto operation = [filterDoc, &db=m_dbname](mongocxx::client &client) mutable  {
    std::vector<Video> res;

    /* It turned out that cursor inside calls operations on thread-shared objects, so
    we should iterate throw it inside the scope */
    auto cursor_all = client[db]["video"].find(std::move(filterDoc));
    for (auto &doc : cursor_all) {
        res.emplace_back(DocumentConvertor<Video>::fromDocument(doc));
    }
    return res;
  };
  return scopeExecute(ClientOperation{operation }, m_pool);
}

std::optional<Video> MongoStorage::getVideo(const UuidFilter &uuidFilter) 
{
  auto doc = DocumentConvertor<UuidFilter>::toDocument(uuidFilter);

  auto operation = [doc, &db=m_dbname](mongocxx::client &client) mutable  {
    return client[db]["video"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{operation}, m_pool);
  if (!filtered) {
        return std::nullopt;
  }
    return DocumentConvertor<Video>::fromDocument(filtered.value());
}

bool MongoStorage::deleteVideo(const UuidFilter &uuidFilter) {
  auto rv = scopeExecute(prepareDeleteOne(m_dbname, "video", 
  DocumentConvertor<UuidFilter>::toDocument(uuidFilter)), m_pool);
  return rv.has_value();
}

bool MongoStorage::addUser(const User &user) {
  //If the collection you request does not exist, MongoDB creates it when you first store data.
  auto rv = scopeExecute(prepareInsertOne(m_dbname,
       "user",
        DocumentConvertor<User>::toDocument(user)), m_pool);
  return rv.has_value();
}

std::optional<User> MongoStorage::getUserImpl(bsoncxx::v_noabi::document::value &&filter) 
{
  auto operation = [doc = filter, &db=m_dbname](mongocxx::client &client) mutable  {
    return client[db]["user"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{operation}, m_pool);
  if (!filtered) {
        return std::nullopt;
  }
  return DocumentConvertor<User>::fromDocument(filtered.value());
}

std::optional<Subject> MongoStorage::getSubjectImpl(bsoncxx::v_noabi::document::value &&filter)
{

  auto operation = [doc = std::move(filter), &db=m_dbname](mongocxx::client &client) mutable  { 
    return client[db]["subject"].find_one(std::move(doc));
  };
  auto filtered = scopeExecute(ClientOperation{ operation }, m_pool);

  if (!filtered) {
        return std::nullopt;
  }
  return DocumentConvertor<Subject>::fromDocument(filtered.value());
}

void MongoStorage::addSubject(const Subject &subject) {
    auto &&doc = DocumentConvertor<Subject>::toDocument(subject);
    auto rv = scopeExecute(prepareInsertOne(m_dbname, "subject", std::move(doc)), m_pool);
}

void MongoStorage::deleteAll() { 
    auto clientEntry = m_pool.acquire();
    auto collection = (*clientEntry)[m_dbname]["video"];
    collection.delete_many({});
}

TransactionHandlePtr MongoStorage::prepareTransaction() { return std::make_unique<TransactionHandle>(m_pool); }

void MongoStorage::prepareAddVideo(const Video &video, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne(m_dbname, "video",
  DocumentConvertor<Video>::toDocument(video)));
}

void MongoStorage::prepareAddUser(const User &user, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne(m_dbname, "user",
   DocumentConvertor<User>::toDocument(user)));
}

void MongoStorage::prepareAddSubject(const Subject &subject, TransactionHandle &transaction) const {
  transaction.addOperation(prepareInsertOne(m_dbname, "subject",
   DocumentConvertor<Subject>::toDocument(subject)));
}

void MongoStorage::prepareUpdateUser(const UserFilter &userUpdate, 
    const UuidFilter &filter, TransactionHandle &transaction) const 
{
  transaction.addOperation(prepareUpdateOne(m_dbname, "user",
  DocumentConvertor<UserFilter>::toDocument(userUpdate),
  DocumentConvertor<UuidFilter>::toDocument(filter)));
}

void MongoStorage::prepareUpdateSubject(const SubjectFilter &subjectUpdate,
     const UuidFilter &filter, TransactionHandle &transaction) const
{
  transaction.addOperation(prepareUpdateOne(m_dbname, "subject", 
  DocumentConvertor<SubjectFilter>::toDocument(subjectUpdate),
  DocumentConvertor<UuidFilter>::toDocument(filter)));
}