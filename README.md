# Signal - C++20 Signals & Slots #

Header-only library. Work in progress.

## Hello world! ##

```cpp
#include "signal.hpp"
#include <iostream>

using SayHello = signal::id<std::string>;

int main()
{
    signal::emitter <SayHello> emitter;
    signal::receiver<SayHello> receiver;

    emitter.connect <SayHello>(receiver);
    emitter.emit<SayHello>("world");

    receiver.bind<SayHello>(
        [](std::string name) {
            std::cout << "Hello " << name << "!\n"; 
        });
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
+ [bitbucket](https://bitbucket.org/aviana37/signal)
+ [github](https://github.com/aviana37/signal)
