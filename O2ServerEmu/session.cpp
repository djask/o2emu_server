#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>
#include <vector>
#include <fstream>
#include <boost/asio.hpp>
#include <stack>

#include "utils.h"

#define SERVER_PORT 15010

using boost::asio::ip::tcp;

class session
	: public std::enable_shared_from_this<session>
{
public:
	session(tcp::socket socket)
		: socket_(std::move(socket)) {
	}

	void start() {
		do_read();
	}

private:
	void do_read() {
		ZeroMemory(data_, max_length);
		auto self(shared_from_this());
		socket_.async_read_some(boost::asio::buffer(data_, max_length),
			[this, self](boost::system::error_code ec, std::size_t length) {
			if (!ec) {
				/* We're not using the read length as sometimes o2jam thinks
				this is wrong lol, so we go by byte 1 of the payload */
				handle_cmd(data_);
			}
		});
	}

	void handle_cmd(unsigned char payload[]) {
		/* Setting up the command variables, I'm not using a struct
		as I'll have to send the whole payload again anyways and this'll
		make it harder*/
		unsigned short len;
		memcpy(&len, payload, sizeof(unsigned short));
		unsigned short cmd;
		memcpy(&cmd, payload + 2, sizeof(unsigned short));

		switch (cmd) {
		case 0x03f1:
		{
			std::cout << "LOGIN REQUEST"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x37, 0x00, 0xf2, 0x03, 0x02, 0x00, 0x00, 0x5d,
				0xfe, 0xda, 0xad, 0xf5, 0x7f, 0x6b, 0x0e, 0x49,
				0x2c, 0x3b, 0xba, 0x56, 0x17, 0xbb, 0x8b, 0x4c,
				0x1d, 0x07, 0x28, 0x80, 0xd2, 0x51, 0x0c, 0xda,
				0x54, 0x4a, 0xd1, 0x50, 0x35, 0x61, 0xa8, 0xfe,
				0x67, 0xb5, 0xaa, 0xe1, 0x8b, 0x5d, 0x7c, 0x7b,
				0x2a, 0xac, 0x22, 0xc3, 0x02, 0xf8, 0x1e
			};
			o2jam_write(packet_bytes);
			break;
		}
		case 0x03f3:
		{
			std::cout << "LOGIN REQUEST NO 2"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x37, 0x00, 0xf4, 0x03, 0x40, 0xba, 0x11, 0x36,
				0x84, 0x0d, 0x40, 0x7b, 0x78, 0x64, 0x2a, 0xc9,
				0xc5, 0x19, 0xcc, 0xaa, 0x7d, 0xb1, 0x65, 0x3b,
				0x70, 0x1e, 0x6c, 0x18, 0x58, 0x0f, 0x05, 0x22,
				0xd8, 0x08, 0xc8, 0xd7, 0x1c, 0x15, 0x36, 0x84,
				0x0d, 0x40, 0x7b, 0x78, 0x64, 0x2a, 0xc9, 0xc5,
				0x19, 0xcc, 0xaa, 0x7d, 0xb1, 0x65, 0xef
			};
			o2jam_write(packet_bytes);
			break;
		}
		case 0xfff0:
		{
			std::cout << "LOGOUT/IDLE REQUEST"; utils::print_hex(payload, len);
			break;
		}
		case 0x03ec:
		{
			std::cout << "SOMETHING AFTER LOGIN???"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x10, 0x00, 0xed, 0x03, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x87, 0x53, 0x00, 0x00
			};
			o2jam_write(packet_bytes);
			break;
		}
		case 0x03ea:
		{
			std::cout << "LOADING Channel1.spt..."; utils::print_hex(payload, len);

			int sz = utils::fcopy(data_, "./Spt/Channel1.spt", 0);
			do_write(sz);
			break;
		}
		case 0x0fbe:
		{
			std::cout << "LOADING MusicList.spt..."; utils::print_hex(payload, len);

			int sz = utils::fcopy(data_, "./Spt/MusicList.spt", 6);
			do_write(sz);
			break;
		}
		case 0x07d0:
		{
			std::cout << "LOADING D007.spt..."; utils::print_hex(payload, len);

			int sz = utils::fcopy(data_, "./Spt/D007.spt", 20);
			do_write(sz);
			break;
		}
		case 0x07d2:
		{
			std::cout << "LOADING D207.spt..."; utils::print_hex(payload, len);
			char packet_bytes[] = {
				0x37, 0x00, 0xdd, 0x07, 0x8c, 0x6e, 0x3f, 0x8f,
				0xc1, 0x91, 0xa7, 0x00, 0x3f, 0x8c, 0x7d, 0x8e,
				0x67, 0x97, 0x70, 0x4f, 0x32, 0x45, 0x6d, 0x75,
				0x21, 0x20, 0x50, 0x6f, 0x77, 0x65, 0x72, 0x65,
				0x64, 0x20, 0x42, 0x79, 0x20, 0x44, 0x65, 0x76,
				0x20, 0x54, 0x65, 0x61, 0x6d, 0x20, 0x4a, 0x61,
				0x63, 0x6b, 0x43, 0x68, 0x65, 0x6e, 0x00
			};

			memcpy(data_, packet_bytes, sizeof(packet_bytes));
			int sz = utils::fcopy(data_ + 55, "./Spt/D207.spt", 20);
			do_write(sz + sizeof(packet_bytes));
			break;
		}
		case 0x13a4:
		{
			std::cout << "NO IDEA BUT WE CAN RESPOND"; utils::print_hex(payload, len);

			unsigned char packet_bytes[] = {
				0x08, 0x00, 0xa5, 0x13, 0xb3, 0x11, 0x01, 0x00
			};

			o2jam_write(packet_bytes);
			break;
		}
		case 0x07d4:
		{
			std::cout << "ROOM CREATION, Name is " << data_ + 4 << std::endl;

			unsigned char packet_bytes[] = {
				0x0d, 0x00, 0xd6, 0x07, 0x00, 0x00, 0x00, 0x00,
				0x00, 0x00, 0x00, 0x00, 0x00
			};

			o2jam_write(packet_bytes);
			flag = true;
			break;
		}
		case 0x07e8:
		{
			std::cout << "GOT A MASSIVE PAYLOAD, I'M ASSUMING THIS IS THE KEY STAMP";
			unsigned char packet_bytes[] = {
				0x04, 0x00, 0xe9, 0x07
			};
			o2jam_write(packet_bytes);
			break;
		}
		case 0x03e8:
		{
			std::cout << "LOGIN SOMETHING???"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x08, 0x00, 0xe9, 0x03, 0x00, 0x00, 0x00, 0x00
			};

			o2jam_write(packet_bytes);
			break;
		}
		case 0x03ef:
		{
			std::cout << "GOT LOGIN CREDENTIALS"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x0c, 0x00, 0xf0, 0x03, 0x00, 0x00, 0x00, 0x00,
				0xee, 0x60, 0x01, 0x00
			};
			o2jam_write(packet_bytes);
			break;
		}

		//song select related commands??
		case 0x0fa0:
		{
			std::cout << "SONG SELECT"; utils::print_hex(payload, len);

			//set 3rd bit to a1 and send
			data_[2] = 0xa1;

			if (!flag) {
				do_write(data_[0]);
			}
			else {
				auto cp = std::make_unique<unsigned char[]>(len);
				memcpy(cp.get(), data_, data_[0]);
				repl_stack.push(std::move(cp));
				do_read();
			}
			break;
		}
		case 0x0fa4:
		{
			std::cout << "INITIAL ROOM STUFF, WAITING FOR LOAD..."; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = "\x06\x00\xa5\x0f\x00\x00";

			auto cp = std::make_unique<unsigned char[]>(packet_bytes[0]);
			memcpy(cp.get(), packet_bytes, packet_bytes[0]);
			repl_stack.push(std::move(cp));
			do_read();
			break;
		}
		case 0x0fb7:
		{
			std::cout << "FINISHED LOADING ROOM, SENDING ALL BUFFERED REPLIES"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = "\x09\x00\xb8\x0f\x01\x00\x00\x00\x00";
			o2jam_write(packet_bytes);

			while (!repl_stack.empty()) {
				o2jam_write(repl_stack.top().get());

				repl_stack.pop();
			}
			flag = false;
			break;
		}
		case 0x0faa:
		{
			std::cout << "SONG PLAY START?"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x0c, 0x00, 0xab, 0x0f, 0x00, 0x00, 0x00, 0x00,
				0x93, 0x21, 0x74, 0x25
			};
			o2jam_write(packet_bytes);

			break;
		}
		case 0x0fac:
		{
			std::cout << "SONG PLAY CMD 2"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x05, 0x00, 0xad, 0x0f, 0x00
			};
			o2jam_write(packet_bytes);

			break;
		}
		case 0x0fb5:
		{
			std::cout << "SONG QUIT?"; utils::print_hex(payload, len);
			unsigned char packet_bytes[] = {
				0x09, 0x00, 0xb6, 0x0f, 0x00, 0x1e, 0x00, 0x00,
				0x00
			};
			o2jam_write(packet_bytes);

			//need to wait for b7 command again...
			flag = true;

			break;
		}
		case 0x0fae:
		{
			std::cout << "SOME SONG STATUS THING, IGNORING"; utils::print_hex(payload, len);
			do_read();
			break;
		}
		case 0x1771:
		{
			std::cout << "TCP echo???"; utils::print_hex(payload, len);
			//echo back
			do_write(4);
			break;
		}
		default:
		{
			std::cout << "UNKNOWN"; utils::print_hex(payload, len);
			//do_write(len);
			do_read();
			break;
		}
		}
	}

	void o2jam_write(unsigned char payload[]) {
		//actuall don't need to zero mem, as we are only writing to length anyway

		//first 2 bytes tells us length
		unsigned short len;
		memcpy(&len, payload, sizeof(unsigned short));
		memcpy(data_, payload, payload[0]);
		do_write(len);
	}

	void do_write(std::size_t length) {
		std::cout << "SENDING ";
		utils::print_hex(data_, length);
		auto self(shared_from_this());
		boost::asio::async_write(socket_,
			boost::asio::buffer(data_, length),
			[this, self](boost::system::error_code ec, std::size_t length) {
			if (!ec) {
				do_read();
			}
		});
	}

	void write_cb(boost::system::error_code ec, std::size_t length) {
		if (!ec) {
			do_read();
		}
	}

	std::stack<std::unique_ptr<unsigned char[]>> repl_stack;
	bool flag;

	tcp::socket socket_;
	enum { max_length = 10248 }; //max payload size we send
	unsigned char data_[max_length];
};

class server {
public:
	server(boost::asio::io_context& io_context, short port)
		: acceptor_(io_context, tcp::endpoint(tcp::v4(), port))
	{
		do_accept();
	}

private:
	void do_accept() {
		acceptor_.async_accept(
			[this](boost::system::error_code ec, tcp::socket socket)
		{
			if (!ec) {
				std::make_shared<session>(std::move(socket))->start();
			}
			do_accept();
		});
	}

	tcp::acceptor acceptor_;
};

int main() {
	try {
		boost::asio::io_context io_context;
		server s(io_context, SERVER_PORT);
		io_context.run();
	}
	catch (std::exception& e) {
		std::cerr << e.what() << std::endl;
	}

	return 0;
}
