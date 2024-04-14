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

//void opendDB() {
    // Create an instance.
    //mongocxx::instance inst{};
    mongocxx::options::client client_options;
   // auto api = mongocxx::options::server_api{ mongocxx::options::server_api::version::k_version_1 };
   // client_options.server_api_opts(api);      
//}

MongoStorage::MongoStorage() : m_client(mongocxx::uri("mongodb://localhost:27017")) {
  //If the database you request does not exist, MongoDB creates it when you first store data.
  m_db = m_client["webvideodb"];
 // m_db["video"].delete_many(bsoncxx::builder::basic::make_document(
  //  bsoncxx::builder::basic::kvp("description", "Architecture of twiter\r")));
  //deleteAll();
}

std::string MongoStorage::addVideo(const boost::json::value &video) {
  //If the collection you request does not exist, MongoDB creates it when you first store data.
  auto videoCollection = m_db["video"];
  auto videoSerialized = boost::json::serialize(video);

  auto insert_one_result = videoCollection.insert_one(bsoncxx::from_json(videoSerialized));
  if (insert_one_result) {
    auto oid = insert_one_result.value().inserted_id().get_oid();
    return oid.value.to_string();
  }
  return {};
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
  auto claimsSerialized = boost::json::serialize(subjectClaims);

  auto insert_one_result = subCollection.insert_one(bsoncxx::from_json(claimsSerialized));
  if (insert_one_result) {
    auto oid = insert_one_result.value().inserted_id().get_oid();
    return oid.value.to_string();
  }
  return {};  
}

void MongoStorage::deleteAll() {
  using namespace bsoncxx::builder::basic;
  auto videoCollection = m_db["video"];
  videoCollection.delete_many({});
}


