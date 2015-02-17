/* Copyright Institute of Sound and Vibration Research - All rights reserved */

#include "listener_position.hpp"
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>

namespace visr
{
namespace pml
{

ListenerPosition::ListenerPosition()
	: mX( 0.0f )
	, mY( 0.0f )
	, mZ( 0.0f )
	, mTimeNS( 0)
	, mFaceID( 0 )
{}

void ListenerPosition::parse(std::istream &  inputStream)
{
	namespace pt = boost::property_tree;

	pt::ptree tree;
	pt::read_json( inputStream, tree);
	// std::cout << "--------\nFirst layer of JSON structure outputting using a for loop:" << endl;
	//for (auto&& kv : tree)
	//{
	//	cout << kv.first << " " << kv.second.get_value<string>() << endl;
	//}
	//cout << "-------\nOutputting using specific requests:" << endl;
	//cout << "nTime=" << tree.get<uint64_t>("nTime") << "\tin units of 100-nanosecond" << endl;
	//cout << "iFace=" << tree.get<int>("iFace") << endl;

	mTimeNS = tree.get<std::uint64_t>("nTime") * 100;
	mFaceID = tree.get<int>("iFace");
	mX = tree.get<float>("headJoint.X");
	mY = tree.get<float>("headJoint.Y");
	mZ = tree.get<float>("headJoint.Z");

	//cout << "headJoint.X=" << tree.get<float>("headJoint.X") << endl;
	//cout << "headJoint.Y=" << tree.get<float>("headJoint.Y") << endl;
	//cout << "headJoint.Z=" << tree.get<float>("headJoint.Z") << endl;
}

std::ostream & operator<<(std::ostream & stream, const ListenerPosition & pos)
{
	stream << "(" << pos.x() << ", " << pos.y() << ", " << pos.z() << ")";
	return stream;
}


} // namespace pml
} // namespace visr
