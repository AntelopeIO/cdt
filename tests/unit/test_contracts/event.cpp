#include <eosio/eosio.hpp>
#include <eosio/asset.hpp>
#include <eosio/event.hpp>

using namespace eosio;

struct [[eosio::event]] transfer_event {
   [[eosio::indexed, eosio::notify]] name        from;
   [[eosio::indexed, eosio::notify]] name        to;
                                     asset       quantity;
   [[eosio::indexed]]                std::string memo;
};

class [[eosio::contract]]  event_test : public contract {
   public:
      using contract::contract;

      [[eosio::action]] void transfer(name from, name to, asset quantity, std::string memo) {
         require_recipient(from);

         // emit the event
         transfer_event my_event{from, to, quantity, memo};
         eosio::emit_event(my_event);

         print_f("transfered : % -> % ? % (%)\n", from, to, quantity, memo);
      }
};
