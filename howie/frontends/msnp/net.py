# net.py -- Connection, HttpProxyConnection classes
#
# Copyright (C) 2003 Manish Jethani (manish_jethani AT yahoo.com)
#
# This program is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 2 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

import socket

from string import split, join

class Connection:  # generic tcp connection wrapper
    def __init__(self, server):
        self.socket = None
        self.server = server

    def establish(self):
        if self.socket == None:
            s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
            server = (self.server[0], int(self.server[1]))
            s.connect(server)
            self.socket = s
        return self.socket

    def send_data(self, buf):
        return self.socket.send(buf)

    def receive_data(self, bufsize):
        return self.socket.recv(bufsize)

    def send_data_all(self, buf):
        total = len(buf)
        sent = 0
        while sent < total:
            sent = sent + self.send_data(buf[sent:])
        return sent

    def send_data_line(self, line):
        # print "C:" + line
        return self.send_data_all(line) + self.send_data_all('\r\n')

    def receive_data_line(self):
        cnt = 0
        buf = ''
        while 1:
            in_byte = self.receive_data(1)
            if in_byte == '':
                return None
            if in_byte == '\r':
                cnt = 1
            elif in_byte == '\n' and cnt == 1:
                cnt = 2
            else:
                cnt = 0
            buf = buf + in_byte
            if cnt == 2:
                # print "S:" + buf
                return buf

    def break_(self):
        self.socket.shutdown(2)
        self.socket.close()
        self.socket = None

class HttpProxyConnection(Connection):  # http tunnelling
    def __init__(self, server, proxy):
        Connection.__init__(self, server)
        self.proxy = proxy

    def establish(self):
        tmp = self.server
        self.server = self.proxy
        try:
            Connection.establish(self)
        finally:
            self.server = tmp

        connect_str = 'CONNECT ' + self.server[0] \
            + ':' + str(self.server[1]) \
            + ' HTTP/1.0\r\n'
        self.send_data_all(connect_str)
        self.send_data_all('User-Agent: msnp.py\r\n')
        self.send_data_all('Host: ' + self.server[0] + '\r\n')
        self.send_data_all('\r\n')

        status = -1
        while 1:
            buf = self.receive_data_line()
            if status == -1:
                resp = split(buf, ' ', 2)
                if len(resp) > 1:
                    status = int(resp[1])
                else:
                    status = 0
            if buf == '\r\n':
                break

        if status != 200:
            self.socket = None
        return self.socket

# vim: set ts=4 sw=4 et tw=79 :

