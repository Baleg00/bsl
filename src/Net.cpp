#include "Net.hpp"

//#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <WinSock2.h>
#include <WS2tcpip.h>

#pragma comment(lib, "ws2_32.lib")

namespace bsl::net
{
	void setup()
	{
		WSADATA data;

		int result = WSAStartup(MAKEWORD(2, 2), &data);

		if (result != 0)
			throw NetError{};
	}

	void cleanup()
	{
		int result = WSACleanup();

		if (result != 0)
			throw NetError{};
	}

	const AddrIPv4 AddrIPv4::LOCALHOST = AddrIPv4{ 127, 0, 0, 0 };
	const AddrIPv4 AddrIPv4::UNSPECIFIED = AddrIPv4{ 0, 0, 0, 0 };
	const AddrIPv4 AddrIPv4::BROADCAST = AddrIPv4{ 255, 255, 255, 255 };

	const AddrIPv6 AddrIPv6::LOCALHOST = AddrIPv6{ 0, 0, 0, 0, 0, 0, 0, 1 };
	const AddrIPv6 AddrIPv6::UNSPECIFIED = AddrIPv6{ 0, 0, 0, 0, 0, 0, 0, 0 };

	Result<IPAddr, HostnameResolutionError> resolve(const char* hostname, const char* service)
	{
		int result;
		::ADDRINFO* addr_info;

		result = ::getaddrinfo(
			reinterpret_cast<::PCSTR>(hostname), 
			reinterpret_cast<::PCSTR>(service),
			NULL, 
			&addr_info);

		if (result != 0)
			return HostnameResolutionError{};

		for (::ADDRINFO* it = addr_info; it != NULL; it = it->ai_next)
		{
			switch (it->ai_family)
			{
			case AF_INET:
			{
				::PSOCKADDR_IN addr = reinterpret_cast<::PSOCKADDR_IN>(it->ai_addr);

				IPAddr ip_addr{
					AddrIPv4{
						static_cast<u8>(addr->sin_addr.S_un.S_un_b.s_b1),
						static_cast<u8>(addr->sin_addr.S_un.S_un_b.s_b2),
						static_cast<u8>(addr->sin_addr.S_un.S_un_b.s_b3),
						static_cast<u8>(addr->sin_addr.S_un.S_un_b.s_b4)
					}
				};
				
				::freeaddrinfo(addr_info);

				return move(ip_addr);
			}
			case AF_INET6:
			{
				::LPSOCKADDR_IN6 addr = reinterpret_cast<::LPSOCKADDR_IN6>(it->ai_addr);

				IPAddr ip_addr{
					AddrIPv6{
						static_cast<u16>(addr->sin6_addr.u.Word[0]),
						static_cast<u16>(addr->sin6_addr.u.Word[1]),
						static_cast<u16>(addr->sin6_addr.u.Word[2]),
						static_cast<u16>(addr->sin6_addr.u.Word[3]),
						static_cast<u16>(addr->sin6_addr.u.Word[4]),
						static_cast<u16>(addr->sin6_addr.u.Word[5]),
						static_cast<u16>(addr->sin6_addr.u.Word[6]),
						static_cast<u16>(addr->sin6_addr.u.Word[7])
					}
				};

				::freeaddrinfo(addr_info);

				return move(ip_addr);
			}
			}
		}

		return HostnameResolutionError{};
	}

	struct _NativeSockAddr
	{
		::SOCKADDR_STORAGE m_sock_addr;
	};

	_NativeSockAddr SockAddrV4::to_native() const noexcept
	{
		_NativeSockAddr result;
		ZeroMemory(&result.m_sock_addr, sizeof(result.m_sock_addr));

		::PSOCKADDR_IN native_sock_addr = reinterpret_cast<::PSOCKADDR_IN>(&result.m_sock_addr);

		native_sock_addr->sin_family = AF_INET;

		native_sock_addr->sin_addr.S_un.S_un_b.s_b1 = m_addr[0];
		native_sock_addr->sin_addr.S_un.S_un_b.s_b2 = m_addr[1];
		native_sock_addr->sin_addr.S_un.S_un_b.s_b3 = m_addr[2];
		native_sock_addr->sin_addr.S_un.S_un_b.s_b4 = m_addr[3];

		native_sock_addr->sin_port = ::htons(static_cast<::u_short>(m_port));

		return result;
	}

