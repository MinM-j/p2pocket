#include <timer.h>

namespace kademlia{
void timer::reset()
{
  m_beg = clock::now();
}

double timer::elapsed() const
{
  return std::chrono::duration_cast<Second>(clock::now() - m_beg).count();
}
}//namespace kademlia
