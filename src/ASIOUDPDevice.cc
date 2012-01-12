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

#include <iostream>
#include <boost/version.hpp>
#include <asio_udp_device/ASIOUDPDevice.h>

using namespace std;
namespace ba = boost::asio;
using ba::ip::udp;

ASIOUDPDevice::ASIOUDPDevice()
{
  open = false;
  local_socket = 0;
  remote_socket = 0;
}

ASIOUDPDevice::ASIOUDPDevice(unsigned int local_port,
                             const string &ip_address,
                             unsigned int remote_port)
{
  open = false;
  local_socket = 0;
  remote_socket = 0;

  Open(local_port, ip_address, remote_port);
}

ASIOUDPDevice::~ASIOUDPDevice()
{
  if (open)
    Close();

  if (local_socket != 0)
    delete local_socket;

  if (remote_socket != 0)
    delete remote_socket;
}

void ASIOUDPDevice::Open(unsigned int local_port,
                         const string &remote_ip_address,
                         unsigned int remote_port)
{
  udp::endpoint local_endpoint(udp::v4(), local_port);
  udp::endpoint
    remote_endpoint(ba::ip::address::from_string(remote_ip_address), remote_port);

  if (!open)
    {
      local_socket = new udp::socket(io_service, local_endpoint);
#if BOOST_VERSION >= 104700
      local_socket->non_blocking(true);
#else
      boost::asio::socket_base::non_blocking_io non_blocking_command(true);
      local_socket->io_control(non_blocking_command);
#endif

      remote_socket = new udp::socket(io_service);

      try
        {
          remote_socket->connect(remote_endpoint);
        }
      catch (std::exception e)
        {
          cerr << "Failed to connect to remote socket" << endl;
          throw;
        }

      if (!local_socket->is_open())
        throw runtime_error("Failed to open local socket");

      if (!remote_socket->is_open())
        throw runtime_error("Failed to open remote socket");

      open = true;
    }
}

void ASIOUDPDevice::Close()
{
  if (open)
    {
      local_socket->close();
      remote_socket->close();
      open = false;
    }
}


void ASIOUDPDevice::SetReadCallback(const boost::function<void (const unsigned char*, size_t)>& handler)
{
  read_callback = handler;
}

bool ASIOUDPDevice::Write(const vector<unsigned char>& msg)
{
  if (!open)
    return false;

  try
    {
      remote_socket->send(ba::buffer(&(msg[0]), msg.size()));
    }
  catch (const boost::system::system_error& err)
    {
      throw;
    }

  return true;
}

bool ASIOUDPDevice::Write(const string& msg)
{
  if (!open)
    return false;

  try
    {
      remote_socket->send(ba::buffer(msg.c_str(), msg.size()));
    }
  catch (const boost::system::system_error& err)
    {
      throw;
    }

  return true;
}

void ASIOUDPDevice::Read()
{
  size_t bytes_transferred = 0;

  if (local_socket->available() > 0)
    {
      try
        {
          bytes_transferred = local_socket->receive(ba::buffer(read_msg, MAX_READ_LENGTH));
        }
      catch (boost::system::system_error& err)
        {
          std::cerr << "Error: " << err.what() << std::endl;
        }
    }

  if (!read_callback.empty() && (bytes_transferred > 0))
    read_callback(const_cast<unsigned char *>(read_msg), bytes_transferred);
}
