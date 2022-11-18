#pragma once
#include "../../core/eosio/context.hpp"
#include "../../core/eosio/datastream.hpp"
#include "../../core/eosio/name.hpp"
#include "../../core/eosio/varint.hpp"

#include <cstddef>
#include <limits>
#include <algorithm>
#include <cctype>
#include <functional>
#include <string_view>
#include <unordered_map>
#include <iterator>

namespace eosio {

   namespace detail {

      template <typename Map>
      struct elem {
         using key_t             = typename Map::key_t;
         using value_t           = typename Map::value_t;
         using hash_key_t        = typename Map::hash_key_t;
         using hash_t            = typename Map::hash_t;
         using raw_iterator_t    = typename Map::raw_iterator_t;
         using record_t          = typename Map::record_t;

         constexpr static inline raw_iterator_t invalidated_iterator = std::numeric_limits<raw_iterator_t>::max();

         elem() = default;
         elem(const elem&) = default;
         elem(elem&& e) = default;
         elem& operator=(elem&&) = default;
         elem(key_t k)
            : key(std::move(k)), hashed_key(hash_t{}(k)) {}
         elem(key_t k, value_t v)
            : key(std::move(k)), hashed_key(hash_t{}(k)), value(std::move(v)) {}
         elem(const hash_key_t hk, const key_t& k, value_t v)
            : key(std::move(k)), hashed_key(hk), value(std::move(v)) {}
         elem(raw_iterator_t it, const hash_key_t hk, const key_t& k, value_t v)
            : raw_itr(it), key(std::move(k)), hashed_key(hk), value(std::move(v)) {}
         
         void update(record_t&& r) {
            key = std::move(r.first);
            value = std::move(r.second);
         }

         key_t          key;
         hash_key_t     hashed_key;
         raw_iterator_t raw_itr = invalidated_iterator;
         value_t        value;
      };

      template <class Alloc>
      struct buffer {
         buffer(size_t n)
         : size(n) {
            data = alloc.allocate(size);
         }
         buffer(buffer&& buf)
            : data(buf.data), size(buf.size) {
         }
         ~buffer() {
            alloc.deallocate(data, size);
         }
         Alloc      alloc;
         char*      data;
         size_t     size;
      };

      template <typename T>
      struct temp_alloc {

         constexpr static std::size_t max_size = 512 / sizeof(T);
         static T static_data[max_size];
         static std::vector<T> dynamic_data;

         // maintain std::allocator signature
         T* allocate(size_t size_needed) {
            if (size_needed > max_size) {
               if (dynamic_data.size() < size_needed)
                  dynamic_data.resize(size_needed);
               return dynamic_data.data();
            } else {
               return &static_data[0];
            }
         }
         inline void deallocate(T*, size_t){}
      };

      template<typename T>
      T temp_alloc<T>::static_data[temp_alloc<T>::max_size];
      template<typename T>
      std::vector<T> temp_alloc<T>::dynamic_data;

      template <class Base>
      struct quadric_probing_policy {
         using key_t          = typename Base::key_t;
         using hash_key_t     = typename Base::hash_key_t;
         using raw_iterator_t = typename Base::raw_iterator_t;
         using record_t       = typename Base::record_t;
         using bucket_t       = record_t;
         using buffer_t       = typename Base::buffer_t;
         using elem_t         = typename Base::elem_t;
         
         static constexpr inline raw_iterator_t invalidated_iterator = Base::elem_t::invalidated_iterator;

