---
content_title: How to insert data into a multi-index table
---

## Overview

This guide provides instructions to insert data into a multi-index table.

## Reference

See the following code reference:

* The [`multi-index`](/reference/Classes/classeosio_1_1multi__index) class.
* The [`multi-index::find(...)`](/reference/Modules/group__multiindex#function-find) method.
* The [`multi-index::emplace(...)`](/reference/Modules/group__multiindex#function-emplace) method.

## Before you begin

Make sure you have the following prerequisites in place:

* An Antelope development environment, for details consult the [Documentation Portal](https://docs.eosnetwork.com/docs/latest/).
* A multi-index `testab` table instance which stores `user` objects indexed by the primary key which is of type `eosio::name`. Consult the section [How to instantiate a multi-index table](./how-to-instantiate-a-multi-index-table.md) to learn how to set it up.

## Procedure

Complete the following steps to insert an user object in the `testtab` multi-index table:

### 1. Verify If The User Already Exists

Use of the multi-index table iterator to find out if the user object already exists. The targeted user is searched based on its account name.

```cpp
[[eosio::action]] void multi_index_example::set( name user ) {
  // check if the user already exists
  auto itr = testtab.find(user.value);
}
```

### 2. Insert The User If Not Found In Table

Use the [`emplace`](/reference/Modules/group__multiindex#function-emplace) method to make the insertion if the user object is not already in the multi-index table. Otherwise print an informational message.

```diff
[[eosio::action]] void multi_index_example::set( name user ) {
  // check if the user already exists
  auto itr = testtab.find(user.value);

+  if ( itr == testtab.end() ) {
+    testtab.emplace( _self, [&]( auto& u ) {
+      u.test_primary = user;
+      u.secondary = "second"_n;
+      u.datum = 0;
+    });
+  }
+  else {
+    printf("User already exists.");
+  }
}
```

[[info | Full example location]]
| A full example project demonstrating the instantiation and usage of multi-index table can be found [here](https://github.com/AntelopeIO/cdt/blob/main/examples/multi_index_example).

## Summary

In conclusion, the above instructions show how to insert data in a multi-index table.

## Next Steps

* You can [iterate and retrieve newly inserted data](./how-to-iterate-and-retrieve-a-multi_index-table) from the multi-index table.
