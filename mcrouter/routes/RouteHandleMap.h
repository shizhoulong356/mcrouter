/**
 *  Copyright (c) 2014, Facebook, Inc.
 *  All rights reserved.
 *
 *  This source code is licensed under the BSD-style license found in the
 *  LICENSE file in the root directory of this source tree. An additional grant
 *  of patent rights can be found in the PATENTS file in the same directory.
 */
#pragma once

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <folly/Range.h>

#include "mcrouter/config.h"
#include "mcrouter/routes/McrouterRouteHandle.h"
#include "mcrouter/routes/RouteSelectorMap.h"

namespace facebook { namespace memcache { namespace mcrouter {

class RoutePolicyMap;

/* A bridge between proxy_* config structures and corresponding route handles.
   It is initialized by RouteHandleMapBuilder. */
class RouteHandleMap {
 public:
  RouteHandleMap(const RouteSelectorMap& routeSelectors,
                 std::string defaultRoute,
                 bool sendInvalidRouteToDefault);

  const std::vector<McrouterRouteHandlePtr>& getTargetsForKey(
    folly::StringPiece prefix,
    folly::StringPiece key) const;

 private:
  std::string defaultRoute_;
  bool sendInvalidRouteToDefault_;

  const std::vector<McrouterRouteHandlePtr> emptyV_;

  std::shared_ptr<RoutePolicyMap> allRoutes_;
  StringKeyedUnorderedMap<std::shared_ptr<RoutePolicyMap>> byRegion_;
  StringKeyedUnorderedMap<std::shared_ptr<RoutePolicyMap>> byRoute_;

  const std::vector<McrouterRouteHandlePtr>& getBySingleRoute(
    folly::StringPiece route,
    folly::StringPiece key) const;

  void foreachRoutePolicy(folly::StringPiece prefix,
    std::function<void(const std::shared_ptr<RoutePolicyMap>&)> f) const;

  /**
   * @return pointer to a precalculated vector of route handles that a request
   * with the given prefix and key should be forwarded to. nullptr if vector for
   * this prefix wasn't precalculated.
   */
  const std::vector<McrouterRouteHandlePtr>*
  getTargetsForKeyFast(folly::StringPiece prefix,
                       folly::StringPiece key) const;

  /**
   * @return A vector of route handles that a request with
   * the given prefix and key should be forwarded to. Works for
   * prefixes with arbitrary amount of '*'
   */
  std::vector<McrouterRouteHandlePtr> getTargetsForKeySlow(
    folly::StringPiece prefix,
    folly::StringPiece key) const;
};

}}}  // facebook::memcache::mcrouter
