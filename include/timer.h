#include <chrono> // for std::chrono functions

namespace kademlia{
class timer
{
private:
	// Type aliases to make accessing nested type easier
	using clock = std::chrono::steady_clock;
	using Second = std::chrono::duration<double, std::ratio<1> >;

	std::chrono::time_point<clock> m_beg { clock::now() };

public:
	void reset();

	double elapsed() const;

	
};
}//namespace kademlia
