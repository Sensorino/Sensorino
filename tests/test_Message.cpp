/*
 * Compilation from within the Sensorino subdirectory:
 * g++ -isystem ../gmock/gmock-1.7.0/gtest/include/ -I ../gmock/gmock-1.7.0/gtest/  -isystem  ../gmock/gmock-1.7.0/include/ -I ../gmock -pthread -I ../libraries/RadioHead -I ../libraries/RadioHead/RHutil  -I . ../tests/test_Message.cpp Message.cpp Sensorino.cpp ../libraries/RadioHead/Dummy.cpp ../libraries/RadioHead/RHGenericDriver.cpp ../libraries/RadioHead/RHReliableDatagram.cpp ../libraries/RadioHead/RHDatagram.cpp ../libraries/RadioHead/RHutil/simulator.cpp  Service.cpp  ../gmock/libgmock.a  -o test
 *
 */

#include <Message.h>
#include <RHutil/simulator.h>
#include <gmock/gmock.h>

TEST(MessageTest, MsgIds) {

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


TEST(MessageTest, GetSet) {
    Message m=Message(255, 128);
    m.setId(25);
    EXPECT_EQ(m.getId(), 25);
}


TEST(MessageTest, AddGetValue) {
    Message m=Message(255, 128);

    m.addFloatValue(TEMPERATURE, 19.23);
    float value;
    m.find(TEMPERATURE, 0 , &value);
    std::cout<<"set "<<19.23<<" and got back "<<value<<"\n";
    EXPECT_FLOAT_EQ(value, 19.23);
}



TEST(MessageTest, AddTooManyValues) {

    EXPECT_EXIT(
        {
            Message m=Message(255, 128);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
            m.addFloatValue(TEMPERATURE, 19.23);
        },
        ::testing::ExitedWithCode(2),
        //".*constructing message bigger that max size.*"
        ""
    );


}




SerialSimulator Serial;

int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}

/* vim: set sw=4 ts=4 et: */

