#pragma once
#include <algorithm>
#include <stdexcept>
#include <string>
#include <filesystem>
#include <memory>
#include <iostream>

#include <boost/json.hpp>
#include <boost/uuid/uuid.hpp>            // uuid class
#include <boost/uuid/uuid_generators.hpp> // generators

#include <endpoint.h>
#include <http/http_handlers.h>

#include "DiskStorage/diskStorage.h"


class BuffersPool {

public:
    using object_type = http::response<http::string_body>;

    struct PoolObject {
        PoolObject() : object(std::make_shared<object_type>()), inUse(false){
          std::cout << std::endl<<
          "============================================" << "CREATE POOL OBJECT" << 
          "============================================" << std::endl;
        };
        std::shared_ptr<object_type> object;
        bool inUse = false;
    };

     std::shared_ptr<object_type> get() {
      auto iter = std::find_if(m_objects.begin(), m_objects.end(), [](const auto &object) {return !object.inUse;});
      if (iter == m_objects.end()) {
        iter = m_objects.emplace(m_objects.end());
        //iter->object->body().resize(100000);
      }
      iter->inUse = true;
      //std::cout << "Pool objects count: " << m_objects.size() << std::endl;
      return iter->object;
    }

    void release(std::shared_ptr<object_type> &object) {
      auto iter = std::find_if(m_objects.begin(), m_objects.end(), 
      [&findObj = object](const PoolObject &poolObj) {
        return poolObj.object == findObj;
      });

      if (iter == m_objects.end()) {
         throw std::runtime_error("Cant find PoolObject to release");
      }
      iter->inUse = false;
    }  
private:
    std::vector<PoolObject> m_objects;
};



namespace ns_server {
  //Each handler belongs to single session
class MediaRequestHandler : public BaseHttpRequestHandler {

public:
    MediaRequestHandler(DiskStoragePtr documentStorage);
    virtual ~MediaRequestHandler() override;
    virtual std::unique_ptr<BaseHttpRequestHandler> clone() override;
    virtual void process_request(std::shared_ptr<http_session> session) override;
private:
   // char *buf = nullptr;
  
    //http::response<http::string_body> res;
    std::shared_ptr<http::response<http::string_body>> m_response = nullptr;
    bool setPayload( const disk_storage::Url &fileUrl);
    DiskStoragePtr m_diskStorage; 
};

}
