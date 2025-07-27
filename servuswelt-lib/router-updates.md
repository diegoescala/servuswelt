# Servuswelt Router Updates

## Version: January 2025

### Major Fix: Multi-Segment Parameterized Route Matching

#### Problem
The servuswelt router had a critical bug in route matching when both parameterized and non-parameterized routes existed for the same base path. This prevented proper RESTful API design patterns.

**Symptoms:**
- `GET /api/clients/list` incorrectly matched `/api/clients/:client_id` instead of exact route
- Multi-segment routes like `/api/clients/:client_id/transactions` failed to work
- Routes with shared base paths conflicted with each other

**Root Cause:**
The router strips base paths from incoming requests before passing them to handlers. For example:
- Request to `/api/clients/list` → handler receives relative path `/`
- Request to `/api/clients/123/transactions` → handler receives `/123/transactions`

The original route matching logic tried to match full paths against relative paths, causing failures.

#### Solution Implemented

**1. Route Matching Algorithm**
Added comprehensive route matching with three new methods:

- **`findBestRoute()`** - Finds the best matching route based on specificity scoring
- **`matchRoute()`** - Performs exact pattern matching between request path and route pattern
- **`calculateSpecificity()`** - Assigns specificity scores (exact segments = 10 points, parameters = 1 point)

**2. Relative Path Conversion**
Routes are now converted to relative patterns for matching:
```cpp
// Examples:
// route="/api/clients/list", base_path="/api/clients/list" → relative_route="/"
// route="/api/clients/:client_id", base_path="/api/clients" → relative_route="/:client_id"  
// route="/api/clients/:client_id/transactions", base_path="/api/clients" → relative_route="/:client_id/transactions"
```

**3. Specificity-Based Route Selection**
The router now prioritizes exact matches over parameterized matches:
- `/api/clients/list` (specificity: 40) beats `/api/clients/:client_id` (specificity: 31)
- Routes are evaluated and the highest specificity match wins

#### Files Modified

**`router.h`**
- Added `RouteMatch` struct with route pointer, parameters map, and specificity score
- Added method declarations for `findBestRoute()`, `matchRoute()`, and `calculateSpecificity()`

**`router.cpp`**
- Replaced simple route delegation with proper route matching logic in `addRoute()`
- Implemented comprehensive route matching algorithm
- Added relative path conversion logic
- Added `<sstream>` include for string manipulation

#### API Impact

**Before Fix:**
```cpp
// These routes would conflict:
Route(methods::GET, "/api/clients/list", getClientsList)          // ❌ Failed
Route(methods::GET, "/api/clients/:client_id", getClient)         // ❌ Incorrectly matched
Route(methods::GET, "/api/clients/:client_id/transactions", ...)  // ❌ Never worked
```

**After Fix:**
```cpp
// All routes now work correctly:
Route(methods::GET, "/api/clients/list", getClientsList)          // ✅ Exact match
Route(methods::GET, "/api/clients/:client_id", getClient)         // ✅ Parameter match  
Route(methods::GET, "/api/clients/:client_id/transactions", ...)  // ✅ Multi-segment match
```

#### Testing Results

All route types now function correctly:
- `GET /api/clients/list` → Exact route matching ✅
- `GET /api/clients/123` → Single parameter extraction ✅
- `GET /api/clients/456/transactions` → Multi-segment parameter extraction ✅

The debug output shows proper route resolution:
```
DEBUG: Request path=/, base_path=/api/clients/list, method=GET
DEBUG: Matched route: /api/clients/list

DEBUG: Request path=/123, base_path=/api/clients, method=GET  
DEBUG: Matched route: /api/clients/:client_id

DEBUG: Request path=/456/transactions, base_path=/api/clients, method=GET
DEBUG: Matched route: /api/clients/:client_id/transactions
```

#### Backward Compatibility

This fix is **fully backward compatible**. Existing applications using servuswelt will continue to work without any code changes, but will now benefit from proper route resolution.

#### Performance Impact

Minimal performance impact. The route matching algorithm runs in O(n) time where n is the number of registered routes, and only executes once per request.

---

This update enables proper RESTful API design patterns where collection endpoints (`/resource`) and individual resource endpoints (`/resource/:id`) can coexist without conflicts, which is fundamental for modern web API development.