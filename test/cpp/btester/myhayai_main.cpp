/*
 *  This is free software: you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  any later version.
 *  The software is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *  You should have received a copy of the GNU General Public License
 *  along with the software. If not, see <http://www.gnu.org/licenses/>.
 *
 *  Copyright 2018, Kuylen E, Willem L, Broeckhove J
 *
 *  This software has been altered form the hayai software by Nick Bruun.
 *  The original copyright, to be found in the directory two levels higher
 *  still aplies.
 */
/**
 * @file
 * Header file for TestFactory.
 */

#include "myhayai/Benchmarker.hpp"
#include "myhayai/MainRunner.hpp"
#include "myhayai/TestFactoryDefault.hpp"

#include <cstddef>
#include <iostream>
#include <memory>
#include <unistd.h>

using namespace std;
using namespace myhayai;

inline void msleep(unsigned int duration) { usleep(duration * 1000); }

/// Delivery man.
class DeliveryMan
{
public:
        /// Initialize a delivery man instance.
        /// @param speed Delivery man speed from 1 to 10.
        explicit DeliveryMan(std::size_t speed) : _speed(speed) {}

        /// Deliver a package.
        /// @param distance Distance the package has to travel.
        void DeliverPackage(std::size_t distance)
        {
                // Waste some clock cycles here.
                std::size_t          largeNumber = 10000u * distance / _speed;
                volatile std::size_t targetNumber;
                while (largeNumber--)
                        targetNumber = largeNumber;
        }

        void Prepare(std::size_t number)
        {
                // Waste some clock cycles here.
                std::size_t          largeNumber = 10000u * number / _speed;
                volatile std::size_t targetNumber;
                while (largeNumber--)
                        targetNumber = largeNumber;
        }

private:
        std::size_t _speed; ///< Delivery man speed from 1 to 10.
};
/*
template <unsigned int N>
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
        void DoThis(unsigned int m, unsigned int n)
        {
                msleep(m);
                SlowDeliveryMan->DeliverPackage(n);
        }
        void DoThat(unsigned int n) { SlowDeliveryMan->DeliverPackage(2 * n); }

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
        void DoThis(unsigned int m, unsigned int n)
        {
                msleep(m);
                SlowDeliveryMan->DeliverPackage(n);
        }
        void DoThat(unsigned int n) { SlowDeliveryMan->DeliverPackage(2 * n); }

        DeliveryMan* SlowDeliveryMan;
};

class ParamTest : public Fixture
{
public:
        ParamTest() : SlowDeliveryMan(nullptr) {}
        ~ParamTest() override = default;
        void         SetUp() override { this->SlowDeliveryMan = new DeliveryMan(1); }
        void         TearDown() override { delete this->SlowDeliveryMan; }
        DeliveryMan* SlowDeliveryMan;

protected:
        void TestPayload(std::size_t duration, std::size_t distance)
        {
                msleep(duration);
                SlowDeliveryMan->DeliverPackage(distance);
        }
};

struct P1 : public ParamTest
{
        void TestBody() override { this->TestPayload(1, 10); }
};
struct P2 : public ParamTest
{
        void TestBody() override { this->TestPayload(7, 3); }
};
*/

template<unsigned int N>
class SleepTestFactory : public TestFactory
{
public:
        Fixture* CreateTest()
        {
                return new Fixture([](){msleep(N);});
        }
};

class ParamTestFactory : public TestFactory
{
public:
        ParamTestFactory(size_t distance, size_t duration) : m_distance(distance), m_duration(duration) {}
        Fixture* CreateTest() override
        {
                auto p = make_shared<DeliveryMan>(1);
                auto body = [p, this](){msleep(m_duration); p->DeliverPackage(m_distance);};
                return new Fixture(body);

        }
private:
        size_t m_distance;
        size_t m_duration;
};
// Notice no inheritance, so the signatures of SetUp, Run, TearDown are not
// restricted, nor indeed their names.
class VerySlowDeliveryManFixture
{
public:
        VerySlowDeliveryManFixture() : m_man(nullptr) {}
        void SetUp(unsigned int speed) { this->m_man = new DeliveryMan(speed); }
        void TearDown() { delete this->m_man; }
        void DoThis(unsigned int m, unsigned int n)
        {
                msleep(m);
                m_man->DeliverPackage(n);
        }
        void DoThat(unsigned int n) { m_man->DeliverPackage(2 * n); }

