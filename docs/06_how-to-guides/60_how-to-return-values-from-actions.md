---
content_title: How-To Return Values From Action
---

## Overview

This how-to demonstrates how a smart contract developer implements return values from an action.

In order to accomplish this, use the `return` statement and pass the desired returned value, which can be of any C++ primitive type, a standard library type, or a user defined type.

## Before you begin

Make sure you have the following prerequisites in place:

* An Antelope development environment, for details consult the [Documentation Portal](https://docs.eosnetwork.com/docs/latest/).
* A smart contract, let’s call it `smrtcontract`, which builds without error.
* An action, let’s call it `checkwithrv`, from which you want to return a value of a user defined type `action_response`.

Refer to the following reference implementation for your starting point:

```cpp
struct action_response
{
  uint16_t id;
  std::pair<int, std::string> status;
};

class [[eosio::contract]] smrtcontract : public contract {
  public:
     using contract::contract;

     [[eosio::action]]
     action_response checkwithrv( name nm );
};
```

## Procedure

Complete the following steps to return an instance of `action_response` from the `checkwithrv` action:

1. Create an instance of the `action_response` C++ user defined structure.
2. Initialize its data members based on the action’s business logic.
3. Use `return` statement and pass as a parameter the instance created and initialized in previous steps.

```cpp
[[eosio::action]]
action_response smrtcontract::checkwithrv( name nm ) {
  print_f("Name : %\n", nm);

  // create instance of the action response structure
  action_response results;

  // initialize its data members
  results.id = 1;
  if (nm == "hello"_n) {
     results.status = {0, "Validation has passed."};
  }
  else {
     results.status = {1, "Input param `name` not equal to `hello`."};
  }
  
  // use return statement
  return results; // the `set_action_return_value` intrinsic is invoked automatically here
}
```

For a complete example of a smart contract that implements an action which returns a value see the [hello example contract](https://github.com/AntelopeIO/cdt/blob/main/examples/hello).

## Next Steps

* Compile the smart contract and deploy it to the Antelope testnet or any Antelope based blockchain.
* Use the `cleos` command to send the `checkwithrv` action to the smart contract and observe the returned value in the `cleos` output.
* Use other means (e.g. programmatically) to send the  `checkwithrv` action to the smart contract and observe the returned value in the action trace.

[[info | Returned values from actions availability]]
The action return values are only available to clients sending the action via the RPC API. Currently, there is no support for an inline action to be able to use the return value, because inline actions don't execute synchronously.

## Summary

In conclusion, the above instructions show how to return values from actions.
