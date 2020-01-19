push: a DISC shell

Push is a dataflow shell which uses two new pipe operators, fan out('|<') and fanin('>|') to create Directed Acyclic Graphs of processes. Push was recently presented at Eurosys 2010, you can view the poster abstract here, the poster itself here and the pre-session presentation here.

This is the new unix port of push.

It should work on any unices supported by plan 9 port(a unix Plan 9 compatibility layer, it's available at http://swtch.com/plan9port/ please install it first).

We are currently working to port a few helper utilities which help show off push's capabilities.

To install, do the following on your unix compatible system. (The following installation instructions are a work in progress and should not be taken at face value. We would appreciate any input and experiences installing push) cd /usr/local wget http://swtch.com/plan9port/plan9port.tgz tar zxvf plan9port.tgz PLAN9=/usr/local/plan9 export PLAN9 PATH=${PATH}:$PLAN9/bin cd $OLDPWD # go back to this README's directory mk # plan9 make mk install