         static inline bucket_t element_to_bucket(const elem_t& el) {
            return {el.key, el.value};
         }
         static inline record_t get_record(const bucket_t& b) {
            return b;
         }
         static inline hash_key_t next_hash(uint16_t collision_num, hash_key_t hash_key) {
            if (!collision_num)
               return hash_key;
            
            auto inc = collision_num * collision_num;
            if (hash_key > std::numeric_limits<hash_key_t>::max() - inc) {
               return std::numeric_limits<hash_key_t>::max() - hash_key;
            };

            return hash_key + inc;
         }
         inline bucket_t extract_bucket(hash_key_t hashed_key, raw_iterator_t& raw_itr, const key_t& k) const {
            return extract_bucket(hashed_key, raw_itr);
         }
         bucket_t extract_bucket(hash_key_t hashed_key, raw_iterator_t& raw_itr) const {
            using namespace internal_use_do_not_use;
            
            raw_itr = db_find_i64( owner.value, scope.value, Base::table_name.value, hashed_key );
            if( raw_itr >= 0 ) {
               return extract_bucket(raw_itr);
            }

            raw_itr = invalidated_iterator;
            return {};
         }
         bucket_t extract_bucket(raw_iterator_t itr) const {
            using namespace internal_use_do_not_use;
            
            check(itr != invalidated_iterator, "invalid iterator passed");

            uint32_t size = db_get_i64( itr, nullptr, 0 );
            buffer_t buff(size);

            auto wrote = db_get_i64( itr, buff.data, buff.size );
            check(wrote == buff.size, "kv get internal failure");

            return unpack<bucket_t>(buff.data, buff.size);
         }

         name owner;
         name scope;
      };

      template <class Map>
      struct iterator {
         using elem_t         = typename Map::elem_t;
         using record_t       = typename Map::record_t;
         using raw_iterator_t = typename Map::raw_iterator_t;
         using map_t          = Map;

         static constexpr inline raw_iterator_t invalidated_iterator = elem_t::invalidated_iterator;

         inline iterator() : element(invalidated_iterator, {}, {}, {}) {}
         inline iterator(raw_iterator_t itr, const map_t* m)
            : element(itr, {}, {}, {}), map(m) {}
         inline iterator(elem_t&& el, const map_t* m) : element(std::move(el)), map(m) {}

         iterator(const iterator&) = delete;
         iterator& operator=(const iterator&) = delete;

         inline iterator(iterator&& o)
            : element(o.element) {
            }
         
         inline iterator& operator=(iterator&& o) {
            return const_cast<iterator&>(const_cast<const iterator&>(*this).operator=(std::forward<iterator&&>(o)));
         }
         inline const iterator& operator=(iterator&& o) const {
            element = std::move(o.element);
            map     = o.map;
            return *this;
         }

         ~iterator() {}

         inline elem_t& operator*() {
            materialize();
            return element;
         }
         inline const elem_t& operator*() const {
            materialize();
            return element;
         }
         inline elem_t* operator->() {
            materialize();
            return &element;
         }
         inline const elem_t* operator->() const {
            materialize();
            return &element;
         }

         iterator& operator++() {
            assert_valid();

            uint64_t next_pk;
            auto next_itr = internal_use_do_not_use::db_next_i64( element.raw_itr, &next_pk );
            if( next_itr < 0 ) {
               element.raw_itr = invalidated_iterator;
            }
            else {
               element.raw_itr = next_itr;
            }
            return *this;
         }

         inline iterator& operator++(int) {
            auto temp = *this;
            ++(*this);
            return temp;
         }

         inline bool operator==(const iterator& o) const {
            return (element.raw_itr == o.element.raw_itr);
         }

         inline bool operator!=(const iterator& o) const { return !((*this) == o); }

         void materialize() const {
            assert_valid();
            static auto bucket_data = map->collision_policy.extract_bucket(element.raw_itr);
            element.update( map->collision_policy.get_record(bucket_data) );
         }

         // mutable for const_iterator. we do not consider increment and element change as non const operation
         // the only methods where constness play role are dereference and pointer
         mutable elem_t          element;
         mutable const map_t*    map;

      private:
         inline void assert_valid() const {
            eosio::check( element.raw_itr != invalidated_iterator, "cannot increment end iterator" );
         }
      };

      template<class Map>
      using const_iterator = const iterator<Map>;
   } // namespace eosio::detail

