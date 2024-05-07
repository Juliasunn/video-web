#pragma once

#include <mongocxx/client.hpp>
#include <mongocxx/instance.hpp>
#include <mongocxx/pool.hpp>

#include <optional>
#include <iostream>
#include <type_traits>

class BaseClientOperation {
public:
    virtual void execute(mongocxx::client &client) = 0;
};

template <typename Operation>
class ClientOperation : public BaseClientOperation {
    using Rv = std::invoke_result_t<Operation, typename mongocxx::client &>;
    Operation m_operation;
public:
    ClientOperation(Operation operation):
       m_operation(operation) {};

    ClientOperation(ClientOperation<Operation> &&other):
        m_operation(std::move(other.m_operation)) {}

    void execute(mongocxx::client &client) override {
        m_operation(client);
    }

   Rv executeWithRet(mongocxx::client &client) {
    std::cout << "Execute operation " << std::endl;
        return m_operation(client);
   }
};

class TransactionHandle {
    mongocxx::pool &m_pool;
    std::vector<std::unique_ptr<BaseClientOperation>> m_preparedOperations;

public:
    TransactionHandle(mongocxx::pool &pool) : m_pool(pool){}

    TransactionHandle(const TransactionHandle &other) = delete;
    TransactionHandle(TransactionHandle &&other) = delete;

    template <typename Operation>
    void addOperation(Operation &&operation) {
        m_preparedOperations.push_back(std::make_unique<Operation>(std::move(operation)));
    }

    void executeTransaction() {
        auto clientEntry = m_pool.acquire();
        auto session = clientEntry->start_session();

        session.start_transaction();

        for (const auto &operation : m_preparedOperations) {
            operation->execute(*clientEntry);
        }
        session.commit_transaction();
    }
};

using TransactionHandlePtr = std::unique_ptr<TransactionHandle>;
 