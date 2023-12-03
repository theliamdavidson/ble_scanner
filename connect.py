import asyncio
import numpy as np
from bleak import BleakClient




#FSR_uuid = "5ac58ee0-c485-4692-a31d-efb2643ccd5a" 
uuidDict = {
    "FSR1" : "50000000-0000-0000-0000-efb2643ccd5a",
    "FSR2" : "50000001-0000-0000-0000-efb2643ccd5a"
}


def encode_bytes_to_string(bytes_):
    # Create a template for how to slice up the bytes
    data_segments = np.dtype([
        ('fsr', np.uint8)
        ])
    formatted_data = np.frombuffer(bytes_, dtype=data_segments)
    return formatted_data

def notification_callback(sender, data):
    for fsr, uuid in uuidDict.items():
        if uuid.split("-")[0] == str(sender).split("-")[0]:
            print(f'{fsr} value: {encode_bytes_to_string(data)[0][0]}%')

async def main():
    # Connect to the Bluetooth device
    leg_address = "08:3A:F2:B5:BB:86"
    async with BleakClient(leg_address) as client:
        print(f"Client connection: {client.is_connected}")
        # Read the Battery Characteristic
        await client.start_notify(uuidDict["FSR1"], notification_callback)
        await client.start_notify(uuidDict["FSR2"], notification_callback)
        await asyncio.sleep(100)


asyncio.run(main())