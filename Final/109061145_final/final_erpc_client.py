from asyncore import read
from time import sleep
import erpc
from final_erpc_service import *
import sys

if __name__ == "__main__":

    if len(sys.argv) != 2:
        print("Usage: python led_test_client.py <serial port to use>")
        exit()

    # Initialize all erpc infrastructure
    xport = erpc.transport.SerialTransport(sys.argv[1], 9600)
    client_mgr = erpc.client.ClientManager(xport, erpc.basic_codec.BasicCodec)
    client = client.Final_ERPCClient(client_mgr)

    # call function
    print("HELLO, welcome to erpc service\n")
    while(1):
        data = int(input("Encoder data insert 1, Ping data insert 2: "))
        if (data == 1):
            print("Traveling distance:    " + str(client.Get_data_from_mbed(1)) + "  cm")
        elif (data == 2):
            print("Ping with distance:    " + str(client.Get_data_from_mbed(2)) + "  cm")
        else:
            print ("wrong console, enter one more time")
    print("\n")
