//
// Copyright Aliaksei Levin (levlam@telegram.org), Arseny Smirnov (arseny30@gmail.com) 2014-2023
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

//https://raw.githubusercontent.com/tdlib/td/master/example/cpp/td_example.cpp

#pragma once

#include <cstdint>
#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <td/telegram/Client.h>
#include <td/telegram/td_api.h>
#include <td/telegram/td_api.hpp>

// Simple single-threaded example of TDLib usage.
// Real world programs should use separate thread for the user input.
// Example includes user authentication, receiving updates, getting chat list and sending text messages.

// overloaded
namespace td_detail {

template <class... Fs>
struct overload;

template <class F>
struct overload<F> : public F {
  explicit overload(F f) : F(f) {
  }
};

template <class F, class... Fs>
struct overload<F, Fs...>
    : public overload<F>
    , public overload<Fs...> {
  overload(F f, Fs... fs) : overload<F>(f), overload<Fs...>(fs...) {
  }
  using overload<F>::operator();
  using overload<Fs...>::operator();
};

}  // namespace td_detail

template <class... F>
auto overloaded(F... f) {
  return td_detail::overload<F...>(f...);
}

namespace td_api = td::td_api;

class TdExample {
 public:

  TdExample();

  void loop();

 private:
  using Object = td_api::object_ptr<td_api::Object>;
  std::unique_ptr<td::ClientManager> client_manager_;
  std::int32_t client_id_{0};

  td_api::object_ptr<td_api::AuthorizationState> authorization_state_;
  bool are_authorized_{false};
  bool need_restart_{false};
  std::uint64_t current_query_id_{0};
  std::uint64_t authentication_query_id_{0};

  std::map<std::uint64_t, std::function<void(Object)>> handlers_;

  std::map<std::int64_t, td_api::object_ptr<td_api::user>> users_;

  std::map<std::int64_t, std::string> chat_title_;

  void restart();

  void send_query(td_api::object_ptr<td_api::Function> f, std::function<void(Object)> handler);

  void process_response(td::ClientManager::Response response);

  std::string get_user_name(std::int64_t user_id) const;

  std::string get_chat_title(std::int64_t chat_id) const;

  void process_update(td_api::object_ptr<td_api::Object> update);

  auto create_authentication_query_handler();

  void on_authorization_state_update();

  void check_authentication_error(Object object);

  std::uint64_t next_query_id();
};

//int main() {
//  TdExample example;
//  example.loop();
//}

