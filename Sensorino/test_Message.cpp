#include <Message.h>
#include <gmock/gmock.h>

// Tests factorial of positive numbers.
TEST(MessageTest, Misc) {

  uint8_t srcAddress=1;

  uint8_t dstAddress=2;

  Message(srcAddress, dstAddress);

  EXPECT_GT(Message(srcAddress, dstAddress).getId(), Message(srcAddress, dstAddress).getId());
  for (int i=0;i<300;i++){
    Message m=Message(srcAddress, dstAddress);
    EXPECT_GE(m.getId(),0);
    EXPECT_LT(m.getId(), 255);
  }
 

}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
