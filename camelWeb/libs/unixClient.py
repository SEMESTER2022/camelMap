import socket
import sys
import json

DEFAULT_SOCKET = '/tmp/camel.sock'

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
        print(f"{self.query}")
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

            recv_size = sock.recv(8)
            exp_recv = int.from_bytes(recv_size, sys.byteorder)

            total_recv = 0
            payload = ''
            while total_recv != exp_recv:
                payload += sock.recv(exp_recv - total_recv).decode('utf-8')
                total_recv += len(payload)

            self.response.set_response(0, '', payload)
        finally:
            sock.close()
        return self.response.dump()
