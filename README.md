# ft_traceroute

## Overview

`Traceroute` is a network diagnostic tool that traces the path of packets from your computer to a specified destination. It helps to identify the route taken by packets through the network, displaying each hop along the way, along with the time it takes for each packet to travel. This implementation of `traceroute` sends UDP packets to the target and listens for ICMP responses from routers along the path.

## Features

- Traces the path of packets to a specified destination.
- Displays the IP address and response time for each hop.
- Handles multiple hops and shows average latency.
- Supports custom TTL values to control the number of hops.
- Compatible with IPv4.

## How It Works

Traceroute works by sending UDP packets with incrementally increasing TTL (Time To Live) values. Each router along the path decreases the TTL value by one. When the TTL reaches zero, the router returns an ICMP "Time Exceeded" message. This allows the traceroute tool to identify the hop and measure the time taken to reach that point.

### Steps:

1. **Create UDP and RAW sockets**: The UDP socket sends packets while the RAW socket listens for ICMP responses.
2. **Incremental TTL**: The tool sends multiple packets, starting with TTL = 1, incrementing by one each time, up to the maximum specified TTL.
3. **Capture responses**: Each ICMP response reveals the hop and the time taken, displaying the IP address and latency.
4. **Stop on target reach**: The tool stops once it reaches the target IP address.

## Installation

To install and compile the project, follow these steps:

```bash
# Clone the repository
git clone https://github.com/Lucas-Ve/traceroute.git

# Navigate into the project directory
cd traceroute

# Compile the code
make
```

## Usage

After compiling, you can run `traceroute` using the following command:

```bash
sudo ./ft_traceroute [OPTIONS] <destination>
```

### Options

- `--help`: Display help information.
- `<destination>`: Specify the target hostname or IP address to trace.

## Requirements

- A Linux environment (tested on Ubuntu, Debian).
- GCC compiler.
- Root privileges (required to create RAW sockets).