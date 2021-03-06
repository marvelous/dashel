#include <dashel/dashel.h>
#include <iostream>
#include <cassert>

using namespace std;
using namespace Dashel;

class PingServer: public Hub
{
public:
	PingServer()
	{
		listenStream = connect("udp:port=8765");
	}

protected:
	Stream* listenStream;
	map<Stream*, string> nicks;
	
protected:
	void connectionCreated(Stream *stream) { }
	
	void incomingData(Stream *stream)
	{
		cerr << "new data....";
		PacketStream* packetStream = dynamic_cast<PacketStream*>(stream);
		assert(packetStream);
		IPV4Address source;
		
		packetStream->receive(source);
		cerr << "Ping from " << source.hostname() << ":" << source.port << ": ";
		
		char c;
		while (true)
		{
			packetStream->read(&c, 1);
			if (c)
				cerr << c;
			else
				break;
		}
		cerr << endl;
	}
	
	void connectionClosed(Stream *stream, bool abnormal) { }
};

class PingClient: public Hub
{
public:
	PingClient(const string& remoteTarget, const string& msg)
	{
		PacketStream* packetStream = dynamic_cast<PacketStream*>(connect("udp:port=8766"));
		assert(packetStream);
		
		packetStream->write(msg.c_str(), msg.length());
		char c = 0;
		packetStream->write(&c, 1);
		
		packetStream->send(IPV4Address(remoteTarget, 8765));
	}
	
protected:
	
	void connectionCreated(Stream *stream) { }
	
	void incomingData(Stream *stream) { }
	
	void connectionClosed(Stream *stream, bool abnormal) { }
};

int main(int argc, char* argv[])
{
	try
	{
		if (argc > 2)
		{
			PingClient client(argv[1], argv[2]);
		}
		else if (argc > 1)
		{
			PingClient client(argv[1], "default message, the other side does lack creativity");
		}
		else
		{
			PingServer().run();
		}
	}
	catch(DashelException e)
	{
		std::cerr << e.what() << std::endl;
	}
	
	return 0;
}
