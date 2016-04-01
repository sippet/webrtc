/*
 *  Copyright 2015 The WebRTC Project Authors. All rights reserved.
 *
 *  Use of this source code is governed by a BSD-style license
 *  that can be found in the LICENSE file in the root of the source
 *  tree. An additional intellectual property rights grant can be found
 *  in the file PATENTS.  All contributing project authors may
 *  be found in the AUTHORS file in the root of the source tree.
 */

#ifndef WEBRTC_P2P_STUNPROBER_STUNPROBER_H_
#define WEBRTC_P2P_STUNPROBER_STUNPROBER_H_

#include <set>
#include <string>
#include <vector>

#include "webrtc/base/asyncinvoker.h"
#include "webrtc/base/basictypes.h"
#include "webrtc/base/bytebuffer.h"
#include "webrtc/base/callback.h"
#include "webrtc/base/ipaddress.h"
#include "webrtc/base/network.h"
#include "webrtc/base/scoped_ptr.h"
#include "webrtc/base/socketaddress.h"
#include "webrtc/base/thread.h"
#include "webrtc/base/thread_checker.h"
#include "webrtc/typedefs.h"

namespace rtc {
class AsyncPacketSocket;
class PacketSocketFactory;
class Thread;
class NetworkManager;
}  // namespace rtc

namespace stunprober {

class StunProber;

static const int kMaxUdpBufferSize = 1200;

typedef rtc::Callback2<void, StunProber*, int> AsyncCallback;

enum NatType {
  NATTYPE_INVALID,
  NATTYPE_NONE,          // Not behind a NAT.
  NATTYPE_UNKNOWN,       // Behind a NAT but type can't be determine.
  NATTYPE_SYMMETRIC,     // Behind a symmetric NAT.
  NATTYPE_NON_SYMMETRIC  // Behind a non-symmetric NAT.
};

class StunProber : public sigslot::has_slots<> {
 public:
  enum Status {       // Used in UMA_HISTOGRAM_ENUMERATION.
    SUCCESS,          // Successfully received bytes from the server.
    GENERIC_FAILURE,  // Generic failure.
    RESOLVE_FAILED,   // Host resolution failed.
    WRITE_FAILED,     // Sending a message to the server failed.
    READ_FAILED,      // Reading the reply from the server failed.
  };

  struct Stats {
    Stats() {}

    int num_request_sent = 0;
    int num_response_received = 0;
    NatType nat_type = NATTYPE_INVALID;
    int average_rtt_ms = -1;
    int success_percent = 0;
    int target_request_interval_ns = 0;
    int actual_request_interval_ns = 0;

    // Also report whether this trial can't be considered truly as shared
    // mode. Share mode only makes sense when we have multiple IP resolved and
    // successfully probed.
    bool shared_socket_mode = false;

    std::string host_ip;

    // If the srflx_addrs has more than 1 element, the NAT is symmetric.
    std::set<std::string> srflx_addrs;
  };

  StunProber(rtc::PacketSocketFactory* socket_factory,
             rtc::Thread* thread,
             const rtc::NetworkManager::NetworkList& networks);
  virtual ~StunProber();

  // Begin performing the probe test against the |servers|. If
  // |shared_socket_mode| is false, each request will be done with a new socket.
  // Otherwise, a unique socket will be used for a single round of requests
  // against all resolved IPs. No single socket will be used against a given IP
  // more than once.  The interval of requests will be as close to the requested
  // inter-probe interval |stun_ta_interval_ms| as possible. After sending out
  // the last scheduled request, the probe will wait |timeout_ms| for request
  // responses and then call |finish_callback|.  |requests_per_ip| indicates how
  // many requests should be tried for each resolved IP address. In shared mode,
  // (the number of sockets to be created) equals to |requests_per_ip|. In
  // non-shared mode, (the number of sockets) equals to requests_per_ip * (the
  // number of resolved IP addresses).
  bool Start(const std::vector<rtc::SocketAddress>& servers,
             bool shared_socket_mode,
             int stun_ta_interval_ms,
             int requests_per_ip,
             int timeout_ms,
             const AsyncCallback finish_callback);

  // Method to retrieve the Stats once |finish_callback| is invoked. Returning
  // false when the result is inconclusive, for example, whether it's behind a
  // NAT or not.
  bool GetStats(Stats* stats) const;

 private:
  // A requester tracks the requests and responses from a single socket to many
  // STUN servers.
  class Requester;

  bool ResolveServerName(const rtc::SocketAddress& addr);
  void OnServerResolved(rtc::AsyncResolverInterface* resolver);

  void OnSocketReady(rtc::AsyncPacketSocket* socket,
                     const rtc::SocketAddress& addr);

  bool Done() {
    return num_request_sent_ >= requests_per_ip_ * all_servers_addrs_.size();
  }

  size_t total_socket_required() {
    return (shared_socket_mode_ ? 1 : all_servers_addrs_.size()) *
           requests_per_ip_;
  }

  bool SendNextRequest();

  // Will be invoked in 1ms intervals and schedule the next request from the
  // |current_requester_| if the time has passed for another request.
  void MaybeScheduleStunRequests();

  // End the probe with the given |status|.  Invokes |fininsh_callback|, which
  // may destroy the class.
  void End(StunProber::Status status);

  Requester* CreateRequester();

  Requester* current_requester_ = nullptr;

  // The time when the next request should go out.
  uint64 next_request_time_ms_ = 0;

  // Total requests sent so far.
  uint32 num_request_sent_ = 0;

  bool shared_socket_mode_ = false;

  // How many requests should be done against each resolved IP.
  uint32 requests_per_ip_ = 0;

  // Milliseconds to pause between each STUN request.
  int interval_ms_;

  // Timeout period after the last request is sent.
  int timeout_ms_;

  // STUN server name to be resolved.
  std::vector<rtc::SocketAddress> servers_;

  // Weak references.
  rtc::PacketSocketFactory* socket_factory_;
  rtc::Thread* thread_;

  // Accumulate all resolved addresses.
  std::vector<rtc::SocketAddress> all_servers_addrs_;

  // Caller-supplied callback executed when testing is completed, called by
  // End().
  AsyncCallback finished_callback_;

  // The set of STUN probe sockets and their state.
  std::vector<Requester*> requesters_;

  rtc::ThreadChecker thread_checker_;

  // Temporary storage for created sockets.
  std::vector<rtc::AsyncPacketSocket*> sockets_;
  // This tracks how many of the sockets are ready.
  size_t total_ready_sockets_ = 0;

  rtc::AsyncInvoker invoker_;

  rtc::NetworkManager::NetworkList networks_;

  RTC_DISALLOW_COPY_AND_ASSIGN(StunProber);
};

}  // namespace stunprober

#endif  // WEBRTC_P2P_STUNPROBER_STUNPROBER_H_