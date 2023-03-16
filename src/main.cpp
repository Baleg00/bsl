#include <iostream>
#include <iomanip>

#include "Net.hpp"

int main()
{
	using namespace bsl;
	
	net::setup();

	net::AddrIPv4 addr = net::resolve("www.google.com", "http").expect().to_ipv4();
	net::SockAddrV4 sock_addr{ addr, 80 };

	net::Socket sock{
		net::AddrFamily::IPv4,
		net::SockType::STREAM,
		net::Proto::TCP
	};

	sock.connect(sock_addr).expect_and_discard();

	const u8 send_buffer[] = "HEAD / HTTP/1.1\n\n";

	usize bytes_sent = sock.send(send_buffer, 17).expect();

	std::cout << "No. bytes sent: " << bytes_sent << "\n";
	std::cout << ">>>>> REQUEST BEGIN >>>>>\n";
	std::cout << send_buffer;
	std::cout << "<<<<< REQUEST END <<<<<\n";

	u8 recv_buffer[2048];

	usize bytes_received = sock.recv(recv_buffer, sizeof(recv_buffer)).expect();

	std::cout << "No. bytes received: " << bytes_received << "\n";

	std::cout << ">>>>> RESPONSE BEGIN >>>>>\n";
	std::cout.write(reinterpret_cast<const char*>(recv_buffer), bytes_received);
	std::cout << "<<<<< RESPONSE END <<<<<\n";

	sock.close().expect_and_discard();

	net::cleanup();

	return 0;
}
