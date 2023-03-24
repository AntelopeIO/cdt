#pragma once

using namespace eosio;

TABLE code_hash {
   uint64_t                    id;
   checksum256                 hash;
   uint64_t primary_key()      const { return id; }
};

