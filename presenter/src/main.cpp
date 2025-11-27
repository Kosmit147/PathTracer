#include <tracer/core.hpp>

#include "log.hpp"

auto main() -> int
{
    PT_INFO("{}", tracer::test());
}
