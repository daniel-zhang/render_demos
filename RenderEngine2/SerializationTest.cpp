#include "SerializationTest.h"

#include <fstream>
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/serialization/split_member.hpp>
#include <boost/serialization/vector.hpp>
#include <assert.h>

class VectorWrapper
{
public:
    VectorWrapper() { }
    bool operator==(VectorWrapper& other) const
    {
        if (rects.size() != other.rects.size())
        {
            return false;
        }
        for (UINT i = 0; i < rects.size(); ++i)
        {
            if (!(rects[i] == other.rects[i]))
            {
                return false;
            }
        }
        return true;
    }

    std::vector<SrcRect> rects;

    friend class boost::serialization::access;

    template<typename Archive>
    void save(Archive& ar, const unsigned version ) const
    {
        ar & rects;
    }

    template<typename Archive>
    void load(Archive& ar, const unsigned version)
    {
        ar & rects;
    }

    BOOST_SERIALIZATION_SPLIT_MEMBER()
};


SerializationTest::SerializationTest()
{

}

SerializationTest::~SerializationTest()
{

}

bool SerializationTest::init()
{
    if (DemoBasic::init() == false)
    {
        return false;
    }

    //////////////////////////////////////
    const char* fileName = "simple_data";
    const char* fileName2 = "vector_data";

    SrcRect r1(1,2,3,4);
    {
        std::ofstream ofs(fileName);
        boost::archive::text_oarchive ar(ofs);
        ar & r1;
    }

    SrcRect r2;

    assert(!(r1 == r2));

    {
        std::ifstream ifs(fileName);
        boost::archive::text_iarchive ar(ifs);
        ar & r2;
    }

    assert(r1==r2);
    //////////////////////////////////////

    VectorWrapper wrapper;
    wrapper.rects.push_back(SrcRect(1, 2, 3, 4));
    wrapper.rects.push_back(SrcRect(5, 6, 7, 8));
    {
        std::ofstream ofs(fileName2);
        boost::archive::text_oarchive ar(ofs);
        ar & wrapper;
    }

    VectorWrapper wrapper2;
    {
        std::ifstream ifs(fileName2);
        boost::archive::text_iarchive ar(ifs);
        ar & wrapper2;
    }
    assert(wrapper == wrapper2);
    

    //////////////////////////////////////
    return true;
}
