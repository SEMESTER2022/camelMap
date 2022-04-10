from os import terminal_size
import socket
import sys
import json

DEFAULT_SOCKET = '/tmp/camel.sock'
MAX_MSG_SIZE=65536

class QueryResponse():
    def __init__(self, error_code = 0, msg = '', payload_data = '{}') -> None:
        self.error_code = error_code
        self.msg = msg
        self.payload_data = payload_data
    def set_response(self, error_code, msg, payload_data):
        self.error_code = error_code
        self.msg = msg
        self.payload_data = payload_data
    def dump(self):
        return {'error_code': self.error_code, 'msg': self.msg, 'payload_data': self.payload_data}

class UnixClient():
    def __init__(self, source_lng, source_lat, target_lng, target_lat):
        data = str(source_lng) + ";" + str(source_lat) + ";" + str(target_lng) + ";" + str(target_lat) + ";\n"
        self.query = data.encode()
        self.response = QueryResponse()

    def DoRequest(self):
        sock = socket.socket(socket.AF_UNIX, socket.SOCK_STREAM)
        try:
            sock.connect(DEFAULT_SOCKET)
        except:
            print('fail to connect socket')
            self.response.set_response(1, 'processing request data failed', '{}')
            return self.response.dump()

        try:
            print(self.query)
            sock.sendall(self.query)
            print('start receive')
            data = ""
            while True:
                buf = sock.recv(1024)

                if len(buf) <= 0:
                    break
                else:
                    data += buf.decode()

            print(f"here {data}")
            self.response.set_response(0, '', data)
        finally:
            sock.close()

        return self.response.dump()