   template <eosio::name::raw TableName, 
             typename Key, 
             typename Value, 
             class Hash = std::hash<Key>,
             template <typename> class CollisionPolicy = detail::quadric_probing_policy,
             template <typename> class Allocator = detail::temp_alloc>
   class unordered_map {
      public:
         constexpr static inline name    table_name = name{static_cast<uint64_t>(TableName)};
         static_assert( table_name && table_name.length() < 13, "multi_index does not support table names with a length greater than 12");
         constexpr static uint16_t max_collisions = 255;

         using key_t              = Key;
         using value_t            = Value;
         using record_t           = std::pair<Key, Value>;
         using hash_key_t         = uint64_t;
         using hash_t             = Hash;
         using raw_iterator_t     = int32_t;
         using allocator_t        = Allocator<char>;
         using buffer_t           = detail::buffer<allocator_t>;
         using self_t             = unordered_map<TableName, Key, Value, Hash, CollisionPolicy, Allocator>;
         using elem_t             = detail::elem<self_t>;
         using iterator_t         = detail::iterator<self_t>;
         using const_iterator_t   = detail::const_iterator<self_t>;
         
         using collision_policy_t = CollisionPolicy<self_t>;
         using bucket_t           = typename collision_policy_t::record_t;


         struct writable_wrapper {
            using unordered_map_t = self_t;
            writable_wrapper(key_t k, value_t v, name p, unordered_map_t& m)
             : element(std::move(k), std::move(v)), payer(p), map_ref(m) {}
            writable_wrapper(elem_t el, name p, unordered_map_t& m)
             : element(std::move(el)), payer(p), map_ref(m) {}

            explicit operator value_t&() { return element.value; }
            operator value_t() const { return element.value; }

            writable_wrapper& operator=(const value_t& o) {
               element.value = o;
               map_ref.set(element, payer);
               return *this;
            }

            writable_wrapper& operator=(value_t&& o) {
               element.value = std::move(o);
               map_ref.set(element, payer);
               return *this;
            }

            elem_t element;
            name payer;
            unordered_map_t& map_ref;
         };

         inline unordered_map(name o, name s)
            : owner(o), scope(s), collision_policy{o, s} {}

         /**
          * Basic constructor of N elements.
          */
         inline unordered_map(name o, name s, std::initializer_list<elem_t> l)
            : owner(o), scope(s), collision_policy{o, s} {
            for ( const auto& e : l ) {
               set(e, owner);
            }
         }

         writable_wrapper operator[](const std::pair<key_t, name>& key_payer) {
            elem_t el{key_payer.first};
            if (!get(el))
               set(el, key_payer.second);
            
            return {el, key_payer.second, *this};
         }

         writable_wrapper operator[](const key_t& k) {
            return (*this)[{k, owner}];
         }
         writable_wrapper operator[](key_t&& k) {
            return (*this)[{std::move(k), owner}];
         }

         writable_wrapper at(key_t&& k, name payer) {
            elem_t el{std::forward<key_t>(k)};
            check(get(el), "key not found");
            return {el, payer, *this};
         }
         writable_wrapper at(key_t&& k) {
            return at(std::forward<key_t&&>(k), owner);
         }
         writable_wrapper at(const key_t& k, name payer) {
            elem_t el{k};
            check(get(el), "key not found");
            return {el, payer, *this};
         }
         writable_wrapper at(const key_t& k) {
            return at(k, owner);
         }

         inline iterator_t begin() const {
            return lower_bound_internal(std::numeric_limits<hash_key_t>::lowest());
         }
         inline const_iterator_t cbegin() const {
            return begin();
         }

         inline iterator_t end() const {
            return {};
         }
         inline const_iterator_t cend() const {
            return end();
         }

         inline bool empty() const {
            return cbegin() == cend();
         }

         inline size_t size() const {
            return std::distance(begin(), end());
         }

         inline void clear() {
            erase(begin(), end());
         }

         inline std::pair<iterator_t,bool> insert( record_t&& rec ) {
            elem_t el{rec.key, rec.value};
            bool updated = get(el);
            set(el);
            return {{el, this}, !updated};
         }

