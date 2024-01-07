import socket
import wave
import struct

def increase_volume(data, volume_factor, width, channels):
    format = {1: 'B', 2: 'h', 4: 'i'}[width] * channels
    count = len(data) // struct.calcsize(format)
    unpacked_data = struct.unpack(format * count, data)
    packed_data = []

    for sample in unpacked_data:
        # Increase volume with clipping
        new_sample = max(min(int(sample * volume_factor), 2**(8*width-1)-1), -(2**(8*width-1)))
        packed_data.append(new_sample)

    return struct.pack(format * count, *packed_data)

# Server settings
TCP_IP = '192.168.1.134'
TCP_PORT = 5005
BUFFER_SIZE = 1024  # Adjust as needed

# WAV file settings
CHANNELS = 1
RATE = 16000  # Sample rate
WIDTH = 4     # Sample width (32-bit)
VOLUME_FACTOR = 2**14

counter = 1  # Initialize a counter for file naming

# Create socket
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
s.bind((TCP_IP, TCP_PORT))
s.listen(1)

while True:

    conn, addr = s.accept()
    print(f"Connection address: {addr}")

    # Create a new wave file with an incremental name
    output_filename = f'input{counter}.wav'
    wf = wave.open(output_filename, 'wb')
    wf.setnchannels(CHANNELS)
    wf.setsampwidth(WIDTH)
    wf.setframerate(RATE)

    buffer = b''
    try:
        while True:
            data = conn.recv(BUFFER_SIZE)
            if not data: break
            buffer += data

            # Process complete frames
            frame_size = WIDTH * CHANNELS
            while len(buffer) >= frame_size:
                frame = buffer[:frame_size]
                buffer = buffer[frame_size:]
                louder_frame = increase_volume(frame, VOLUME_FACTOR, WIDTH, CHANNELS)
                wf.writeframes(louder_frame)
    finally:
        wf.close()
        conn.close()
        # s.close()
        counter += 1  # Increment the counter for the next iteration

    print(f"Audio saved to {output_filename}")
