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


namespace {

std::string addToCollection(mongocxx::collection &collection, const boost::json::value &obj) {
 // boost::json -> string
 auto serialized = boost::json::serialize(obj);

  auto insertOneResult = collection.insert_one(bsoncxx::from_json(serialized));
  if (insertOneResult) {
    auto oid = insertOneResult.value().inserted_id().get_oid();
    return oid.value.to_string();
  }
  return {};
}

}

MongoStorage::MongoStorage() : m_client(mongocxx::uri("mongodb://localhost:27017")) {
  //If the database you request does not exist, MongoDB creates it when you first store data.
  m_db = m_client["webvideodb"];
 // m_db["video"].delete_many(bsoncxx::builder::basic::make_document(
  //  bsoncxx::builder::basic::kvp("description", "Architecture of twiter\r")));
  deleteAll();
}

std::string MongoStorage::addVideo(const boost::json::value &video) {
  //If the collection you request does not exist, MongoDB creates it when you first store data.
  auto videoCollection = m_db["video"];
  return addToCollection(videoCollection, video);
}

std::vector<boost::json::value> MongoStorage::getVideo() const {
  std::vector<boost::json::value> res;

  auto videoCollection = m_db["video"];

  auto cursor_all = videoCollection.find({});
  for (auto doc : cursor_all) {
    std::string jsonStr = bsoncxx::to_json(doc);
    res.push_back(boost::json::parse(jsonStr));
    // Do something with doc
    //assert(doc["_id"].type() == bsoncxx::type::k_oid);
  }
  return res;

}

std::optional<boost::json::value> MongoStorage::getVideo(const boost::uuids::uuid &uuid) const {
  using namespace bsoncxx::builder::basic;
  auto videoCollection = m_db["video"];
  //auto filter = kvp("uuid", boost::lexical_cast<std::string>(uuid));
  auto filtered = videoCollection.find_one(make_document(kvp("uuid", boost::uuids::to_string(uuid))));

  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  return boost::json::parse(jsonStr);
}

std::string MongoStorage::addUser(const boost::json::value &user) {
  //If the collection you request does not exist, MongoDB creates it when you first store data.
  auto userCollection = m_db["user"];
  return addToCollection(userCollection, user);
}

std::optional<boost::json::value> MongoStorage::getUser(const boost::uuids::uuid &uuid) const {
  using namespace bsoncxx::builder::basic;
  auto userCollection = m_db["user"];
  //auto filter = kvp("uuid", boost::lexical_cast<std::string>(uuid));
  auto filtered = userCollection.find_one(make_document(kvp("uuid", boost::uuids::to_string(uuid))));

  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  return boost::json::parse(jsonStr);
}


std::optional<boost::uuids::uuid> MongoStorage::getSubject(const boost::json::value &providedData) const {
  using namespace bsoncxx::builder::basic;
  auto subCollection = m_db["subject"];
  auto dataSerialized = boost::json::serialize(providedData);
  //auto filter = kvp("uuid", boost::lexical_cast<std::string>(uuid));
  auto filtered = subCollection.find_one(bsoncxx::from_json(dataSerialized));

  if (!filtered) {
        return std::nullopt;
  }
  std::string jsonStr = bsoncxx::to_json(*filtered);
  auto sub = boost::json::parse(jsonStr).as_object();
  return boost::lexical_cast<boost::uuids::uuid>(boost::json::value_to<std::string>(sub["uuid"]));
}

std::string MongoStorage::addSubject(const boost::json::value &subjectClaims) const {
  auto subCollection = m_db["subject"];
  return addToCollection(subCollection, subjectClaims);  
}

void MongoStorage::deleteAll() {
  using namespace bsoncxx::builder::basic;
 // auto videoCollection = m_db["video"];
  //videoCollection.delete_many({});
  auto collection = m_db["user"];
  collection.delete_many({});
}


