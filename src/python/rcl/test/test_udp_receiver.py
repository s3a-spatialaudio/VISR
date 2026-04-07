import pytest

import visr
import rcl
import pml
import rrl

asyncio = pytest.importorskip("asyncio")
nest_asyncio = pytest.importorskip("nest_asyncio")

nest_asyncio.apply()


class ClientProtocol:
    def __init__(self):
        self.transport = None

    def connection_made(self, transport):
        self.transport = transport
        print("Connection established")

    def datagram_received(self, data, addr):
        print("Received:", data.decode())

    def error_received(self, exc):
        print("Error received:", exc)

    def connection_lost(self, exc):
        print("Connection closed")
        self.on_con_lost.set_result(True)


@pytest.mark.asyncio
@pytest.mark.parametrize(
    "receive_mode",
    [rcl.UdpReceiver.Mode.Synchronous, rcl.UdpReceiver.Mode.Asynchronous],
)
async def test_udp_receicer(receive_mode):
    fs = 48000
    bs = 256

    num_blocks = 16
    wait_time = 0.01

    cc = visr.SignalFlowContext(bs, fs)

    udp_port = 5005
    local_address = "127.0.0.1"
    local_port = udp_port + 1

    transport = None
    try:
        # Create a UDP sender.
        loop = asyncio.get_event_loop()
        listen = loop.create_datagram_endpoint(
            lambda: ClientProtocol(), local_addr=(local_address, local_port)
        )
        transport, protocol = loop.run_until_complete(listen)

        comp = rcl.UdpReceiver(cc, "UDP", None, port=udp_port, mode=receive_mode)

        flow = rrl.AudioSignalFlow(comp)
        out_port = flow.parameterSendPort("messageOutput")

        msg_data = "Hello VISR!"
        transport.sendto(msg_data.encode(), (local_address, udp_port))

        received = False
        for idx in range(num_blocks):
            flow.process()
            if out_port.size() > 0:
                param = out_port.front()
                assert isinstance(param, pml.StringParameter)
                assert param.str == msg_data
                out_port.pop()
                received = True
                break
            await asyncio.sleep(wait_time)
        assert received and "UdpReceiver: No message received within timeout"
    finally:
        if transport is not None:
            transport.close()


# Enable to run the unit test as a script.
if __name__ == "__main__":
    pytest.main([__file__])
