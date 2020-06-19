# Signal - C++20 Signals & Slots #

Header-only library. Work in progress.

## Hello world! ##

```cpp
#include "signal.hpp"
#include <iostream>

struct tag{};
using SayHello = signal::id<tag, std::string>;

int main()
{
    signal::emitter <SayHello> emitter;
    signal::receiver<SayHello> receiver;

    emitter.connect<SayHello>(receiver, 
        [](std::string name) {
            std::cout << "Hello " << name << "!\n"; 
        }
    );

    emitter.emit<SayHello>("world");
    receiver.receive<SayHello>();

    return 0;
}
```

## v1.0 Milestones ##
+ ~~Base implementation~~
+ Finish documentation
+ Implement tests
+ Patch unnecessary data copies
+ Add multi-threading support
+ Implement move semantics

## Mirrors ##
+ [github](https://github.com/aviana37/signal)
