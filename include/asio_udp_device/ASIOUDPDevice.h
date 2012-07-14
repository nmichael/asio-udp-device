/*
  This file is part of asio-udp-device, a class wrapper to
  use the boost::asio udp functionality.

  asio-udp-device is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.

  Nathan Michael, Oct. 2011
*/

#ifndef __ASIOUDPDEVICE__
#define __ASIOUDPDEVICE__

#include <string>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/function.hpp>

#define MAX_READ_LENGTH 512

class ASIOUDPDevice
{
 public:
  ASIOUDPDevice();
  ASIOUDPDevice(unsigned int local_port);
  ASIOUDPDevice(unsigned int local_port,
                const std::string &ip_address,
                unsigned int remote_port);
  ~ASIOUDPDevice();

  void Open(unsigned int local_port);
  void Open(unsigned int local_port,
            const std::string &ip_address,
            unsigned int remote_port);
  void Close();

  void Read();
  void SetReadCallback(const boost::function<void (const unsigned char*, size_t)>& handler);

  bool Write(const std::string& msg);
  bool Write(const std::vector<unsigned char>& msg);

 private:
  bool open;

  boost::asio::io_service io_service;
  boost::asio::ip::udp::socket* local_socket;
  boost::asio::ip::udp::socket* remote_socket;

  boost::function<void (const unsigned char*, size_t)> read_callback;

  unsigned char read_msg[MAX_READ_LENGTH];
};
#endif
