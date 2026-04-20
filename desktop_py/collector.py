import subprocess
import json
import platform
import re


def _run_cmd(cmd):
    try:
        out = subprocess.check_output(cmd, shell=True, stderr=subprocess.DEVNULL, timeout=4)
        return out.decode('utf-8', errors='ignore')
    except Exception:
        return ""


def collect_network_metadata():
    """Collect safe, passive network metadata using read-only OS commands.
    This does not perform packet inspection or active probing.
    """
    data = {
        'networks': [],
        'devices': [],
        'protocols': {},
        'interfaces': []
    }

    # 1) IP/Interface info
    ip_addr = _run_cmd('ip -o addr')
    for line in ip_addr.splitlines():
        parts = line.split()
        if len(parts) >= 4:
            iface = parts[1]
            data['interfaces'].append({'iface': iface, 'raw': line})

    # 2) ARP / neighbor table
    neigh = _run_cmd('ip neigh')
    for line in neigh.splitlines():
        # Format: 192.168.1.1 dev eth0 lladdr aa:bb:cc:dd:ee:ff REACHABLE
        m = re.match(r"(\S+)\s+dev\s+(\S+)\s+lladdr\s+(\S+)\s*(.*)", line)
        if m:
            ip, iface, mac, extra = m.groups()
            data['devices'].append({'ip': ip, 'mac': mac, 'iface': iface, 'info': extra})

    # 3) Attempt to read WiFi scan list via nmcli if available (read-only)
    nmcli = _run_cmd('nmcli -t -f SSID,SIGNAL,SECURITY dev wifi list')
    if nmcli:
        for line in nmcli.splitlines():
            parts = line.split(':')
            if len(parts) >= 2:
                ssid = parts[0]
                signal = parts[1]
                security = parts[2] if len(parts) > 2 else ''
                try:
                    sig = int(signal)
                except Exception:
                    sig = 0
                data['networks'].append({'ssid': ssid, 'signal_strength': sig, 'security': security})

    # 4) Protocol summary via ss
    ss_out = _run_cmd('ss -s')
    if ss_out:
        data['protocols']['summary'] = ss_out.strip()

    # 5) Lightweight traffic volume hints via /proc/net/dev
    try:
        with open('/proc/net/dev','r') as f:
            lines = f.readlines()
            for l in lines[2:]:
                parts = l.split()
                if len(parts) >= 17:
                    iface = parts[0].strip(':')
                    rx = int(parts[1])
                    tx = int(parts[9])
                    data.setdefault('interfaces_stats', []).append({'iface': iface, 'rx_bytes': rx, 'tx_bytes': tx})
    except Exception:
        pass

    # Ensure deterministic output
    return data
