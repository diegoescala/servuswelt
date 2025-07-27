# Servuswelt Router Bug: Route Matching Conflict

## Problem Description

The servuswelt router has a bug in its route matching algorithm when both parameterized and non-parameterized routes exist for the same base path.

## Symptoms

When registering these routes:
```cpp
Route(methods::GET, "/api/bank/accounts", getAccounts)
Route(methods::GET, "/api/bank/accounts/:id", getAccount)
Route(methods::GET, "/api/bank/transactions", getTransactions)  
Route(methods::GET, "/api/bank/transactions/:id", getTransaction)
```

The router incorrectly matches requests:
- `GET /api/bank/accounts` → matches `/api/bank/accounts/:id` with empty `:id` parameter
- `GET /api/bank/transactions` → matches `/api/bank/transactions/:id` with empty `:id` parameter

This causes the wrong handler to be invoked (`getAccount` instead of `getAccounts`, `getTransaction` instead of `getTransactions`).

## Expected Behavior

The router should match exact paths first before attempting parameterized matches:
- `GET /api/bank/accounts` → should match `/api/bank/accounts` exactly
- `GET /api/bank/accounts/123` → should match `/api/bank/accounts/:id` 

## Current Workaround

We've worked around this by modifying our route patterns to avoid the conflict:
```cpp
Route(methods::GET, "/api/bank/accounts/all", getAccounts)
Route(methods::GET, "/api/bank/accounts/:id", getAccount)
Route(methods::GET, "/api/bank/transactions/all", getTransactions)
Route(methods::GET, "/api/bank/transactions/:id", getTransaction)
```

## Suggested Fix

The route matching algorithm should prioritize exact matches over parameterized matches. Routes should be evaluated in order of specificity, with exact string matches taking precedence over parameter patterns.

## Impact

This bug prevents proper REST API design where collection endpoints (`/resource`) and individual resource endpoints (`/resource/:id`) coexist, which is a fundamental REST pattern.