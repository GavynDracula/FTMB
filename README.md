# FTMB
Reimplement FTMB(Fault Tolerance Middle-Box) for laboratory's research

Running Environment:

205(eno3 192.168.1.2) ------ (eno3 192.168.1.1)206(eno4 192.168.2.1) ------ (em3 192.168.2.2)210

Notice:
- The route and arp of machines is possible to be set manually
- 205 is master machine on which Master runs, 206 is switch machine where In/OutputLogger and StableStorage are deployed, 210 is backup machine with Backup
- Network Function in Master and Backup is a simple ICMP-ECHO packet counter now, just for fast development
- Reimplemented FTMB now is a pcap version, so don't forget to checkout to pcap branch

To-Do:
- Packet processing is based on pcap library currently, and will be improved by DPDK
- Communication between components is based on socket now, and will be replaced by ZeroMQ
