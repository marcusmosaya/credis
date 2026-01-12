import socket
import time
def encode_command(*args):
    """
    Encode a Redis command in RESP format.
    Example: encode_command("SET", "mykey", "hello")
    """
    cmd = f"*{len(args)}\r\n"
    for arg in args:
        arg = str(arg)
        cmd += f"${len(arg)}\r\n{arg}\r\n"
    return cmd.encode("utf-8")

def decode_response(sock:socket):
    """
    Decode a single RESP reply from a Redis server socket.
    """
    prefix = sock.recv(1)
    if not prefix:
        raise ConnectionError("No response from server")
    if prefix == b'+':  # Simple String
        return recvuntil(sock,b'\r\n').decode().strip()
    elif prefix == b'-':  # Error
        return Exception(recvuntil(sock,b'\r\n').decode().strip())
    elif prefix == b':':  # Integer
        return int(recvuntil(sock,b'\r\n').decode().strip())
    elif prefix == b'$':  # Bulk String
        length = int(recvuntil(sock,b'\r\n').decode().strip())
        if length == -1:
            return None
        data = sock.recv(length)
        sock.recv(2)  # discard trailing \r\n
        return data.decode()
    elif prefix == b'*':  # Array
        count = int(recvuntil(sock,b'\r\n').decode().strip())
        return [decode_response(sock) for _ in range(count)]
    else:
        raise ValueError(f"Unknown RESP type: {prefix}")

def redis_command(host="127.0.0.1", port=8080):
    """
    Send a Redis command and return the decoded response.    """
    isRunning=True
    with socket.create_connection((host, port)) as sock:
        while isRunning:
            command=input("credis >")
            if(command=="exit"):
                isRunning=False
                return
            command_list=command.split()
            command=command_list[0]
            sock.sendall(encode_command(command.upper(), *command_list[1:]))
            print("credis >"+ str(decode_response(sock)))
def recvuntil(sock: socket.socket, terminator: bytes) -> bytes:
    data = b""
    while terminator not in data:
        chunk = sock.recv(1)
        if not chunk:
            break
        data += chunk
    return data 

# Example usage
if __name__ == "__main__":
    redis_command()
    #print(redis_command("GET","c",9))
    #print(redis_command("HGET","user:001","name"))
    '''print(redis_command("SET", "c",1))  # Expect: OK
    print(redis_command("SET","b",2))
    print(redis_command("SET","c",3))
    print(redis_command("GET","a"))
    print(redis_command("GET","a"))
    print(redis_command("GET","b"))
    print(redis_command("SET","d",4))
    print(redis_command("GET","c"))'''
