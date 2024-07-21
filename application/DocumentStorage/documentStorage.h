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
#include "convertors/video.h"
#include "convertors/subject.h"
#include "convertors/user.h"
#include "convertors/filters.h"
#include "convertors/stream.h"

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

    bool addVideo(const Video &video);

    std::vector<Video> getVideo(const VideoFilter &filter);
    std::optional<Video> getVideo(const UuidFilter &uuidfilter);
    bool deleteVideo(const UuidFilter &uuid);

    template<typename Filter>
    std::optional<User> getUser(const Filter &uniqueFilter) {
        return getUserImpl(DocumentConvertor<Filter>::toDocument(uniqueFilter));
    }

    bool addUser(const User &user);

    template<typename Filter>
    std::optional<Subject> getSubject(const Filter &authFilter){
        return getSubjectImpl(DocumentConvertor<Filter>::toDocument(authFilter)); 
    }

    void addSubject(const Subject &subject);

    TransactionHandlePtr prepareTransaction();
    void prepareAddVideo(const Video &video, TransactionHandle &transaction) const;
    void prepareAddUser(const User &user, TransactionHandle &transaction) const;
    void prepareAddSubject(const Subject &fullAuthData, TransactionHandle &transaction) const;

    void prepareUpdateUser(const UserFilter &userUpdate, const UuidFilter &filter, TransactionHandle &transaction) const;
    void prepareUpdateSubject(const SubjectFilter &subjectUpdate, const UuidFilter &filter, TransactionHandle &transaction) const;

private:
    void deleteAll();
    std::optional<User> getUserImpl(bsoncxx::v_noabi::document::value &&filter);
    std::optional<Subject> getSubjectImpl(bsoncxx::v_noabi::document::value &&filter);
    std::mutex m_transactionMutex;
};
