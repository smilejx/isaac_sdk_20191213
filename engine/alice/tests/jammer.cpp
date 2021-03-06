/*
Copyright (c) 2018, NVIDIA CORPORATION. All rights reserved.

NVIDIA CORPORATION and its licensors retain all intellectual property
and proprietary rights in and to this software, related documentation
and any modifications thereto. Any use, reproduction, disclosure or
distribution of this software and related documentation without an express
license agreement from NVIDIA CORPORATION is strictly prohibited.
*/
#include "engine/alice/alice.hpp"
#include "engine/alice/tests/foo_transmission.hpp"
#include "gtest/gtest.h"

namespace isaac {
namespace alice {

// A very fast task sends messages to a slow task. Both tick periodically.
TEST(MessagePassing, Test1) {
  Application app;
  // create publisher and subscriber nodes
  Node* pub_node = app.createMessageNode("pub");
  auto* pub = pub_node->addComponent<FooTransmitter>("transmitter");
  pub->async_set_tick_period("5ms");
  Node* sub_node = app.createMessageNode("sub");
  auto* sub = sub_node->addComponent<FooReceiver>("receiver");
  sub->async_set_tick_period("100ms");
  sub->async_set_count_tolerance(1000);
  Connect(pub->tx_foo(), sub->rx_foo());
  // run for a while
  app.startWaitStop(1.00);
  EXPECT_NEAR(pub->getTickCount(), 200, 20);
  EXPECT_NEAR(sub->getTickCount(), 10, 2);
}

}  // namespace alice
}  // namespace isaac