	Maybe<SockAddrV4> SockAddrV4::from_native(const _NativeSockAddr& native) noexcept
	{
		if (native.m_sock_addr.ss_family != AF_INET)
			return {};

		const ::SOCKADDR_IN* native_sock_addr = reinterpret_cast<const ::SOCKADDR_IN*>(&native.m_sock_addr);

		return SockAddrV4{
			AddrIPv4{
				static_cast<u8>(native_sock_addr->sin_addr.S_un.S_un_b.s_b1),
				static_cast<u8>(native_sock_addr->sin_addr.S_un.S_un_b.s_b2),
				static_cast<u8>(native_sock_addr->sin_addr.S_un.S_un_b.s_b3),
				static_cast<u8>(native_sock_addr->sin_addr.S_un.S_un_b.s_b4)
			},
			::ntohs(native_sock_addr->sin_port)
		};
	}

	_NativeSockAddr SockAddrV6::to_native() const noexcept
	{
		_NativeSockAddr result;
		ZeroMemory(&result.m_sock_addr, sizeof(result.m_sock_addr));

		::PSOCKADDR_IN6 native_sock_addr = reinterpret_cast<::PSOCKADDR_IN6>(&result.m_sock_addr);

		native_sock_addr->sin6_family = AF_INET6;

		native_sock_addr->sin6_addr.u.Word[0] = ::htons(m_addr[0]);
		native_sock_addr->sin6_addr.u.Word[1] = ::htons(m_addr[1]);
		native_sock_addr->sin6_addr.u.Word[2] = ::htons(m_addr[2]);
		native_sock_addr->sin6_addr.u.Word[3] = ::htons(m_addr[3]);
		native_sock_addr->sin6_addr.u.Word[4] = ::htons(m_addr[4]);
		native_sock_addr->sin6_addr.u.Word[5] = ::htons(m_addr[5]);
		native_sock_addr->sin6_addr.u.Word[6] = ::htons(m_addr[6]);
		native_sock_addr->sin6_addr.u.Word[7] = ::htons(m_addr[7]);

		native_sock_addr->sin6_port = ::htons(static_cast<::u_short>(m_port));

		return result;
	}

