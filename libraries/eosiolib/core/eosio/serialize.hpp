#include <bluegrass/meta/preprocessor.hpp>

#define EOSLIB_REFLECT_MEMBER_OP( OP, elem ) \
  OP t.elem

/**
 *  @defgroup serialize Serialize
 *  @ingroup core
 *  @brief Defines C++ API to serialize and deserialize object
 */

/**
 *  Defines serialization and deserialization for a class
 *
 *  @ingroup serialize
 *  @param TYPE - the class to have its serialization and deserialization defined
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define EOSLIB_SERIALIZE( TYPE,  MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }

/**
 *  Defines serialization and deserialization for a class which inherits from other classes that
 *  have their serialization and deserialization defined
 *
 *  @ingroup serialize
 *  @param TYPE - the class to have its serialization and deserialization defined
 *  @param BASE - a sequence of base class names (basea)(baseb)(basec)
 *  @param MEMBERS - a sequence of member names.  (field1)(field2)(field3)
 */
#define EOSLIB_SERIALIZE_DERIVED( TYPE, BASE, MEMBERS ) \
 template<typename DataStream> \
 friend DataStream& operator << ( DataStream& ds, const TYPE& t ){ \
    ds << static_cast<const BASE&>(t); \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, <<, MEMBERS );\
 }\
 template<typename DataStream> \
 friend DataStream& operator >> ( DataStream& ds, TYPE& t ){ \
    ds >> static_cast<BASE&>(t); \
    return ds BLUEGRASS_META_FOREACH_SEQ( EOSLIB_REFLECT_MEMBER_OP, >>, MEMBERS );\
 }
