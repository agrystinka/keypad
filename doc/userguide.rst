User guide
==========

It is about how to use Packet Sniffer.

**Work with packet-sniffer requires root privileges.**


Before using CLI, run the daemon. To do it, you could use:

``make run``  or ``./packet-sniffer``.


To interact with daemon packet-sniffer you could use following commands.

* ``./cli start`` - start writing sniffed packets into dump.txt.

* ``./cli stop`` - stop writing sniffed packets into dump.txt.

* ``./cli reset`` - stop writing sniffed packets and clean file dump.txt.

* ``./cli show -a`` - show all info stored into dump.txt.

* ``./cli show -i [IP address]`` - show how many packets from/to some IP address are stored in dump.txt.

Before using commands ``./cli show*`` make sure that you closed writing session by using command ``./cli stop``.

If you need some help, use  ``./cli --help`` or ``make runcli``.

While packet-sniffer works, it creates few files:

* ``dump.txt`` - here info about sniffed packets could be written, if you use command ``./cli start``.

* ``log.txt`` - here all daemon log messages are stored.

The PID of this daemon is in ``/var/run/packet-sniffer.pid``.

If you want to kill daemon packet-sniffer process, please use ``SIGTERM`` only.