        DeliveryMan* m_man;
};

class Param2TestFactory : public TestFactory
{
public:
        Param2TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
                : m_distance(distance), m_duration(duration), m_speed(speed){}
        Fixture* CreateTest() override
        {
                auto f = make_shared<VerySlowDeliveryManFixture>();
                auto body = [f, this](){f->DoThis(m_duration, m_distance);};
                auto setup = [f, this](){f->SetUp(m_speed);};
                auto teardown = [f](){f->TearDown();};
                return new Fixture(body, setup, teardown);

        }
private:
        unsigned int m_distance;
        unsigned int m_duration;
        unsigned int m_speed;
};


class Param3TestFactory : public TestFactory
{
public:
        Param3TestFactory(unsigned int distance, unsigned int duration, unsigned int speed)
                : m_distance(distance), m_duration(duration), m_speed(speed){}
        Fixture* CreateTest() override
        {
                auto f = make_shared<VerySlowDeliveryManFixture>();
                auto body = bind(&VerySlowDeliveryManFixture::DoThis, f, m_duration, m_distance);
                auto setup = bind(&VerySlowDeliveryManFixture::SetUp, f, m_speed);
                auto teardown = bind(&VerySlowDeliveryManFixture::TearDown, f);
                return new Fixture(body, setup, teardown);
        }
private:
        unsigned int m_distance;
        unsigned int m_duration;
        unsigned int m_speed;
};

class Param4TestFactory : public TestFactory
{
public:
        Param4TestFactory(unsigned int distance, unsigned int duration, unsigned int number, unsigned int speed)
                : m_distance(distance), m_duration(duration), m_number(number), m_speed(speed){}
        Fixture* CreateTest() override
        {
                auto p = make_shared<DeliveryMan>(m_speed);
                auto body = bind(&DeliveryMan::Prepare, p, m_number);
                auto setup = [p, this](){ msleep(m_duration); bind(&DeliveryMan::Prepare, p, m_distance);};
                return new Fixture(body, setup);
        }
private:
        unsigned int m_distance;
        unsigned int m_duration;
        unsigned int m_number;
        unsigned int m_speed;
};


int main(int argc, char** argv)
{


        /*
        Benchmarker::RegisterTest("Sleeper", "Sleep6ms", 7, new TestFactoryDefault<SleepTest<6>>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("Delivery", "Delivery100", 10, new TestFactoryDefault<PackageTest>(),
                                  TestParametersDescriptor());
        Benchmarker::RegisterTest("DoTest", "Do1", 10, new TestFactoryDefault<Do1Test>(), TestParametersDescriptor());
        Benchmarker::RegisterTest("DoTest", "Do2", 5, new TestFactoryDefault<Do2Test>(), TestParametersDescriptor());
        Benchmarker::RegisterTest("ParameTest", "P1", 5, new TestFactoryDefault<P1>(),
                                  TestParametersDescriptor("(std::size_t duration, std::size_t distance)", "(1, 10)"));
        Benchmarker::RegisterTest("ParameTest", "P2", 5, new TestFactoryDefault<P2>(),
                                  TestParametersDescriptor("(std::size_t duration, std::size_t distance)", "(7, 3)"));
        */

        Benchmarker::RegisterTest("Sleeper", "Sleep1ms", 5, new SleepTestFactory<10>);

        Benchmarker::RegisterTest("Delivery", "Param with distance=12,duration=10", 5, new ParamTestFactory(12, 10));
        Benchmarker::RegisterTest("Delivery", "Param with distance=5,duration=8", 5, new ParamTestFactory(5, 8));

        Benchmarker::RegisterTest("Delivery", "Param2 with distance=5,duration=8, speed=3", 10,
                                  new Param2TestFactory(5, 8, 3));

        Benchmarker::RegisterTest("Delivery", "Param3 with distance=5, duration=8, speed=3", 10,
                                  new Param3TestFactory(5, 8, 3));

        Benchmarker::RegisterTest("Delivery", "Param4 with distance=5, duration=8, number=2, speed=3", 10,
                                  new Param4TestFactory(5, 8, 10, 3));

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
