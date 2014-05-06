#include <Message.h>
#include <gmock/gmock.h>

// Tests factorial of positive numbers.
TEST(MessageTest, Misc) {

  uint8_t srcAddress[4];
  srcAddress[0]=1;
  srcAddress[1]=2;
  srcAddress[2]=3;
  srcAddress[3]=4;

  uint8_t dstAddress[4];
  dstAddress[0]=1;
  dstAddress[1]=2;
  dstAddress[2]=3;
  dstAddress[3]=4;

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
