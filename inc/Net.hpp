#pragma once

#include "Error.hpp"
#include "Types.hpp"
#include "Variant.hpp"
#include "Result.hpp"
#include "Maybe.hpp"
#include "Tuple.hpp"

namespace bsl::net
{
	struct NetError : Error
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Network error.";
		}
	};

	struct HostnameResolutionError : NetError
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Hostname resolution error.";
		}
	};

	struct SocketError : NetError
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket error.";
		}
	};

	struct SocketConnectError : SocketError
	{
		virtual [[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket connect error.";
		}
	};

	struct SocketCloseError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket close error.";
		}
	};

	struct SocketSendError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket send error.";
		}
	};

	struct SocketReceiveError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket receive error.";
		}
	};

	struct SocketBindError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket bind error.";
		}
	};

	struct SocketListenError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket listen error.";
		}
	};

	struct SocketAcceptError : SocketError
	{
		[[nodiscard]] const char* msg() const noexcept override
		{
			return "Socket accept error.";
		}
	};

	void setup();
	void cleanup();

	class AddrIPv4;
	class AddrIPv6;
	class IPAddr;

	struct _NativeSockAddr;

	class SockAddrV4;
	class SockAddrV6;
	class SockAddr;

	struct _NativeSocket;

	class Socket;

	class AddrIPv4
	{
	private:
		u8 m_addr[4];

		u32 int_value() const noexcept
		{
			return (static_cast<u32>(m_addr[0]) << 24) | 
				(static_cast<u32>(m_addr[1]) << 16) | 
				(static_cast<u32>(m_addr[2]) << 8) | 
				static_cast<u32>(m_addr[3]);
		}

	public:
		static const AddrIPv4 LOCALHOST;
		static const AddrIPv4 UNSPECIFIED;
		static const AddrIPv4 BROADCAST;

		AddrIPv4(u8 a, u8 b, u8 c, u8 d) noexcept
			: m_addr{ a, b, c, d }
		{
		}

		[[nodiscard]] bool is_localhost() const noexcept
		{
			return *this == LOCALHOST;
		}

		[[nodiscard]] bool is_unspecified() const noexcept
		{
			return *this == UNSPECIFIED;
		}

		[[nodiscard]] bool is_broadcast() const noexcept
		{
			return *this == BROADCAST;
		}

		[[nodiscard]] bool is_documentation() const noexcept
		{
			return (m_addr[0] == 192 && m_addr[1] == 0 && m_addr[2] == 2) ||
				(m_addr[0] == 192 && m_addr[1] == 51 && m_addr[2] == 100) ||
				(m_addr[0] == 203 && m_addr[1] == 0 && m_addr[2] == 113) ||
				(m_addr[0] == 233 && m_addr[1] == 252 && m_addr[2] == 0);
		}

		[[nodiscard]] const u8* octets() const noexcept
		{
			return m_addr;
		}

		[[nodiscard]] u8 operator[](usize idx) const
		{
			return m_addr[idx];
		}

		[[nodiscard]] bool operator<(const AddrIPv4& rhs) const noexcept
		{
			return int_value() < rhs.int_value();
		}

		[[nodiscard]] bool operator>(const AddrIPv4& rhs) const noexcept
		{
			return rhs < *this;
		}

		[[nodiscard]] bool operator<=(const AddrIPv4& rhs) const noexcept
		{
			return !(*this > rhs);
		}

		[[nodiscard]] bool operator>=(const AddrIPv4& rhs) const noexcept
		{
			return !(*this < rhs);
		}

		[[nodiscard]] bool operator==(const AddrIPv4& rhs) const noexcept
		{
			return (*this <= rhs) && (*this >= rhs);
		}

		[[nodiscard]] bool operator!=(const AddrIPv4& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	};

	class AddrIPv6
	{
	private:
		u16 m_addr[8];

		u64 high_int_value() const noexcept
		{
			return (static_cast<u64>(m_addr[0]) << 48) |
				(static_cast<u64>(m_addr[1]) << 32) |
				(static_cast<u64>(m_addr[2]) << 16) |
				static_cast<u64>(m_addr[3]);
		}

		u64 low_int_value() const noexcept
		{
			return (static_cast<u64>(m_addr[4]) << 48) |
				(static_cast<u64>(m_addr[5]) << 32) |
				(static_cast<u64>(m_addr[6]) << 16) |
				static_cast<u64>(m_addr[7]);
		}

	public:
		static const AddrIPv6 LOCALHOST;
		static const AddrIPv6 UNSPECIFIED;

		AddrIPv6(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h) noexcept
			: m_addr{ a, b, c, d, e, f, g, h }
		{
		}

		AddrIPv6(const AddrIPv4& addr) noexcept
			: AddrIPv6{ 0, 0, 0, 0, 0, 0xFFFF, 
				static_cast<u16>((m_addr[0] << 8) | m_addr[1]),
				static_cast<u16>((m_addr[2] << 8) | m_addr[3])
			}
		{
		}

		[[nodiscard]] bool is_localhost() const noexcept
		{
			return *this == LOCALHOST;
		}

		[[nodiscard]] bool is_unspecified() const noexcept
		{
			return *this == UNSPECIFIED;
		}

		[[nodiscard]] bool is_linklocal() const noexcept
		{
			return (m_addr[0] & 0xFFC0) == 0xFE80;
		}

		[[nodiscard]] bool is_ipv4_mapped() const noexcept
		{
			return m_addr[0] == 0 &&
				m_addr[1] == 0 &&
				m_addr[2] == 0 &&
				m_addr[3] == 0 &&
				m_addr[4] == 0 &&
				m_addr[5] == 0xFFFF;
		}

		[[nodiscard]] bool is_documentation() const noexcept
		{
			return m_addr[0] == 0x2001 && m_addr[1] == 0x0DB8;
		}

		[[nodiscard]] bool is_benchmarking() const noexcept
		{
			return m_addr[0] == 0x2001 && m_addr[1] == 2 && m_addr[1] == 0;
		}

		[[nodiscard]] bool is_private() const noexcept
		{
			return (m_addr[0] & 0xFE00) == 0xFC00;
		}

		[[nodiscard]] Maybe<AddrIPv4> to_ipv4() const noexcept
		{
			if (is_unspecified())
				return AddrIPv4::UNSPECIFIED;

			if (is_localhost())
				return AddrIPv4::LOCALHOST;

			if (!is_ipv4_mapped())
				return {};

			return AddrIPv4{
				static_cast<u8>((m_addr[6] >> 8) & 0xFF),
				static_cast<u8>(m_addr[6] & 0xFF),
				static_cast<u8>((m_addr[7] >> 8) & 0xFF),
				static_cast<u8>(m_addr[7] & 0xFF)
			};
		}

		[[nodiscard]] const u16* hextets() const noexcept
		{
			return m_addr;
		}

		[[nodiscard]] u16 operator[](usize idx) const
		{
			return m_addr[idx];
		}

		[[nodiscard]] bool operator<(const AddrIPv6& rhs) const noexcept
		{
			if (high_int_value() < rhs.high_int_value())
				return true;
			else if (high_int_value() > rhs.high_int_value())
				return false;

			return low_int_value() < rhs.low_int_value();
		}

		[[nodiscard]] bool operator>(const AddrIPv6& rhs) const noexcept
		{
			return rhs < *this;
		}

		[[nodiscard]] bool operator<=(const AddrIPv6& rhs) const noexcept
		{
			return !(*this > rhs);
		}

		[[nodiscard]] bool operator>=(const AddrIPv6& rhs) const noexcept
		{
			return !(*this < rhs);
		}

		[[nodiscard]] bool operator==(const AddrIPv6& rhs) const noexcept
		{
			return (*this <= rhs) && (*this >= rhs);
		}

		[[nodiscard]] bool operator!=(const AddrIPv6& rhs) const noexcept
		{
			return !(*this == rhs);
		}
	};

	class IPAddr
	{
	private:
		Variant<AddrIPv4, AddrIPv6> m_addr;

	public:
		IPAddr(const AddrIPv4& ip) noexcept
			: m_addr{ InPlaceIndex<0>{}, ip }
		{
		}

		IPAddr(const AddrIPv6& ip) noexcept
			: m_addr{ InPlaceIndex<1>{}, ip }
		{
		}

		IPAddr(u8 a, u8 b, u8 c, u8 d) noexcept
			: m_addr{ InPlaceIndex<0>{}, a, b, c, d }
		{
		}

		IPAddr(u16 a, u16 b, u16 c, u16 d, u16 e, u16 f, u16 g, u16 h) noexcept
			: m_addr{ InPlaceIndex<1>{}, a, b, c, d, e, f, g, h }
		{
		}

		[[nodiscard]] bool is_localhost() const noexcept
		{
			return m_addr.index() == 0 ?
				get<0>(m_addr).is_localhost() :
				get<1>(m_addr).is_localhost();
		}

		[[nodiscard]] bool is_unspecified() const noexcept
		{
			return m_addr.index() == 0 ?
				get<0>(m_addr).is_unspecified() :
				get<1>(m_addr).is_unspecified();
		}

		[[nodiscard]] bool is_ipv4() const noexcept
		{
			return m_addr.index() == 0;
		}

		[[nodiscard]] bool is_ipv6() const noexcept
		{
			return m_addr.index() == 1;
		}

		[[nodiscard]] AddrIPv4 to_ipv4() const
		{
			return get<0>(m_addr);
		}

		[[nodiscard]] AddrIPv6 to_ipv6() const
		{
			return get<1>(m_addr);
		}
	};

	[[nodiscard]] Result<IPAddr, HostnameResolutionError> resolve(const char* hostname, const char* service = nullptr);

	class SockAddrV4
	{
	private:
		friend SockAddr;

		[[nodiscard]] _NativeSockAddr to_native() const noexcept;
		[[nodiscard]] static Maybe<SockAddrV4> from_native(const _NativeSockAddr& native) noexcept;

		AddrIPv4 m_addr;
		u16 m_port;

	public:
		SockAddrV4(const AddrIPv4& addr, u16 port) noexcept
			: m_addr{ addr }, m_port{ port }
		{
		}

		[[nodiscard]] AddrIPv4 addr() const noexcept
		{
			return m_addr;
		}

		[[nodiscard]] u16 port() const noexcept
		{
			return m_port;
		}
	};

	class SockAddrV6
	{
	private:
		friend SockAddr;

		[[nodiscard]] _NativeSockAddr to_native() const noexcept;
		[[nodiscard]] static Maybe<SockAddrV6> from_native(const _NativeSockAddr& native) noexcept;

		AddrIPv6 m_addr;
		u16 m_port;

	public:
		SockAddrV6(const AddrIPv6& addr, u16 port) noexcept
			: m_addr{ addr }, m_port{ port }
		{
		}

		[[nodiscard]] const AddrIPv6& addr() const noexcept
		{
			return m_addr;
		}

		[[nodiscard]] u16 port() const noexcept
		{
			return m_port;
		}
	};

	class SockAddr
	{
	private:
		friend Socket;

		[[nodiscard]] _NativeSockAddr to_native() const noexcept;
		[[nodiscard]] static Maybe<SockAddr> from_native(const _NativeSockAddr& native) noexcept;

		Variant<SockAddrV4, SockAddrV6> m_addr;

	public:
		SockAddr(const SockAddrV4& addr)
			: m_addr{ InPlaceIndex<0>{}, addr }
		{
		}

		SockAddr(const SockAddrV6& addr)
			: m_addr{ InPlaceIndex<1>{}, addr }
		{
		}

		[[nodiscard]] bool is_ipv4() const noexcept
		{
			return m_addr.index() == 0;
		}

		[[nodiscard]] bool is_ipv6() const noexcept
		{
			return m_addr.index() == 1;
		}

		[[nodiscard]] Maybe<SockAddrV4> to_ipv4() const
		{
			if (m_addr.index() != 0)
				return {};

			return get<0>(m_addr);
		}

		[[nodiscard]] Maybe<SockAddrV6> to_ipv6() const
		{
			if (m_addr.index() != 1)
				return {};

			return get<1>(m_addr);
		}
	};

	enum class AddrFamily
	{
		UNSPECIFIED,
		IPv4,
		IPv6
	};

	enum class SockType
	{
		STREAM,
		DATAGRAM
	};

	enum class Proto
	{
		TCP,
		UDP
	};

	class Socket
	{
	private:
		_NativeSocket* m_sock;

		AddrFamily m_family;
		SockType m_type;
		Proto m_proto;

		[[nodiscard]] static Maybe<Socket> from_native(const _NativeSocket& native);

		Socket(const _NativeSocket& native, AddrFamily family, SockType type, Proto proto);

	public:
		Socket(AddrFamily family, SockType type, Proto proto);
		Socket(const Socket&) = delete;
		Socket(Socket&& other) noexcept;

		~Socket();

		[[nodiscard]] AddrFamily addr_family() const noexcept;
		[[nodiscard]] SockType sock_type() const noexcept;
		[[nodiscard]] Proto proto() const noexcept;

		[[nodiscard]] Result<Unit, SocketConnectError> connect(const SockAddr& addr);
		[[nodiscard]] Result<Unit, SocketCloseError> close();

		[[nodiscard]] Result<Unit, SocketBindError> bind(const SockAddr& addr);
		
		[[nodiscard]] Result<Unit, SocketListenError> listen(usize backlog);
		[[nodiscard]] Result<Socket, SocketAcceptError> accept();

		[[nodiscard]] Result<SockAddr, SocketError> addr() const;
		[[nodiscard]] Result<SockAddr, SocketError> peer() const;

		[[nodiscard]] bool is_connected() const noexcept;

		[[nodiscard]] Result<usize, SocketSendError> send(const u8* buffer, usize length);
		[[nodiscard]] Result<usize, SocketReceiveError> recv(u8* buffer, usize length);

		[[nodiscard]] Result<usize, SocketSendError> send_to(const SockAddr& addr, const u8* buffer, usize length);
		[[nodiscard]] Result<Tuple<usize, SockAddr>, SocketReceiveError> recv_from(u8* buffer, usize length);
	};

	class TCPServer
	{
	private:
		Socket m_sock;

		u16 m_port;

	public:
		TCPServer(u16 port);

		[[nodiscard]] u16 port() const noexcept;

		[[nodiscard]] Result<SockAddr, SocketError> addr() const noexcept;

		[[nodiscard]] Result<Unit, SocketListenError> listen(usize backlog);
		[[nodiscard]] Result<Socket, SocketAcceptError> accept();

		[[nodiscard]] Result<Unit, SocketCloseError> close();
	};
}
