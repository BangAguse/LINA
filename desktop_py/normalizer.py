def normalize_network_data(raw: dict) -> dict:
    # Normalize keys and ensure consistent shapes
    norm = {
        'networks': [],
        'devices': [],
        'protocols': raw.get('protocols', {}),
        'interfaces': raw.get('interfaces', [])
    }

    for n in raw.get('networks', []):
        norm['networks'].append({
            'ssid': n.get('ssid') or '(hidden)',
            'signal_strength': int(n.get('signal_strength') or 0),
            'security': n.get('security') or ''
        })

    for d in raw.get('devices', []):
        norm['devices'].append({
            'ip': d.get('ip',''),
            'mac': d.get('mac',''),
            'iface': d.get('iface',''),
            'info': d.get('info','')
        })

    # Interface stats if present
    if 'interfaces_stats' in raw:
        norm['interfaces_stats'] = raw['interfaces_stats']

    return norm
