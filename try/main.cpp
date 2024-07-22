/**
 *  Select GCC-11 kit or newer
 *  Create build folder $: mkdir -p /workspaces/signal/build
 *  Do build $: cd /workspaces/signal/build && cmake ../ && make
 *  Execute $: cd /workspaces/signal/build/try && ./signal
 */

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