	Maybe<SockAddrV6> SockAddrV6::from_native(const _NativeSockAddr& native) noexcept
	{
		if (native.m_sock_addr.ss_family != AF_INET6)
			return {};

		const ::SOCKADDR_IN6* native_sock_addr = reinterpret_cast<const ::SOCKADDR_IN6*>(&native.m_sock_addr);

		return SockAddrV6{
			AddrIPv6{
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[0])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[1])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[2])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[3])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[4])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[5])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[6])),
				static_cast<u16>(::ntohs(native_sock_addr->sin6_addr.u.Word[7]))
			},
			::ntohs(native_sock_addr->sin6_port)
		};
	}

	_NativeSockAddr SockAddr::to_native() const noexcept
	{
		if (m_addr.index() == 0)
			return get<0>(m_addr).to_native();
		
		return get<1>(m_addr).to_native();
	}

	Maybe<SockAddr> SockAddr::from_native(const _NativeSockAddr& native) noexcept
	{
		switch (native.m_sock_addr.ss_family)
		{
		case AF_INET: return Maybe<SockAddr>{ SockAddrV4::from_native(native) };
		case AF_INET6: return Maybe<SockAddr>{ SockAddrV6::from_native(native) };
		}

		return {};
	}

	struct _NativeSocket
	{
		::SOCKET m_sock;
	};

	Maybe<Socket> Socket::from_native(const _NativeSocket& native)
	{
		::WSAPROTOCOL_INFOW proto_info;
		int proto_info_len = sizeof(proto_info);

		int result = ::getsockopt(
			native.m_sock,
			SOL_SOCKET,
			SO_PROTOCOL_INFO,
			reinterpret_cast<char*>(&proto_info), 
			&proto_info_len);

		if (result == SOCKET_ERROR)
		{
			int error = ::WSAGetLastError();
			return {};
		}

		AddrFamily af;

		switch (proto_info.iAddressFamily)
		{
		case AF_UNSPEC: af = AddrFamily::UNSPECIFIED; break;
		case AF_INET: af = AddrFamily::IPv4; break;
		case AF_INET6: af = AddrFamily::IPv6; break;
		default: return {};
		}

		SockType ty;

		switch (proto_info.iSocketType)
		{
		case SOCK_STREAM: ty = SockType::STREAM; break;
		case SOCK_DGRAM: ty = SockType::DATAGRAM; break;
		default: return {};
		}

		Proto pt;

		switch (proto_info.iProtocol)
		{
		case ::IPPROTO_TCP: pt = Proto::TCP; break;
		case ::IPPROTO_UDP: pt = Proto::UDP; break;
		default: return {};
		}

		return Socket{ native, af, ty, pt };
	}

	Socket::Socket(const _NativeSocket& native, AddrFamily family, SockType type, Proto proto)
		: m_sock{ new _NativeSocket{ native.m_sock } }, m_family{ family }, m_type{ type }, m_proto{ proto }
	{
	}

	Socket::Socket(AddrFamily family, SockType type, Proto proto)
		: m_family{ family }, m_type{ type }, m_proto{ proto }
	{
		int af, ty, pt;

		switch (family)
		{
		case AddrFamily::UNSPECIFIED: af = AF_UNSPEC; break;
		case AddrFamily::IPv4: af = AF_INET; break;
		case AddrFamily::IPv6: af = AF_INET6; break;
		default: throw SocketError{};
		}

		switch (type)
		{
		case SockType::STREAM: ty = SOCK_STREAM; break;
		case SockType::DATAGRAM: ty = SOCK_DGRAM; break;
		default: throw SocketError{};
		}

		switch (proto)
		{
		case Proto::TCP: pt = ::IPPROTO_TCP; break;
		case Proto::UDP: pt = ::IPPROTO_UDP; break;
		default: throw SocketError{};
		}

		m_sock = new _NativeSocket{};
		m_sock->m_sock = ::socket(af, ty, pt);

		if (m_sock->m_sock == INVALID_SOCKET)
			throw SocketError{};
	}

	Socket::Socket(Socket&& other) noexcept
		: m_sock{ other.m_sock }, m_family{ other.m_family }, m_type{ other.m_type }, m_proto{ other.m_proto }
	{
		other.m_sock = nullptr;
	}

	Socket::~Socket()
	{
		if (m_sock != nullptr)
		{
			::shutdown(m_sock->m_sock, SD_BOTH);
			::closesocket(m_sock->m_sock);

			delete m_sock;
		}
	}

	AddrFamily Socket::addr_family() const noexcept
	{
		return m_family;
	}

	SockType Socket::sock_type() const noexcept
	{
		return m_type;
	}

	Proto Socket::proto() const noexcept
	{
		return m_proto;
	}

	Result<Unit, SocketConnectError> Socket::connect(const SockAddr& addr)
	{
		_NativeSockAddr native_sock_addr = addr.to_native();

		int result = ::connect(
			m_sock->m_sock, 
			reinterpret_cast<::sockaddr*>(&native_sock_addr.m_sock_addr),
			sizeof(native_sock_addr.m_sock_addr));

		if (result == SOCKET_ERROR)
			return SocketConnectError{};

		return Unit{};
	}

	Result<Unit, SocketCloseError> Socket::close()
	{
		int result = ::closesocket(m_sock->m_sock);

		if (result == SOCKET_ERROR)
			return SocketCloseError{};

		return Unit{};
	}

	Result<Unit, SocketBindError> Socket::bind(const SockAddr& addr)
	{
		_NativeSockAddr native_sock_addr = addr.to_native();

		int result = ::bind(
			m_sock->m_sock, 
			reinterpret_cast<const ::SOCKADDR*>(&native_sock_addr.m_sock_addr),
			sizeof(native_sock_addr.m_sock_addr));

		if (result == SOCKET_ERROR)
			return SocketBindError{};

		return Unit{};
	}

	Result<Unit, SocketListenError> Socket::listen(usize backlog)
	{
		int result = ::listen(m_sock->m_sock, static_cast<int>(backlog));

		if (result == SOCKET_ERROR)
			return SocketListenError{};

		return Unit{};
	}

	[[nodiscard]] Result<Socket, SocketAcceptError> Socket::accept()
	{
		_NativeSockAddr native_sock_addr;
		int sock_addr_len = sizeof(native_sock_addr.m_sock_addr);

		_NativeSocket native_sock;

		native_sock.m_sock = ::accept(
			m_sock->m_sock, 
			reinterpret_cast<::sockaddr*>(&native_sock_addr.m_sock_addr),
			&sock_addr_len);

		if (native_sock.m_sock == INVALID_SOCKET)
			return SocketAcceptError{};

		return Socket{ native_sock, m_family, m_type, m_proto };
	}

	Result<SockAddr, SocketError> Socket::addr() const
	{
		_NativeSockAddr native_sock_addr;
		int storage_len = sizeof(native_sock_addr.m_sock_addr);

		int result = ::getsockname(
			m_sock->m_sock, 
			reinterpret_cast<::PSOCKADDR>(&native_sock_addr.m_sock_addr),
			&storage_len);

		if (result == SOCKET_ERROR)
			return SocketError{};

		Maybe<SockAddr> sock_addr = SockAddr::from_native(native_sock_addr);
		
		if (sock_addr.has_value())
			return sock_addr.unwrap();

		return SocketError{};
	}

	Result<SockAddr, SocketError> Socket::peer() const
	{
		_NativeSockAddr native_sock_addr;
		int storage_len = sizeof(native_sock_addr.m_sock_addr);

		int result = ::getpeername(
			m_sock->m_sock, 
			reinterpret_cast<::PSOCKADDR>(&native_sock_addr.m_sock_addr),
			&storage_len);

		if (result == SOCKET_ERROR)
			return SocketError{};

		Maybe<SockAddr> sock_addr = SockAddr::from_native(native_sock_addr);

		if (sock_addr.has_value())
			return sock_addr.unwrap();

		return SocketError{};
	}

	bool Socket::is_connected() const noexcept
	{
		::SOCKADDR_STORAGE native_sock_addr;
		int storage_len = sizeof(native_sock_addr);

		int result = ::getpeername(
			m_sock->m_sock, 
			reinterpret_cast<::PSOCKADDR>(&native_sock_addr),
			&storage_len);

		return result == 0;
	}

	Result<usize, SocketSendError> Socket::send(const u8* buffer, usize length)
	{
		int result = ::send(
			m_sock->m_sock, 
			reinterpret_cast<const char*>(buffer), 
			static_cast<int>(length), 0);

		if (result == SOCKET_ERROR)
			return SocketSendError{};

		return static_cast<usize>(result);
	}

	Result<usize, SocketReceiveError> Socket::recv(u8* buffer, usize length)
	{
		int result = ::recv(
			m_sock->m_sock, 
			reinterpret_cast<char*>(buffer), 
			static_cast<int>(length), 
			0);

		if (result == SOCKET_ERROR)
			return SocketReceiveError{};

		return static_cast<usize>(result);
	}

	Result<usize, SocketSendError> Socket::send_to(const SockAddr& addr, const u8* buffer, usize length)
	{
		_NativeSockAddr native_sock_addr = addr.to_native();

		int result = ::sendto(
			m_sock->m_sock,
			reinterpret_cast<const char*>(buffer),
			static_cast<int>(length),
			0,
			reinterpret_cast<const ::SOCKADDR*>(&native_sock_addr.m_sock_addr),
			sizeof(native_sock_addr.m_sock_addr));

		if (result == SOCKET_ERROR)
			return SocketSendError{};

		return static_cast<usize>(result);
	}

	Result<Tuple<usize, SockAddr>, SocketReceiveError> Socket::recv_from(u8* buffer, usize length)
	{
		_NativeSockAddr native_sock_addr;
		int native_sock_addr_len = sizeof(native_sock_addr.m_sock_addr);

		int result = ::recvfrom(
			m_sock->m_sock,
			reinterpret_cast<char*>(buffer),
			static_cast<int>(length),
			0,
			reinterpret_cast<::SOCKADDR*>(&native_sock_addr.m_sock_addr),
			&native_sock_addr_len);

		if (result == SOCKET_ERROR)
			return SocketReceiveError{};

		return Tuple<usize, SockAddr>{ static_cast<usize>(result), SockAddr::from_native(native_sock_addr).unwrap() };
	}

	TCPServer::TCPServer(u16 port)
		: m_sock{ AddrFamily::IPv4, SockType::STREAM, Proto::TCP }, m_port { port }
	{
		Result<Unit, SocketBindError> result = m_sock.bind(SockAddrV4{ AddrIPv4::UNSPECIFIED, m_port });

		if (result.is_error())
			throw result.expect_error();
	}

	u16 TCPServer::port() const noexcept
	{
		return m_port;
	}

	Result<SockAddr, SocketError> TCPServer::addr() const noexcept
	{
		return m_sock.addr();
	}

	Result<Unit, SocketListenError> TCPServer::listen(usize backlog)
	{
		return m_sock.listen(backlog);
	}

	Result<Socket, SocketAcceptError> TCPServer::accept()
	{
		return m_sock.accept();
	}

	Result<Unit, SocketCloseError> TCPServer::close()
	{
		return m_sock.close();
	}
}
