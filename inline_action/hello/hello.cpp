#include <eosiolib/eosio.hpp>
#include <eosiolib/print.hpp>

#include <eosiolib/transaction.hpp>
using namespace eosio;

class hello : public eosio::contract {
  public:
      using contract::contract;

      /// @abi action
      void hi(account_name from, account_name to, uint64_t sec) {
        require_auth(from);
        print("Hello, from:", name{from}, ", to:", name{to});

        transaction out{};
        out.actions.emplace_back(
          permission_level{to, N(active)},
          N(hello.target), N(callme),
          std::make_tuple(to)
        );
        out.delay_sec = sec;
        out.send(123, from);

        // action (
        //   //这里{to, active}必须授权给{_self, eosio.code}
        //   permission_level{to, N(active)},
        //   //调用 hello.target合约 的'callme' action
        //   N(hello.target), N(callme),
        //   std::make_tuple(to)
        // ).send();
      }

      /// @abi action
      void cancel(uint128_t sender_id) {
        cancel_deferred(sender_id);
        print("cancel deferred called");
      }
};

EOSIO_ABI(hello, (hi) (cancel))

//当前合约_self(hello.code)想要以to@active去调用其他合约(hello.target)，
//必须获得to的授权，即必须在to@active中添加hello.code@eosio.code授权
// ./cleos set account permission user1 active '{"threshold": 1,"keys": [],"accounts": [{"permission":{"actor":"hello.code","permission":"eosio.code"},"weight":1}]}' owner -p user1@owner

// extern "C" {
//    void apply( uint64_t receiver, uint64_t code, uint64_t action ) {
//       auto self = receiver;
//       if( action == N(onerror)) {
//          /* onerror is only valid if it is for the "eosio" code account and authorized by "eosio"'s "active permission */
//          eosio_assert(code == N(eosio), "onerror action's are only valid from the \"eosio\" system account");
//       }
//       if( code == self || action == N(onerror) ) {
//          hello thiscontract( self );
//          switch( action ) {
//             EOSIO_API( hello, (hi) )
//          }
//          /* does not allow destructor of thiscontract to run: eosio_exit(0); */
//       }
//    }
// }
