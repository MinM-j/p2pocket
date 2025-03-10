# Problem
Centralized file storage systems face significant limitations, including scalability challenges, maintenance complexities, and vulnerability to failures such as hardware issues and cyberattacks. They often become bottlenecks under heavy loads, leading to performance issues and inefficient resource allocation. Privacy and security risks are heightened due to data being stored in a single location, increasing the likelihood of breaches and unauthorized access. Additionally, concerns arise over the misuse of personal data, such as images, for AI training without consent, raising ethical and privacy issues

# About Project
P2Pocket is a conceptualized P2P storage sharing system, where users share their available storage to the network and uses storage resources of the network in need. The project
is heavily inspired from BitTorrent, a P2P file sharing system. The proposed system comes
with its own set of challenges and may not be an appropriate alternative to the existing
solutions. Therefore, through this project, we only aim to explore the practicality of P2P
system to address the ever growing need for supplementary storage.

Visit the [docs](./docs/)folder for further information about the projcet.

# running guide
compile the file and run the executable
enter `help` for help

run the tmux.sh from `build` directory: (need to give executable permission)

cls && g++ -std=c++20 main.cpp ./src/interface.cpp && ./a.out