         template< class InputIt >
         void insert( InputIt begin, InputIt end ) {
            while (begin != end) {
               elem_t el{begin.first, end.second};
               get(el);// we need this in case of collisions
               set(el);

               ++begin;
            }
         }
         inline void insert( std::initializer_list<record_t> ilist ) {
            insert( ilist.begin(), ilist.end() );
         }

         iterator_t find(const key_t& k) const {
            elem_t el{k, owner};
            return get(el) ? iterator_t{el, this} : end();
         }

         void erase(const key_t& k) {
            using namespace internal_use_do_not_use;
            elem_t el{k};
            check(get(el), "no such key exist");
            db_remove_i64(el.raw_itr);
         }

         iterator_t erase(const const_iterator_t& it) {
            using namespace internal_use_do_not_use;

            auto raw_itr = (*it).element.raw_itr;
            ++it;
            db_remove_i64(raw_itr);
            return const_cast<iterator_t>(it);
         }
         inline iterator_t erase(iterator_t& it) {
            return erase(it);
         }
         iterator_t erase(const const_iterator_t& begin, const const_iterator_t& end) {
            while (begin != end) {
               begin = erase(begin);
            }
            return const_cast<iterator_t>(end);
         }
         inline iterator_t erase(iterator_t& begin, iterator_t& end) {
            return erase(begin, end);
         }

         inline iterator_t lower_bound(const key_t& k) const {
            return lower_bound_internal(hash_t{}(k));
         }
protected:
         // no need to expose method that takes raw hash
         iterator_t lower_bound_internal(const hash_key_t& hk) const {
            using namespace internal_use_do_not_use;

            auto itr = db_lowerbound_i64( owner.value, scope.value, table_name.value, hk );
            if( itr < 0 ) return end();

            return {{itr, {}, {}, {}}, this};
         }
public:
         iterator_t upper_bound(const key_t& k) const {
            //we need manually increment hash 
            auto hashed_key = hash_t{}(k);
            auto next_hash = collision_policy.next_hash(1, hashed_key);
            if (hashed_key > next_hash)
               return end();
            
            return lower_bound_internal( next_hash );
         }

         inline std::pair<iterator_t, iterator_t> equal_range(const key_t& k) { 
            return {lower_bound(k), upper_bound(k)};
         }
         inline std::pair<const_iterator_t, const_iterator_t> equal_range(const key_t& k) const {
            return equal_range(k);
         }

         inline bool contains(const key_t& k) const {
            elem_t el{k};
            return get(el);
         }

         friend iterator_t;

      protected:

         bool get(elem_t& element) const {
            
            uint16_t cur_collision = 0;
            static bucket_t temp_bucket;
            static record_t temp_record;
            do {
               //intential post increment to have zero for first call. in that case next_hash supposed to return the key unchanged
               element.hashed_key = collision_policy.next_hash(cur_collision++, element.hashed_key);
               temp_bucket = collision_policy.extract_bucket(element.hashed_key, element.raw_itr, element.key);
               temp_record = collision_policy.get_record(temp_bucket);
            // loop if collision
            } while ( temp_record != record_t{} &&
                      temp_record.first != element.key && 
                      cur_collision < max_collisions );
            
            element.value = std::move(temp_record.second);
            return temp_record != record_t{};
         }

         inline bool set(const elem_t& el, name payer) const {
            using namespace internal_use_do_not_use;

            static bucket_t temp_bucket = collision_policy.element_to_bucket(el);
            buffer_t buffer = pack_value(temp_bucket);
            auto wrote = db_store_i64( scope.value, table_name.value, payer.value, el.hashed_key, buffer.data, buffer.size );
            return wrote == buffer.size;
         }

         template <typename T>
         inline buffer_t pack_value(T&& v) const {
            buffer_t buffer(pack_size(v));
            datastream<char*> ds(buffer.data, buffer.size);
            ds << std::forward<T>(v);
            return std::move(buffer);
         }

      private:
         name                 owner;
         name                 scope;
         allocator_t          alloc;
         collision_policy_t   collision_policy;

      friend detail::iterator<self_t>;
   };

} // namespace eosio
