
import pytest

import visr
import rcl
import pml
import rrl

asyncio = pytest.importorskip("asyncio")
nest_asyncio = pytest.importorskip("nest_asyncio") 

nest_asyncio.apply()

class EchoClientProtocol:
    def __init__(self, message_received_future):
        self.message_received = message_received_future
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport
        # print('Connection established')

    def datagram_received(self, data, addr):
        print("Received:", data.decode())
        self.message_received.set_result(data.decode())
        # print("Close the socket")

    def error_received(self, exc):
        print('Error received:', exc)

    def connection_lost(self, exc):
        print("Connection closed")
        self.on_con_lost.set_result(True)


@pytest.mark.asyncio
@pytest.mark.parametrize("send_mode", [rcl.UdpSender.Mode.Synchronous, rcl.UdpSender.Mode.Asynchronous])
async def test_udp_sender(send_mode):
    fs=48000
    bs=256

    cc = visr.SignalFlowContext(bs, fs)

    rec_address = "127.0.0.1"
    rec_port = 55001

    comp = rcl.UdpSender(cc, "UDP", None, receiverAddress=rec_address, receiverPort=rec_port,
       mode=send_mode)

    flow=rrl.AudioSignalFlow(comp)
    in_port = flow.parameterReceivePort("messageInput")

    # Create a UDP receiver.
    loop = asyncio.get_event_loop()
    message_received = loop.create_future()
    listen = loop.create_datagram_endpoint(
        lambda: EchoClientProtocol(message_received),
        local_addr=(rec_address, rec_port))
    transport, protocol = loop.run_until_complete(listen)

    msg_data = "Hello world!"
    msg_param = pml.StringParameter(msg_data)
    in_port.enqueue(msg_param)

    flow.process()

    try:
        recv_msg = await asyncio.wait_for(message_received, timeout=0.1)
        assert recv_msg == msg_data
    except asyncio.TimeoutError:
        assert False and "UdpSender: No message received within timeout."
    finally:
        transport.close()


# Enable to run the unit test as a script.
if __name__ == "__main__":
    pytest.main([__file__])
