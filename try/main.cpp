/**
 *  Select GCC-11 kit or newer
 *  Build via cli or $: mkdir -p ${CODESPACE_VSCODE_FOLDER}/build && cd ${CODESPACE_VSCODE_FOLDER}/build && cmake ../ && make
 *  Execute on the terminal $: cd ${CODESPACE_VSCODE_FOLDER}/build/try && ./signal
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