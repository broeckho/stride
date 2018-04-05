#include "myhayai/MainRunner.hpp"
#include "myhayai/Benchmarker.hpp"
#include "myhayai/TestFactoryDefault.hpp"

#include <cstddef>
#include <iostream>
#include <unistd.h>

using namespace std;
using namespace myhayai;



inline void msleep(unsigned int duration)
{
        usleep(duration * 1000);
}

/// Delivery man.
class DeliveryMan
{
public:
        /// Initialize a delivery man instance.
        /// @param speed Delivery man speed from 1 to 10.
        explicit DeliveryMan(std::size_t speed) :   _speed(speed) {}

        /// Deliver a package.
        /// @param distance Distance the package has to travel.
        void DeliverPackage(std::size_t distance)
        {
                // Waste some clock cycles here.
                std::size_t largeNumber = 10000u * distance / _speed;
                volatile std::size_t targetNumber;
                while (largeNumber--)
                        targetNumber = largeNumber;
        }

private:
        std::size_t _speed; ///< Delivery man speed from 1 to 10.
};

template<unsigned int N>
class SleepTest : public Fixture
{
public:
        void TestBody() override { msleep(N); }
};


class PackageTest : public Fixture
{
public:
        void TestBody() override { DeliveryMan(10).DeliverPackage(100); }
};

class SlowDeliveryManFixture : public Fixture
{
public:
        SlowDeliveryManFixture() : SlowDeliveryMan(nullptr) {}
        void SetUp() override { this->SlowDeliveryMan = new DeliveryMan(1); }
        void TearDown() override { delete this->SlowDeliveryMan; }
        void DoThis(unsigned int m, unsigned int n) { msleep(m); SlowDeliveryMan->DeliverPackage(n); }
        void DoThat(unsigned int n) { SlowDeliveryMan->DeliverPackage(2*n); }

        DeliveryMan* SlowDeliveryMan;
};

class Do1Test : public SlowDeliveryManFixture
{
public:
        void TestBody() override { DoThis(3, 4); }
};

class Do2Test : public SlowDeliveryManFixture
{
public:
        void TestBody() override { DoThat(8); }
};



class ParamDeliveryManFixture : public Fixture
{
public:
        ParamDeliveryManFixture() : SlowDeliveryMan(nullptr) {}
        void SetUp() override { this->SlowDeliveryMan = new DeliveryMan(1); }
        void TearDown() override { delete this->SlowDeliveryMan; }
        void DoThis(unsigned int m, unsigned int n) { msleep(m); SlowDeliveryMan->DeliverPackage(n); }
        void DoThat(unsigned int n) { SlowDeliveryMan->DeliverPackage(2*n); }

        DeliveryMan* SlowDeliveryMan;
};

class ParamTest : public Fixture
{
public:
        ParamTest() : SlowDeliveryMan(nullptr) {}
        ~ParamTest() override = default;
        void SetUp() override { this->SlowDeliveryMan = new DeliveryMan(1); }
        void TearDown() override { delete this->SlowDeliveryMan; }
        DeliveryMan* SlowDeliveryMan;

protected:
        void TestPayload(std::size_t duration, std::size_t distance)
        {
                msleep(duration);
                SlowDeliveryMan->DeliverPackage(distance);
        }
};

struct P1 : public ParamTest { void TestBody() override { this->TestPayload(1, 10); }};
struct P2 : public ParamTest { void TestBody() override { this->TestPayload(7, 3); }};

int main(int argc, char** argv)
{
        Benchmarker::RegisterTest("Sleeper", "Sleep1ms", 5, new TestFactoryDefault<SleepTest<1>>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("Sleeper", "Sleep6ms", 7, new TestFactoryDefault<SleepTest<6>>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("Delivery", "Delivery100", 10, new TestFactoryDefault<PackageTest>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("DoTest", "Do1", 10, new TestFactoryDefault<Do1Test>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("DoTest", "Do2", 5, new TestFactoryDefault<Do2Test>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("ParameTest", "P1", 5, new TestFactoryDefault<P1>(),
                                  TestParametersDescriptor("(std::size_t duration, std::size_t distance)", "(1, 10)"));
        Benchmarker::RegisterTest("ParameTest", "P2", 5, new TestFactoryDefault<P2>(),
                                  TestParametersDescriptor("(std::size_t duration, std::size_t distance)", "(7, 3)"));
        // Set up the main runner.
        MainRunner runner;

        // Parse the arguments.
        int result = runner.ParseArgs(argc, argv);
        if (result) {
                return result;
        }

        // Execute based on the selected mode.
        return runner.Run();
}

